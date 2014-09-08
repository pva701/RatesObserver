# -*- coding: utf-8 -*-

import scrapy


class LineItem(scrapy.Item):
    title = scrapy.Field()
    timestamp = scrapy.Field()
    href = scrapy.Field()
    teams = scrapy.Field()
    win1 = scrapy.Field()
    draw = scrapy.Field()
    win2 = scrapy.Field()