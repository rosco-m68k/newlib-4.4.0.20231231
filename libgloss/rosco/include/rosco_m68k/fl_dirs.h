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
 * Public bits of FAT filelib (for directory stuff above and
 * beyond the basics in dirent.h)
 * ------------------------------------------------------------
 */

#ifndef __ROSCOM68K_LIBC_FL_DIRS_H__
#define __ROSCOM68K_LIBC_FL_DIRS_H__

int                 fl_createdirectory(const char *path);
int                 fl_is_dir(const char *path);

#endif