#!/bin/bash
# Loads the cajipci driver
#

CAJIPCI_PATH=/usr/local/mtc/altix2

case "$1" in
start)
	cd $CAJIPCI_PATH; scripts/chardev_load.sh
;;

stop)
	cd $CAJIPCI_PATH; scripts/chardev_unload.sh
;;

restart)
  	$0 stop
  	$0 start
;;

status)
              [ -f /dev/altixpci0 ] && echo "Driver Loaded and ready" || echo "Driver not loaded"
;;

*)
        echo "Usage: $0 {start|stop|restart|status}"
        exit 1
esac
