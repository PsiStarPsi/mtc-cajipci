#!/bin/bash
source scripts/chardev_config.sh
/sbin/rmmod altix 
rm -f /dev/${device}0
