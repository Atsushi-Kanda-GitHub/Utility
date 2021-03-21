import requests
import json
import hmac
import hashlib
import time
import websocket
from datetime import datetime
from pprint import pprint




def is_open():
    """
        GMO市場がOpenしているかどうか
        Returns:
        -----
            True : OPEN  False : Close
    """
    url = "https://api.coin.z.com/public/v1/status"
    status = requests.get(url).json()
    if status["status"] == 0 and status["data"]["status"] == "OPEN":
        return True

    return False


def latest_rate(symbol="BTC"):
    """
        最新rate取得
        Parameters:
            symbol : str
        Returns:
        -----
            rate : dic
    """
    url = f"https://api.coin.z.com/public/v1/ticker?symbol={symbol}"
    status = requests.get(url).json()
    return status["data"][0]


def board_info(symbol="BTC"):
    """
        板情報取得
        Parameters:
            symbol : str
        Returns:
        -----
            rate : dic
    """
    url = f"https://api.coin.z.com/public/v1/orderbooks?symbol={symbol}"
    status = requests.get(url).json()
    if status["status"] == 0:
        return status["data"]
    return None


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
