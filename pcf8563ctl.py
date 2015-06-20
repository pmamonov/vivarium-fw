#!/usr/bin/python

import sys, time
sys.path.append("/home/pi/RTC_SDL_PCF8563")
import SDL_PCF8563

rtc = SDL_PCF8563.SDL_PCF8563(twi = 1, addr = 0x51)

if len(sys.argv) > 1:
	if sys.argv[1] == "set":
		print "Setting pcf8563 to local time"
		rtc.write_now()

t = rtc.read_datetime()
print t.strftime("%Y-%m-%d %H:%M:%S")
