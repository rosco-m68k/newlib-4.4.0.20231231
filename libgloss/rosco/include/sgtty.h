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
 * Legacy (or BSD 🤷) SGTTY "support"
 * ------------------------------------------------------------
 */

#ifndef __ROSCOM68K_LIBC_SGTTY_H__
#define __ROSCOM68K_LIBC_SGTTY_H__

struct sgttyb {
    char sg_ispeed;
    char sg_ospeed;
    char sg_erase;
    char sg_kill;
    int sg_flags;
};

#ifdef __cplusplus
extern "C" {
#endif

// Both of these will just error with ENOSYS...
extern int gtty (int __fd, struct sgttyb *__params);
extern int stty (int __fd, const struct sgttyb *__params);

#ifdef __cplusplus
}
#endif

#endif//__ROSCOM68K_LIBC_SGTTY_H__
