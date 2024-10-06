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

#include <stdint.h>

#define FATFS_MAX_LONG_FILENAME     260

typedef struct fat_dir_list_status
{
    uint32_t                sector;
    uint32_t                cluster;
    uint8_t                 offset;
} FAT_DIR;

typedef struct fat_dir_ent
{
    char                    filename[FATFS_MAX_LONG_FILENAME];
    uint8_t                 is_dir;
    uint32_t                cluster;
    uint32_t                size;
    uint16_t                access_date;
    uint16_t                write_time;
    uint16_t                write_date;
    uint16_t                create_date;
    uint16_t                create_time;
} fat_dirent;

FAT_DIR*            fl_opendir(const char* path, FAT_DIR *dir);
int                 fl_readdir(FAT_DIR *dirls, fat_dirent *entry);
int                 fl_closedir(FAT_DIR* dir);

// Extensions
int                 fl_createdirectory(const char *path);
int                 fl_is_dir(const char *path);

#endif