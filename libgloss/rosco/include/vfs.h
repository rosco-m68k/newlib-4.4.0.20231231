/*
 *------------------------------------------------------------
 *                                  ___ ___ _   
 *  ___ ___ ___ ___ ___       _____|  _| . | |_ 
 * |  _| . |_ -|  _| . |     |     | . | . | '_|
 * |_| |___|___|___|___|_____|_|_|_|___|___|_,_| 
 *                     |_____|           stdlibs
 * ------------------------------------------------------------
 * Copyright (c)2019-2024 Ross Bamford & contributors
 * See rosco_m68k LICENSE.md for licence information.
 *
 * low-level "VFS" layer for newlib
 * 
 * NOT PUBLIC API!
 * ------------------------------------------------------------
 */
#ifndef ___ROSCOM68K_LIBC_VFS_H__
#define ___ROSCOM68K_LIBC_VFS_H__

#include <stdint.h>
#include <string.h>

#define STDIN_FN        "stdin"
#define STDOUT_FN       "stdout"
#define STDERR_FN       "stderr"
#define SD_FN_PREFIX    "/sd/"

#define EXTRACT_SD_NAME(name)     ((name + (strlen(SD_FN_PREFIX) - 1)))

#define REAL_FILE_OFS   32

#endif //___ROSCOM68K_LIBC_VFS_H__
