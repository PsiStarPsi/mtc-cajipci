#!/bin/bash
source scripts/chardev_config.sh
/sbin/rmmod cajipci
rm -f /dev/${device}0
