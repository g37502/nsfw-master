#!/usr/bin/env python
# coding=utf-8
'''
@auhor: gyl
@file: .py
@data: 2021/5/20 
@desc:
'''
# import lib
import os
import subprocess
from config import config_h
# pathshell="/home/pebai/TxtFilter.v2.0715/TxtFilterTest/Debug/TxtFilterTest"
# ter="/home/pebai/TxtFilter.v2.0715/termList.dat"
# mod="/home/pebai/TxtFilter.v2.0715/model9.v2.dat"
# file="/home/pebai/TxtFilter.v2.0715/samplesFilder/大干杨钰莹-人妻交换.html"
pathshell=config_h.get_config('check','pathshell')
ter=config_h.get_config('check','ter')
mod=config_h.get_config('check','mod')

def check_file(file,pathshell=pathshell,ter=ter,mod=mod,):
	comment = f'{pathshell} {ter} {mod} {file}'
	rc, out = subprocess.getstatusoutput(comment)
	# print('结果是：',rc,'使用时间:',out)
	return rc