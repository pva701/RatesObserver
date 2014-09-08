# -*- coding: utf-8 -*-


BOT_NAME = 'williamhill'

SPIDER_MODULES = ['williamhill.spiders']
NEWSPIDER_MODULE = 'williamhill.spiders'

# use custom user-agent list
DOWNLOADER_MIDDLEWARES = {
    'scrapy.contrib.downloadermiddleware.useragent.UserAgentMiddleware': None,
    'williamhill.rotate_useragent.RotateUserAgentMiddleware': 400
}

RETRY_TIMES = 4