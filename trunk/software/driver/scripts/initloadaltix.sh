#!/bin/bash
# Loads the cajipci driver
#

CAJIPCI_PATH=/usr/local/mtc/altix2

### BEGIN INIT INFO
# Provides:          cajipci
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Loads and sets up cajipci driver.
# Description:       Enables clocking and triggers
### END INIT INFO

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
