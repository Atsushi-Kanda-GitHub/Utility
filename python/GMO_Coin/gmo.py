import requests
import json
import hmac
import hashlib
import time
import websocket
from datetime import datetime
from pprint import pprint


class GMOWebsocket:
    def __init__(self):
        websocket.enableTrace(True)
        self.ws = websocket.WebSocketApp("wss://api.coin.z.com/ws/public/v1",
                                         on_open=self.on_open,
                                         on_message=self.on_message,
                                         on_error=self.on_error,
                                         on_close=self.on_close)

    def run_forever(self):
        self.ws.run_forever()

    def on_message(self, ws, message):
        ask_list  = message["asks"]
        bids_list = message["bids"]
        
        pprint(json.loads(message))

    def on_error(self, error):
        print(error)

    def on_close(self, ws):
        print("socket close")

    def on_open(self, ws):
        message = {"command": "subscribe", "channel":"orderbooks", "symbol": "BTC"}
        self.ws.send(json.dumps(message))



gmo_ws = GMOWebsocket()
gmo_ws.run_forever()
