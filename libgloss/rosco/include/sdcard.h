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
 * SD Card routines for libc.
 * ------------------------------------------------------------
 */
#ifndef __ROSCOM68K_LIBC_SDCARD_H__
#define __ROSCOM68K_LIBC_SDCARD_H__

#include "sdcard.h"

bool SD_get_csd(SDCard *sd, SDCard_CSD *csd;

uint32_t SD_get_size(SDCard *sd);

static int FAT_media_read(uint32_t sector, uint8_t *buffer, uint32_t sector_count) {
    for(int i = 0; i < sector_count; i++) {
        if (!SD_read_block(&sdcard, sector + i, buffer)) {
            return 0;
        }
        buffer += 512;
    }

    return 1;
}

static int FAT_media_write(uint32_t sector, uint8_t *buffer, uint32_t sector_count) {
    for(int i = 0; i < sector_count; i++) {
        if (!SD_write_block(&sdcard, sector + i, buffer)) {
            return 0;
        }
        buffer += 512;
    }

    return 1;
}


bool SD_FAT_initialize() {
    if (SD_initialize(&sdcard) != SD_INIT_OK) {
        return false;
    }

    if (fl_attach_media(FAT_media_read, FAT_media_write) != FAT_INIT_OK) {
        return false;
    } else {  
        return true;
    }
}

SDCard* SD_FAT_get_sd_card()

#endif//__ROSCOM68K_LIBC_SDCARD_H__