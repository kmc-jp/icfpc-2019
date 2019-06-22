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

def put_coininfo():
  blockinfo = subprocess.check_output(['./lambda-cli.py','getblockchaininfo']).decode()
  balance = subprocess.check_output(['./lambda-cli.py','getbalance']).decode()
  balances = subprocess.check_output(['./lambda-cli.py','getbalances']).decode()

  res = u"BCInfo: {}Balance: {}Balances: {}".format(
    blockinfo, balance, balances
  )
  post(res)

  block = int(json.loads(blockinfo.replace("'", '"'))["block"])
  db = ETN()
  if int(db.get("last_block")) != int(block):
    db.set("last_block", int(block))
    info = subprocess.check_output(['./lambda-cli.py','getmininginfo']).decode()
    post(u"new block\n>>>\n"+info)

import subprocess
schedule.every(5).minutes.do(put_coininfo)

put_coininfo()

while True:
    schedule.run_pending()
    time.sleep(10)
