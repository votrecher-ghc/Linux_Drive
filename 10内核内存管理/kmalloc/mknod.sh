#!/bin/bash

sudo mknod /dev/mychar c 11 0
sudo chmod a+w /dev/mychar
ls /dev/mychar -l

