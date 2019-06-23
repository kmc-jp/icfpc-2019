#!/usr/bin/env python3
from github_webhook import Webhook
from flask import Flask
import os
import sys
import dotenv
dotenv_path = os.path.join(os.path.dirname(__file__), '.env')
dotenv.load_dotenv(dotenv_path)
import requests
SLACK_INCOMING = os.environ.get("SLACK_INCOMING")
def post(msg):
  requests.post(SLACK_INCOMING, data = json.dumps({
      'text': msg,
      'username': u'LambdaCoin',
      'icon_emoji': u':bitcoin:',
  }))

app = Flask(__name__)  # Standard Flask app
webhook = Webhook(app) # Defines '/postreceive' endpoint

@app.route("/")        # Standard Flask endpoint
def hello_world():
  post("Its works http://34.85.73.57:12080")
  return "Its works"

@webhook.hook()        # Defines a handler for the 'push' event
def on_push(data):
  post("Update start")
  info = subprocess.check_output(['./update.sh']).decode()
  post("Update finish: " + info)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=12080)
