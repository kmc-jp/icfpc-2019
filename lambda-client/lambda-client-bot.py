#!/usr/bin/env python3
import os
import dotenv
dotenv_path = os.path.join(os.path.dirname(__file__), '.env')
dotenv.load_dotenv(dotenv_path)

import schedule
import requests
import json
import time

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

import subprocess
schedule.every(5).minutes.do(put_coininfo)

put_coininfo()

while True:
    schedule.run_pending()
    time.sleep(10)
