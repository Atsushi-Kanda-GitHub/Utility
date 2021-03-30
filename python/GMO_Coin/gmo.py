import requests
import json
import hmac
import hashlib
import time
import websocket
from datetime import datetime
from pprint import pprint
from dataclasses import dataclass

@dataclass
class PositionInfo:
    """
        Position Info
        symbol   : Coin種類
        order_id : 注文番号
        size     : 数量
        contract : True : 約定済み Flase : 注文中
    """
    symbol: str
    order_id : int
    size: str
    contract : bool


class GMOWebsocket:
    def __init__(self):
        self.api_key    = ""
        self.secret_key = ""
        self.total_gain = 0.0
        self.position   = None

        websocket.enableTrace(True)
        self.ws = websocket.WebSocketApp("wss://api.coin.z.com/ws/public/v1",
                                         on_open=self.on_open,
                                         on_message=self.on_message,
                                         on_error=self.on_error,
                                         on_close=self.on_close)

    def run_forever(self):
        self.ws.run_forever()

    def on_message(self, ws, message):
        res = json.loads(message)
        asks = res["asks"]
        bids = res["bids"]
        pprint(asks)
        pprint(bids)

    def on_error(self, error):
        print(error)

    def on_close(self, ws):
        print("socket close")

    def on_open(self, ws):
        message = {"command": "subscribe", "channel":"orderbooks", "symbol": "BTC_JPY"}
        self.ws.send(json.dumps(message))

    def get_private_info(self, target):
        """
            ユーザー情報取得
            Parameter
            -----
                target : str
                    "margin" : 余力情報
                    "assets" : 資産残高
                    "orders" : 注文情報
                    "positionSummary": 建玉Summery
        """
        time_stamp = "{0}000".format(int(time.mktime(datetime.now().timetuple())))
        method     = "GET"
        end_point  = "https://api.coin.z.com/private"
        path       = f"/v1/{target}"

        text = time_stamp + method + path
        sign = hmac.new(bytes(self.secret_key.encode("ascii")), bytes(text.encode("ascii")), hashlib.sha256).hexdigest()
        headers = {
            "API-KEY"       : self.api_key,
            "API-TIMESTAMP" : time_stamp,
            "API-SIGN"      : sign
        }

        parameters = {"symbol": "BTC_JPY"} if target == "positionSummary" else None
        res = requests.get(end_point + path, headers=headers, params=parameters)
        print (json.dumps(res.json(), indent=2))

    def get_contract_info(self):
        """
            約定情報取得
        """
        if self.position is NOne:
            return

        time_stamp = "{0}000".format(int(time.mktime(datetime.now().timetuple())))
        method     = "GET"
        end_point  = "https://api.coin.z.com/private"
        path       = "/v1/executions"

        text = time_stamp + method + path
        sign = hmac.new(bytes(self.secret_key.encode("ascii")), bytes(text.encode("ascii")), hashlib.sha256).hexdigest()
        parameters = {
            "orderId": str(self.position.order_id)
        }

        headers = {
            "API-KEY": self.api_key,
            "API-TIMESTAMP": time_stamp,
            "API-SIGN": sign
        }

        res = requests.get(end_point + path, headers=headers, params=parameters)
        if res[status] != 0:
            return
        total_size = 0.0
        for execution in res["data"]["list"]:
            if execution["orderId"] == self.position.order_id:
                self.total_gain += int(execution["lossGain"])
                total_size += float(execution["size"])
        if self.position.size == total_size:
            self.position = None

    def order(self, symbol, side, execution, size, price):
        """
            注文
            Parameter
            ------
            symbol : str
                BTC     : BitCoin
                BTC_JPY : BitCoin Leverage
                XRP     : Ripple
                XRP_JPY : Ripple Leverage
            side : str
                BUY  : 買建
                SELL : 売立
            execution : str
                MARKET : 成行
                LIMIT  : 指値
                STOP   : 逆指値
            size : str
                注文数量
            price : str
                価格
            Returns
            -----
                OrderID: int
        """
        time_stamp = "{0}000".format(int(time.mktime(datetime.now().timetuple())))
        method     = "POST"
        end_point  = "https://api.coin.z.com/private"
        path       = "/v1/order"
        body = {
            "symbol"        : symbol,
            "side"          : side,
            "executionType" : execution,
            "price"         : price,
            "size"          : size
        }

        text = time_stamp + method + path + json.dumps(body)
        sign = hmac.new(bytes(self.secret_key.encode("ascii")), bytes(text.encode("ascii")), hashlib.sha256).hexdigest()

        headers = {
            "API-KEY"       : self.api_key,
            "API-TIMESTAMP" : time_stamp,
            "API-SIGN"      : sign
        }

        res = requests.post(end_point + path, headers=headers, data=json.dumps(body))
        self.position = PositionInfo(symbol, int(res["data"]), size, False)

    def cancel_order(self):
        """
            Cancel Order
            Parameter
            -----
            order_id : int
                OrderID
        """
        time_stamp = "{0}000".format(int(time.mktime(datetime.now().timetuple())))
        method    = "POST"
        end_point  = "https://api.coin.z.com/private"
        path       = "/v1/cancelOrder"
        body = {
            "orderId": self.position.order_id
        }

        text = time_stamp + method + path + json.dumps(body)
        sign = hmac.new(bytes(self.secret_key.encode("ascii")), bytes(text.encode("ascii")), hashlib.sha256).hexdigest()

        headers = {
            "API-KEY"       : self.api_key,
            "API-TIMESTAMP" : time_stamp,
            "API-SIGN"      : sign
        }

        res = requests.post(end_point + path, headers=headers, data=json.dumps(body))
        self.position = None

    def cancel_all_order(self):
        """
            Cancel All Order
        """
        time_stamp = "{0}000".format(int(time.mktime(datetime.now().timetuple())))
        method    = "POST"
        end_point  = "https://api.coin.z.com/private"
        path      = "/v1/cancelBulkOrder"
        body = {
            "symbols" : ["BTC","BTC_JPY"]
        }

        text = time_stamp + method + path + json.dumps(body)
        sign = hmac.new(bytes(self.secret_key.encode("ascii")), bytes(text.encode("ascii")), hashlib.sha256).hexdigest()

        headers = {
            "API-KEY"       : self.api_key,
            "API-TIMESTAMP" : time_stamp,
            "API-SIGN"      : sign
        }

        res = requests.post(end_point + path, headers=headers, data=json.dumps(body))
        self.position = None

    def close_order(self, price):
        """
            決済
            Parameter
            ------
            price : str
                決済金額
        """
        time_stamp = "{0}000".format(int(time.mktime(datetime.now().timetuple())))
        method     = "POST"
        end_point  = "https://api.coin.z.com/private"
        path       = "/v1/closeBulkOrder"
        body = {
            "symbol"        : "BTC_JPY",
            "side"          : "BUY",
            "executionType" : "LIMIT",
            "timeInForce"   : "FAS",
            "price"         : price,
            "size"          : self.position.size
        }

        text = time_stamp + method + path + json.dumps(body)
        sign = hmac.new(bytes(self.secret_key.encode("ascii")), bytes(text.encode("ascii")), hashlib.sha256).hexdigest()

        headers = {
            "API-KEY"       : self.api_key,
            "API-TIMESTAMP" : time_stamp,
            "API-SIGN"      : sign
        }

        res = requests.post(end_point + path, headers=headers, data=json.dumps(body))
        if res["status"] == 0:
            self.position = None


gmo_ws = GMOWebsocket()
gmo_ws.run_forever()
