# -*- coding: utf-8 -*-

import re
import urllib
from scrapy import log
from scrapy.exceptions import DropItem
from items import WilliamhillItem


class WilliamhillPipeline(object):
    def process_item(self, item, spider):
        if isinstance(item, WilliamhillItem):
            return self.process_williamitem(item)
        return item


    def process_williamitem(self, item):
        if not item['timestamp']:
            item['timestamp'] = '0'
        else:
            ts = re.findall('tzTime:br:(\d+)', item['timestamp'])
            if ts:
                item['timestamp'] = ts[0]
            else:
                item['timestamp'] = '0'
        item['team1'] = item['team1'].strip()
        item['team2'] = item['team2'].strip()
        item['href'] = urllib.unquote(item['href'].strip().encode('utf-8'))
        item['win1'] = item['win1'].strip()
        item['draw'] = item['draw'].strip()
        item['win2'] = item['win2'].strip()
        return item
        