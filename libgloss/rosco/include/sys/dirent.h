/*
 * vim: set et ts=4 sw=4
 *------------------------------------------------------------
  *                                  ___ ___ _   
 *  ___ ___ ___ ___ ___       _____|  _| . | |_ 
 * |  _| . |_ -|  _| . |     |     | . | . | '_|
 * |_| |___|___|___|___|_____|_|_|_|___|___|_,_| 
 *                     |_____|           stdlibs
* ------------------------------------------------------------
 * Copyright (c) 2024 Ross Bamford & Contributors
 * MIT License
 *
 * Minimal POSIX dirent.h for rosco_m68k
 * ------------------------------------------------------------
 */

#ifndef __ROSCO_M68K_SYS_DIRENT_H
#define __ROSCO_M68K_SYS_DIRENT_H

#include <stdint.h>
#include <sys/_types.h>
#include <sys/syslimits.h>
#include <rosco_m68k/fl_dirs.h>

#ifndef _INO_T_DECLARED
typedef	__ino_t		ino_t;
#define	_INO_T_DECLARED
#endif

#ifndef _OFF_T_DECLARED
typedef	__off_t		off_t;
#define	_OFF_T_DECLARED
#endif

struct dirent {
    // POSIX
    ino_t       d_ino;                  /* inode number */
    char        d_name[NAME_MAX + 1];   /* filename */

    // Non-POSIX
    uint8_t     d_is_dir;
    uint32_t    d_size;
};


typedef struct {
    struct fat_dir_list_status  fl_dir;
    struct dirent               dirent;
} DIR;

#endif