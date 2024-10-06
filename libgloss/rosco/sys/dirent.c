/*
 * vim: set et ts=4 sw=4
 *------------------------------------------------------------
 *                                  ___ ___ _
 *  ___ ___ ___ ___ ___       _____|  _| . | |_
 * |  _| . |_ -|  _| . |     |     | . | . | '_|
 * |_| |___|___|___|___|_____|_|_|_|___|___|_,_|
 *                     |_____|       firmware v2
 * ------------------------------------------------------------
 * Copyright (c) 2024 Ross Bamford & Contributors
 * MIT License
 *
 * POSIX dirent.h implementation for rosco_m68k
 * ------------------------------------------------------------
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include <errno.h>
#undef errno
extern int errno;

#include "rosco_m68k/sdfat.h"
#include "fat_filelib.h"
#include "vfs.h"

static int fat_initialized;

static DIR dirs[32];
static uint32_t dirs_bmp;

#define GUARD_FAT_INIT()                    \
    do {                                    \
        if (!SD_FAT_get_sd_card()) {        \
            if (!SD_FAT_initialize()) {     \
                errno = ENXIO;              \
                return NULL;                \
            }                               \
            fat_initialized = 1;            \
        }                                   \
    } while (0)

static int get_index(DIR *ptr) {
    if (ptr >= dirs && ptr < dirs + (sizeof(dirs) / sizeof(dirs[0]))) {
        return ptr - dirs;
    } else {
        return -1;
    }
}

int closedir(DIR *dir) {
    int num = get_index(dir);

    if (num == -1 || (dirs_bmp & (1 << num)) == 0) {
        errno = EBADF;
        return -1;
    }

    // mark dir entry as not in use
    dirs_bmp &= ~(1 << num);

    // Not used in the filelib, just returns 0, but call it anyway in case 
    // it's used in the future...
    return fl_closedir((FL_DIR*)dir);
}

DIR* opendir(const char *path) {
    GUARD_FAT_INIT();

    if (dirs_bmp == 0xFFFFFFFF) {
        errno = EMFILE;
        return NULL;
    }

    int num = -1;

    for (int i = 0; i < 32; i++) {
        if ((dirs_bmp & (1 << i)) == 0) {
            num = i;
            break;
        }
    }

    if (num == -1) {
        errno = EMFILE;
        return NULL;
    }

    DIR* result = (DIR*)fl_opendir(EXTRACT_SD_NAME(path), (FL_DIR*) &dirs[num]);

    if (result != NULL) {
        // Open ok - mark entry as used
        dirs_bmp |= (1 << num);
    }

    return result;
}

// TODO this doesn't set errno on error, we need to improve the filelib for that...
struct dirent* readdir(DIR *dir) {
    struct fs_dir_ent ent;

    if (fl_readdir((FL_DIR*)dir, &ent) == 0) {
        dir->dirent.d_ino = ent.cluster;
        dir->dirent.d_is_dir = ent.is_dir;
        dir->dirent.d_size = ent.size;

        strncpy(dir->dirent.d_name, ent.filename, NAME_MAX);

        // In case path was >= NAME_MAX chars...
        dir->dirent.d_name[NAME_MAX] = 0;

        return &dir->dirent;
    }

    return NULL;
}

int readdir_r(DIR *dir, struct dirent *dirent, struct dirent **dirent_p) {
    return ENOSYS;
}

void rewinddir(DIR *dir) {
    // TODO this needs implementing, might have to stash the name in the DIR 🙄
}

void seekdir(DIR *dir, long int ofs) {
    // This space intentionally left blank, this is noop currently...
    // TODO implement it, but FAT filelib makes it a PITA.
    //
}

long int telldir(DIR *dir) {
    return -1;
}