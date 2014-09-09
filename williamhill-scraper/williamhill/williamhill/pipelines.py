# -*- coding: utf-8 -*-

class LinePipeline(object):
    def process_item(self, item, spider):
        if not item['time']:
            item['time'] = '0'
        timestamp = re.findall('tzTime:br:(\d+)', item['time'])[0] or '0'
        
        item['team1'], item['team2'] = [x.strip() for x in item['teams'].split('-')] 

        item['href'] = item['href'].strip()
        item['win1'] = item['win1'].strip()
        item['draw'] = item['draw'].strip()
        item['win2'] = item['win2'].strip()

        return item
