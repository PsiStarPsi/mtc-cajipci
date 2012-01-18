/*! \mainpage Altix Kernel Module documentation
 *
 * \section lic_sec License
 *
 * Copyright(c) 2005-2011 Sergey Negrashov, University of Hawaii, Instrument Development Lab.
 * sin8@hawaii.edu
 * This code distribution is licensed under the terms of of the
 * GNU General Public License Version 2 (the ``GPL'').
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the GPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the GPL along with this
 * program. If not, go to http://www.gnu.org/licenses/gpl.html
 *  or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * \section intro_sec Introduction
 *
 * This is the documentation for the altix kernel driver for the CPCI readout system.
 * It provides support for readout and control of multiple DSP CPCI cards by one or more userland programs.
 *
 * \section install_sec Requirements
 *
 * This Software requires a recent Linux kernel, and a recent C compiler. Furthermore in order to build this software,
 * Linux kernel headers or Linux source tree of the matching version must be available.
 *
 * Prerequisites:
 *  \li Compact PCI crate with an i686 or a X86_64 CPU. (Note: i586/i386 systems such as Pentium Pro are not supported)
 * 	\li Linux kernel of version 2.6.26 or above.
 * 	\li Linux kernel source tree, or Linux kernel headers of same version as the running kernel.
 *  \li gcc compiler of version 4.3 or above. (Note: because this driver uses the kernel build system, other compilers such as Borland C compiler or Intel C compiler are not supported.
 *  \li python 2.5
 *  \li sudo utility
 *
 * Additional Requirements: Because kernel documentation is built from the source code, several tools are required to rebuild this document from the source tree.
 * 	\li doxygen
 *  \li Latex distribution such as TeXLive.
 *  \li Graphviz software for generating dependency graphs.
 *
 * Tested systems:
 *
 *  \li Debian Lenny. Linux Kernel 2.6.26
 * 	\li Debian Squeeze. Linux Kernel 2.6.32, Kernel 2.6.38.
 * 	\li Ubuntu 10.04
 *  \li Ubuntu 10.10
 *
 * In order to build the kernel module run:
 *
 * \a make
 *
 * the output is altix.ko.
 *
 * In order to load the driver run:
 *
 * \a make \a load
 *
 * In order to unload the driver run:
 *
 * \a make \a unload
 *
 * Doxygen and graphviz packages are required to build documentation.
 *
 * To build the documentation type:
 *
 * \a make docs
 *
 * the output latex is in doc/latex, outputs this PDF into the docs folder.
 *
 * \section Device Driver Overview
 *
 * After the driver is loaded, the PCI devices can be accessed via altixpci0 in the /dev folder.  This file can be accessed by any user on the CPU.
 *
 * Since the device is implemented as a character device, standard UNIX system calls may be used to access this device - open, close, read, and write.
 * In addition to these standard calls, multiple IOCTL functions are implemented to handle functions that are not handled directly via system calls.
 *
 * IOCTL calls supported by the driver: (constants are defined in altix_userland.h)
 *
 * ALTIX_IOCTL_NUM (0x881): returns number of cards loaded by the driver.
 *
 * ALTIX_IOCTL_INFO (0x882): returns information about the loaded cards.  A pointer to an array of altix_pci_card_info is returned.
 *
 * ALTIX_IOCTL_LOCK (0x883): Locks a given PCI card.  By locking a card, all read/write operations on the file descriptor are assigned to the currently locked card.
 *
 * ALTIX_IOCTL_RELEASE (0x884): Releases a locked PCI card, so other file descriptors can access it.
 *
 * ALTIX_IOCTL_STAT (0x885): Returns an array of altix_pci_card_stat objects - information about data read and written to the given card.
 *
 * ALTIX_IOCTL_DMA (0x887): Enables or disables DMA mode.  This requires a card to be currently locked.
 *
 * ALTIX_IOCTL_CHAN (0x888): Selects a channel on the currently locked card.
 *
 * ALTIX_IOCTL_VERSION (0x889): Retrieves the current driver version.
 *
 * ALTIX_IOCTL_CHAN_ENABLE (0x88A): Enables or disables the current channel.
 *
 * ALTIX_IOCTL_CHAN_STATUS (0x88B): Retrieves information about the current channel read/write status, e.g. whether data is available on it or not.
 *
 */
/**
 * \file about.h
 * This file maintains module information required for kernel drivers.
 */
#ifndef ALTIX_ABOUT_HEADER
#define ALTIX_ABOUT_HEADER
#include "altix_userland.h"
#include <linux/string.h>
#include <linux/kernel.h>
MODULE_AUTHOR("Sergey Negrashov, Andrew Wong");
MODULE_DESCRIPTION("Driver for CPCI DSP system");
MODULE_VERSION(ALTIX_DRIVER_VERSION_STRING);
MODULE_LICENSE("GPL");
#endif
