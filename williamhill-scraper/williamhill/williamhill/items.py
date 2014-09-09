# -*- coding: utf-8 -*-

import scrapy


class LineItem(scrapy.Item):
    title = scrapy.Field()
    time = scrapy.Field()
    href = scrapy.Field()
    team1 = scrapy.Field()
    team2 = scrapy.Field()
    win1 = scrapy.Field()
    draw = scrapy.Field()
    win2 = scrapy.Field()