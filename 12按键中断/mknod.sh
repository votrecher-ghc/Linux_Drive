#!/bin/bash

sudo mknod /dev/fs4412key2 c 11 0
sudo chmod a+w /dev/fs4412key2
ls /dev/fs4412key2 -l

