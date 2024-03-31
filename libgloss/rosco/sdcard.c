/*
 *------------------------------------------------------------
 *                                  ___ ___ _   
 *  ___ ___ ___ ___ ___       _____|  _| . | |_ 
 * |  _| . |_ -|  _| . |     |     | . | . | '_|
 * |_| |___|___|___|___|_____|_|_|_|___|___|_,_| 
 *                     |_____|      libraries v1                 
 * ------------------------------------------------------------
 * Copyright (c)2020 Ross Bamford
 * See top-level LICENSE.md for licence information.
 *
 * Additional SD support routines 
 * ------------------------------------------------------------
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "sdfat.h"
#include "part.h"

static SDCard sdcard;
static PartHandle part;
static bool initialized;

static int FAT_media_read(uint32_t sector, uint8_t *buffer, uint32_t sector_count) {
    // TODO Hack - only supporting part 0 for now...
    return Part_read(&part, 0, buffer, sector, sector_count);
}

static int FAT_media_write(uint32_t sector, uint8_t *buffer, uint32_t sector_count) {
#ifdef __WARNING__WILL__CORRUPT__FILESYSTEM__
    // TODO Hack - only supporting part 0 for now...
    return Part_write(&part, 0, buffer, sector, sector_count);
#else
    return 0;
#endif
}

static void __sd_fat_shutdown() {
    if (initialized) {
        fl_shutdown();
    }
}

bool SD_FAT_initialize() {
    if (SD_initialize(&sdcard) != SD_INIT_OK) {
        return false;
    }

    if (Part_init_SD(&part, &sdcard) != PART_INIT_OK) {
        printf("Part init failed\n");
        return false;
    }

    if (!Part_valid(&part, 0)) {
        printf("Part 0 not valid\n");
        return false;
    }

    if (fl_attach_media(FAT_media_read, FAT_media_write) != FAT_INIT_OK) {
        return false;
    }

    initialized = true;

    atexit(__sd_fat_shutdown);
    return true;
}

SDCard* SD_FAT_get_sd_card() {
  if (initialized) {
    return &sdcard;
  } else {
    return NULL;
  }
}
