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
 * Low-level terminal IO "support"
 * ------------------------------------------------------------
 */

#include <errno.h>
#undef errno
extern int errno;

int ioctl(int fd, unsigned long request, ...) {
    errno = ENOSYS;
    return -1;
}

