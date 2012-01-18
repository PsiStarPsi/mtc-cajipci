#!/bin/bash
source scripts/chardev_config.sh
# Group: since distributions do it differently, look for wheel or use staff
if grep -q '^staff:' /etc/group; then
    group="staff"
else
    group="wheel"
fi

# invoke insmod with all arguments we got
# and use a pathname, as insmod doesn't look in . by default
/sbin/rmmod altix &> /dev/null 
/sbin/insmod altix.ko $* || exit 1

# retrieve major number
major=$(awk -v MODULE=$module '$2==MODULE {print $1;}' /proc/devices)

# Remove stale nodes and replace them, then give gid and perms
# Usually the script is shorter, it's scull that has several devices in it.

rm -f /dev/${device}0
mknod /dev/${device}0 c $major 0
chgrp $group /dev/${device}0
chmod $mode  /dev/${device}0
