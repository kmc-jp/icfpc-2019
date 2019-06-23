#!/usr/bin/env python3
import os
import dotenv
dotenv_path = os.path.join(os.path.dirname(__file__), '.env')
dotenv.load_dotenv(dotenv_path)

import schedule
import requests
import json
import time

class ETN:
    def __init__(self, fn = "./db.json"):
        self.fn = fn
        self.load()
        self.save()
    def get(self, key):
        self.load()
        return self.db[key]
    def set(self, key, value):
        self.load()
        self.db[key] = value
        self.save()
    def load(self):
        if os.path.exists(self.fn):
            with open(self.fn) as f:
                self.db = json.load(f)
        else:
            self.db = json.loads("{}")
    def save(self):
        with open(self.fn, "w") as f:
            json.dump(self.db, f)
db = ETN()
try:
    db.get("last_block")
except KeyError:
    db.set("last_block", 0)

SLACK_INCOMING = os.environ.get("SLACK_INCOMING")
def post(msg):
  requests.post(SLACK_INCOMING, data = json.dumps({
      'text': msg,
      'username': u'LambdaCoin',
      'icon_emoji': u':bitcoin:',
  }))

def jsonloads(i):
  return json.loads(i.replace("'", '"'))

def mining(block):
  info = subprocess.check_output(['./lambda-cli.py','getmininginfo']).decode()
  post(u"new block\n>>>\n"+info)
  info = jsonloads(info)
  with open('problems/{}.json'.format(block), 'w') as f:
      json.dump(info, f)

  puzzlef = 'problems/{}puzzle'.format(block)
  with open(puzzlef, 'w') as f:
      f.write(info["puzzle"])
  taskf = 'problems/{}task'.format(block)
  with open(taskf, 'w') as f:
      f.write(info["task"])

  post(u"start Solving")
  puzzlesolf = 'problems/{}puzzle.sol'.format(block)
  with open(puzzlesolf, "w") as o:
      with open(puzzlef, "r") as i:
          solveerr = subprocess.Popen('cat',stderr=subprocess.PIPE,stdin=i,stdout=o)
          err = solveerr.communicate()[1].decode()
          post(u"Solve Err: " + err)
  with open(puzzlesolf, "r") as o:
      post("Solve Result: "+ o.read())

  post(u"End. start Generating")
  taskdescf = 'problems/{}task.desc'.format(block)
  with open(taskdescf, "w") as o:
      with open(taskf, "r") as i:
          generr = subprocess.Popen('cat',stderr=subprocess.PIPE,stdin=i,stdout=o)
          err = generr.communicate()[1].decode()
          post(u"Gen Err: " + err)
  with open(taskdescf, "r") as o:
      post("Gen Result: "+ o.read())

  post(u"End. start Submit...")
  sub = subprocess.check_output(['echo', './lambda-cli.py','submit',str(block),puzzlesolf,taskdescf]).decode()
  post("Result: "+sub)

def check_mining():
  blockinfo = subprocess.check_output(['./lambda-cli.py','getblockchaininfo']).decode()
  block = int(jsonloads(blockinfo)["block"])
  block = int(jsonloads(blockinfo)["block"])
  db = ETN()
  if int(db.get("last_block")) != int(block):
    print("Start mining")
    mining(block)
    db.set("last_block", int(block))

def put_coininfo():
  blockinfo = subprocess.check_output(['./lambda-cli.py','getblockchaininfo']).decode()
  balance = subprocess.check_output(['./lambda-cli.py','getbalance']).decode()
  balances = subprocess.check_output(['./lambda-cli.py','getbalances']).decode()

  res = u"BCInfo: {}Balance: {}Balances: {}".format(
    blockinfo, balance, balances
  )
  post(res)

import subprocess
schedule.every(1).minutes.do(check_mining)
schedule.every(5).minutes.do(put_coininfo)

put_coininfo()
check_mining()

while True:
    schedule.run_pending()
    time.sleep(10)
