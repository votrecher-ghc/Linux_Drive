#!/bin/bash

sudo mknod /dev/myled_dt c 11 0
sudo chmod a+w /dev/myled_dt
ls /dev/myled_dt -l

