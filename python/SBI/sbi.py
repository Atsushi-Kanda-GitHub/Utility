# coding: UTF-8
import sys
import time
import requests
import threading
from datetime import datetime
from bs4 import BeautifulSoup

class SBI:
  BASE_URL  = "https://site1.sbisec.co.jp/ETGate/"
  PORTFOLIO = "?_ControlID=WPLETpfR001Control&_PageID=DefaultPID&_DataStoreID=DSWPLETpfR001Control&_ActionID=DefaultAID&getFlg=on"

  def __init__(self, outpath):
    self.outpath = outpath
    self.session = requests.session()

  """Login SBI"""
  def loginSBI(self, user_id, pw):
    USER_ID = "user_id";
    USER_PW = "user_password"

    param = self.createLoginParam()
    param[USER_ID] = user_id
    param[USER_PW] = pw

    res = self.session.post(SBI.BASE_URL, data=param)
    res.raise_for_status()

  """get Portfolio"""
  def getPortfolio(self):
    HEADER = "<!--▽明細部-->"
    HOOTER = "<!--△明細部-->"

    res = self.session.get(SBI.BASE_URL + SBI.PORTFOLIO)
    res.encoding = "cp932"

    start_pos  = res.text.find(HEADER)
    end_pos    = res.text.find(HOOTER)
    table_html = res.text[start_pos : end_pos]
    bs = BeautifulSoup(table_html, "lxml")
    table_info = bs.findAll("table")[1].findAll("table")[0]

    data = ""
    timestamp = self.createDatetime()
    rows = table_info.findAll("tr")
    for row_index in range(1, len(rows)):
      cols = rows[row_index].findAll("td")
      if len(cols) <= 1:
        continue
      
      data += timestamp + "\t"
      for col_index in range(1, 8):
        if col_index == 1:
          data += cols[col_index].text[0:5] +"\t"
        else:
          data += cols[col_index].text + "\t"
      data = data[:-1] +"\n"
    
    with open(self.outpath, "a") as f:
      f.write(data)

  """Create Post Parameter in Login"""
  def createLoginParam(self):
    FORM_START = "<form name=\"form_login\""
    FORM_END   = "</form>"
    INPUT_STR  = "input"
    NAME_STR   = "name"
    VALUE_STR  = "value"

    res = self.session.get(SBI.BASE_URL)

    start_pos = res.text.find(FORM_START)
    end_pos   = res.text.find(FORM_END, start_pos)

    form = res.text[start_pos : end_pos + len(FORM_END)]
    soup = BeautifulSoup(form, "lxml")
    login_form = soup.findAll(INPUT_STR)

    params = {}
    for param in login_form:
      if param.has_attr(NAME_STR) and param.has_attr(VALUE_STR):
        params[param[NAME_STR]] = param[VALUE_STR]

    return params

  def createDatetime(self):
    return datetime.now().strftime("%H:%M:%S")


if __name__ == "__main__":
  if len(sys.argv) < 3:
    print("Error Parameter")
    sys.exit()

  sbi = SBI("data/" + datetime.now().strftime("%Y-%m-%d") + ".txt")
  sbi.loginSBI(sys.argv[1], sys.argv[2])

  sbi_timer = threading.Timer(10, sbi.getPortfolio)
  sbi_timer.start()

