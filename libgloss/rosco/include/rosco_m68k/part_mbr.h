/*
 * vim: set et ts=4 sw=4
 *------------------------------------------------------------
 *                                  ___ ___ _
 *  ___ ___ ___ ___ ___       _____|  _| . | |_
 * |  _| . |_ -|  _| . |     |     | . | . | '_|
 * |_| |___|___|___|___|_____|_|_|_|___|___|_,_|
 *                     |_____|           stdlibs
 * ------------------------------------------------------------
 * Copyright (c) 2021 Ross Bamford & Contributors
 * MIT License
 *
 * MBR structure for partition access
 * ------------------------------------------------------------
 */
#ifndef __ROSCOM68K_LIBC_PART_MBR_H__
#define __ROSCOM68K_LIBC_PART_MBR_H__

#include <stdint.h>

typedef struct {
    uint8_t     status;
    uint8_t     chs_start[3];
    uint8_t     type;
    uint8_t     chs_end[3];
    uint32_t    lba_start;
    uint32_t    sector_count;
} PartEntry;

typedef struct {
    uint8_t     bootcode[446];
    PartEntry   parts[4];
    uint8_t     signature[2];
} MBR;

#endif//__ROSCOM68K_LIBC_PART_MBR_H__
