# -*- coding: utf-8 -*-

import re
import urllib
from scrapy.exceptions import DropItem


class LinePipeline(object):
    def process_item(self, item, spider):
    	if item['timestamp']: 
        	item['timestamp'] = re.findall('tzTime:br:(\d+)', item['timestamp'])[0]
        else:
        	item['timestamp'] = 0
        item['team1'] = item['team1'].strip()
        item['team2'] = item['team2'].strip()
        item['href'] = urllib.unquote(item['href'].strip().encode('utf-8'))
        item['win1'] = item['win1'].strip()
        item['draw'] = item['draw'].strip()
        item['win2'] = item['win2'].strip()
        return item
