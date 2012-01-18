#ifndef ALTIX_CONFIG_HEADER
#define ALTIX_CONFIG_HEADER
/**
 * \file config.h
 * This file contains some configuration options for the driver.
 */
///Debuging Flag
#define ALTIX_DEBUG
///Vendor for the PCI core
#define ALTIX_VEN_ID 0x10EE
///Device ID for the PCI core
#define ALTIX_DEV_ID 0x0001
///PCI BAR for the PCI core, Open-Cores uses 0 for configuration space, so 1 is default.
#define ALTIX_PCI_BAR 0x1
///PCI BAR for the configuration space, Open-Cores uses 0 by default.
#define ALTIX_CONFIG_BAR 0x0

#endif
