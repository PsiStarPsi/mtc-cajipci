#!/bin/bash
# Loads the cajipci driver
#

CAJIPCI_PATH=/usr/local/mtc/cajipci/

### BEGIN INIT INFO
# Provides:          cajipci_clock
# Required-Start:    $cajicpi $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Configures the cajipci clock.
# Description:       Enables Clocking
### END INIT INFO

case "$1" in
start)
	cajipciCLOCKOFF;
	cd $CAJIPCI_PATH/config; /usr/local/bin/cajipciCLOCK reg_encal_off.INI 0 >> /dev/null; /usr/local/bin/cajipciCLOCK reg_encal_on.INI 0 >> /dev/null;
	/usr/local/bin/cajipciCLOCK reg_encal_off.INI 1 >> /dev/null;  /usr/local/bin/cajipciCLOCK reg_encal_on.INI 1 >> /dev/null;
	/usr/local/bin/cajipciCLOCK reg_encal_off.INI 2 >> /dev/null; /usr/local/bin/cajipciCLOCK reg_encal_on.INI 2 >> /dev/null;
;;

stop)
	cajipciCLOCKOFF;
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
