# -*- coding: utf-8 -*-

import scrapy
import re
from scrapy import log

from bets_parser.items import WilliamhillItem


class WilliamhillSpider(scrapy.Spider):
    name = "williamhill"
    allowed_domains = ['sports.williamhill.com']
    start_urls = [
        'http://sports.williamhill.com/bet/ru/betting/y/5/et/%D0%A4%D1%83%D1%82%D0%B1%D0%BE%D0%BB.html',
    ]


    def parse(self, response):
        for li in response.xpath('/html/body/div[1]/div[8]/div[2]/div[3]/ul/*'):
            title = li.xpath('h3/text()').extract()
            if not title:
                continue
            title = title[0].strip().encode('utf-8')
            hrefs = li.xpath('ul//@href').extract()
            if not hrefs:
                continue
            for href in hrefs:
                line = scrapy.Request(href, callback=self.parse_league)
                line.meta['league'] = title
                yield line



    def parse_league(self, response):
        for tr in response.xpath('/html/body/div[1]/div[8]/div[2]/div[3]/div[2]/div[2]/div/div/table/tbody//tr'):
            item = WilliamhillItem()
            try:
                timestamp = tr.xpath('td[1]/span/@id').extract()[0]
            except:
                timestamp = '0'
            log.msg('timestamp: ' + timestamp)
            href = tr.xpath('td[3]//a/@href').extract()[0]
            teams = tr.xpath('td[3]/a/span/text()').extract()[0]
            win1 = tr.xpath('td[5]/div/div[1]/text()').extract()[0]
            draw = tr.xpath('td[6]/div/div[1]/text()').extract()[0]
            win2 = tr.xpath('td[7]/div/div[1]/text()').extract()[0]

            item['league'] = response.meta['league']
            item['timestamp'] = timestamp
            item['href'] = href
            t = [x.strip() for x in teams.split(u'₋')]
            item['team1'] = t[0] or None
            item['team2'] = t[1] or None
            item['win1'] = win1
            item['draw'] = draw
            item['win2'] = win2
            yield item
