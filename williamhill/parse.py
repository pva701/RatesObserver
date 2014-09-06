import requests
import bs4
from constants import *
from random import choice as choice
from bs4 import BeautifulSoup as Bs
from userAgents import UA

from functools import wraps
import time


def timed(f):
    @wraps(f)
    def wrapper(*args, **kwds):
        start = time.time()
        result = f(*args, **kwds)
        elapsed = time.time() - start
        print "%s took %0.1f ms to finish" % (f.__name__, elapsed * 1000)
        return result
    return wrapper


@timed
def request(url, payload=None):
    headers = {'User-agent': choice(UA)}
    return requests.get(url, headers=headers, params=payload).text or None


@timed
def parse_bets_ids(text_data):
    ret = []
    bs = Bs(text_data)
    divs = bs.find_all('div', id=DIV_ID)[0].children
    for div in divs:
        if isinstance(div, bs4.element.Tag):
            tmp = div.attrs['id']
            # format: 'ip_type_1234', need 1234
            bid = tmp.split('_')[2]
            ret.append(bid)
    return ret

@timed
def parse_bet(bid, day):
    payload = {
        'action': 'GoExpandDailyMatches',
        'type': bid,
        'days': day,
    }
    url = URL_FOOTBALL.format(day)
    text = request(url, payload)
    bs = Bs(text)
    #with open('test.html', 'w') as f:
    #    f.write(bs.prettify().encode('utf-8'))
    bet_rows = bs.find_all('tr', class_='rowOdd')
    for row in bet_rows:
        tds = row.find_all('td', )


def main():
    for day in DAYS_TO_PARSE:
        url = URL_FOOTBALL.format(day)
        text = request(url)
        bets_ids = parse_bets_ids(text)
        for bid in bets_ids:
            parse_bet(bid, day)
            break


if __name__ == '__main__':
    main()

