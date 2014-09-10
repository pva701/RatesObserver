#!/usr/bin/python
# -*- coding: utf-8 -*-

import random
import time
import socket
import sys
from scrapy.contrib.downloadermiddleware.retry import RetryMiddleware
from scrapy.conf import settings


class ProxyMiddleware(object):
    def process_request(self, request, spider):
        request.meta['proxy'] = settings.get('HTTP_PROXY')


class RandomUserAgentMiddleware(object):
    def process_request(self, request, spider):
        ua  = random.choice(settings.get('UA'))
        if ua:
            request.headers.setdefault('User-Agent', ua)


class RetryChangeProxyMiddleware(RetryMiddleware):
    def _retry(self, request, reason, spider):
        try:
            tor_c = socket.create_connection(('127.0.0.1', 9051))
            tor_c.send('AUTHENTICATE "{}"\r\nSIGNAL NEWNYM\r\n'.format('lolkatrolka'))
            response = tor_c.recv(1024)
            if response != '250 OK\r\n250 OK\r\n':
                sys.stderr.write('Unexpected response from Tor control port: {}\n'.format(response))
        except Exception, e:
            sys.stderr.write('Error connecting to Tor control port: {}\n'.format(repr(e)))
        return RetryMiddleware._retry(self, request, reason, spider)

