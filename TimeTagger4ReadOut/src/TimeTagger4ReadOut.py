
import numpy as np
import crono_exts.timetagger4vector as tt4v

status = tt4v.init()
if status != 0:
    exit
status = tt4v.config()
if status != 0:
    exit

status = tt4v.start()
if status != 0:
    exit

count = 100

while count > 0:
    print(tt4v.read())
    count = count - 1

tt4v.stop()
tt4v.close()