/*
 *------------------------------------------------------------
 *                                  ___ ___ _   
 *  ___ ___ ___ ___ ___       _____|  _| . | |_ 
 * |  _| . |_ -|  _| . |     |     | . | . | '_|
 * |_| |___|___|___|___|_____|_|_|_|___|___|_,_| 
 *                     |_____|           stdlibs               
 * ------------------------------------------------------------
 * Copyright (c)2024 Ross Bamford
 * See top-level LICENSE.md for licence information.
 *
 * ioctl "support"
 * ------------------------------------------------------------
 */

#ifndef __ROSCOM68K_LIBC_IOCTL_H__
#define __ROSCOM68K_LIBC_IOCTL_H__

#define TIOCFLUSH   0x10

#ifdef __cplusplus
extern "C" {
#endif

// Just errors with ENOSYS...
int ioctl(int fd, unsigned long request, ...);

#ifdef __cplusplus
}
#endif

#endif//__ROSCOM68K_LIBC_IOCTL_H__



