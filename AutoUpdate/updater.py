#!/usr/bin/python
#-*- coding=utf-8

import os, os.path, json, hashlib
import requests
import subprocess


URL_UPDATE_VERSION = 'http://127.0.0.1/version.json'
FILE_UPDATER = 'updater.py'
FILE_HEX = 'binray.hex'


def md5(fileanme):
    return hashlib.md5(open(fileanme, 'rb').read()).hexdigest()

def check_md5(filename, hash_file):
    if not os.path.isfile(filename):
        return False
    return md5(filename) == hash_file

def download(url, filename):
    r = requests.get(url, stream=True)
    chunk_size = 1024
    with open(filename, 'wb') as fd:
        for chunk in r.iter_content(chunk_size):
            fd.write(chunk)
        fd.close()

def get_remote_version():
    return json.loads(requests.get(URL_UPDATE_VERSION).text)


def update(filename, hash_file, url):
    if check_md5(filename, hash_file):
        print '[INFO] %s version is the newest' % (hash_file)
    else:
        download(url, filename)
        if check_md5(filename, hash_file):
            print '[INFO] Updating %ssuccessfully' % (filename)
        else:
            raise Exception('Updateing %s faild' % (filename))

fields = get_remote_version()

try:
    update(FILE_UPDATER, fields['md5_updater'], fields['link_updater'])
    update(FILE_HEX, fields['md5_binary'], fields['link_binary'])
    cmd = 'avrdude -c linuxgpio -C /etc/avrdude.conf -p m32u4 -U flash:w:binray.hex -Uflash:w:$1 $2'
    ret_code = subprocess.call(cmd, shell=True)
    if ret_code == 1:
        print 'Success'
    else:
        print 'Failed'
except:
    raise








