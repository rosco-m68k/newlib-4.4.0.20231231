/*
 * vim: set et ts=4 sw=4
 *------------------------------------------------------------
 *                                  ___ ___ _
 *  ___ ___ ___ ___ ___       _____|  _| . | |_
 * |  _| . |_ -|  _| . |     |     | . | . | '_|
 * |_| |___|___|___|___|_____|_|_|_|___|___|_,_|
 *                     |_____|       firmware v2
 * ------------------------------------------------------------
 * Copyright (c) 2021 Ross Bamford & Contributors
 * MIT License
 *
 * Partition / Filesystem access routines (deals with MBR)
 * ------------------------------------------------------------
 */

#include <stdint.h>
#include <stdbool.h>
#include "ata.h"
#include "sdfat.h"
#include "part.h"
#include "part_mbr.h"
#include <errno.h>
#undef errno
extern int errno;

static MBR buffer;

PartInitStatus Part_init_SD(PartHandle *handle, SDCard *device) {
    if (SD_read_block(device, 0, (uint8_t*)&buffer)) {
        if (buffer.signature[0] == 0x55 && buffer.signature[1] == 0xAA) {
            handle->device_type = PART_DEVICE_TYPE_SD;
            handle->SD_device = device;

            for (int i = 0; i < 4; i++) {
                handle->parts[i].status = buffer.parts[i].status;
                handle->parts[i].type = buffer.parts[i].type;
                handle->parts[i].lba_start = __builtin_bswap32(buffer.parts[i].lba_start);
                handle->parts[i].sector_count = __builtin_bswap32(buffer.parts[i].sector_count);
            }

            return PART_INIT_OK;
        } else {
            return PART_INIT_BAD_SIGNATURE;
        }
    } else {
        return PART_INIT_READ_FAILURE;
    }
}

PartInitStatus Part_init_ATA(PartHandle *handle, ATADevice *device) {
    if (ATA_read_sectors((uint8_t*)&buffer, 0, 1, device) == 1) {
        if (buffer.signature[0] == 0x55 && buffer.signature[1] == 0xAA) {
            handle->device_type = PART_DEVICE_TYPE_ATA;
            handle->ata_device = device;

            for (int i = 0; i < 4; i++) {
                handle->parts[i].status = buffer.parts[i].status;
                handle->parts[i].type = buffer.parts[i].type;
                handle->parts[i].lba_start = __builtin_bswap32(buffer.parts[i].lba_start);
                handle->parts[i].sector_count = __builtin_bswap32(buffer.parts[i].sector_count);
            }

            return PART_INIT_OK;
        } else {
            return PART_INIT_BAD_SIGNATURE;
        }
    } else {
        return PART_INIT_READ_FAILURE;
    }
}

static uint32_t Part_read_SD(PartHandle *handle, uint8_t part_num, uint8_t *buffer, uint32_t start, uint32_t count) {
    if (part_num > 3) {
        errno = EINVAL;
        return 0;
    } else if (handle->parts[part_num].type == 0) {
        errno = ENXIO;
        return 0;
    } else {
        RuntimePart *part = &handle->parts[part_num];
        if (start >= part->sector_count) {
            // Out of range for partition
            return 0;
        } else {
            if (count > part->sector_count - start) {
                count = part->sector_count - start;
            }

            for (uint32_t i = 0; i < count; i++) {
                if (!SD_read_block(handle->SD_device, part->lba_start + start + i, buffer)) {
                    return i;
                }
                buffer += 512;
            }
            return count;
        }
    }
}

static uint32_t Part_read_ATA(PartHandle *handle, uint8_t part_num, uint8_t *buffer, uint32_t start, uint32_t count) {
    if (part_num > 3 || handle->parts[part_num].type == 0) {
        return 0;
    } else {
        RuntimePart *part = &handle->parts[part_num];
        if (start >= part->sector_count) {
            // Out of range for partition
            return 0;
        } else {
            if (count > part->sector_count - start) {
                count = part->sector_count - start;
            }

            return ATA_read_sectors(buffer, part->lba_start + start, count, handle->ata_device);
        }
    }
}

static uint32_t Part_write_SD(PartHandle *handle, uint8_t part_num, uint8_t *buffer, uint32_t start, uint32_t count) {
    if (part_num > 3) {
        errno = EINVAL;
        return 0;
    } else if (handle->parts[part_num].type == 0) {
        errno = ENXIO;
        return 0;
    } else {
        RuntimePart *part = &handle->parts[part_num];
        if (start >= part->sector_count) {
            // Out of range for partition
            return 0;
        } else {
            if (count > part->sector_count - start) {
                count = part->sector_count - start;
            }

            for (uint32_t i = 0; i < count; i++) {
                if (!SD_write_block(handle->SD_device, part->lba_start + start + i, buffer)) {
                    return i;
                }
                buffer += 512;
            }
            return count;
        }
    }
}

static uint32_t Part_write_ATA(PartHandle *handle, uint8_t part_num, uint8_t *buffer, uint32_t start, uint32_t count) {
    if (part_num > 3 || handle->parts[part_num].type == 0) {
        return 0;
    } else {
        RuntimePart *part = &handle->parts[part_num];
        if (start >= part->sector_count) {
            // Out of range for partition
            return 0;
        } else {
            if (count > part->sector_count - start) {
                count = part->sector_count - start;
            }

            return ATA_write_sectors(buffer, part->lba_start + start, count, handle->ata_device);
        }
    }
}

uint32_t Part_read(PartHandle *handle, uint8_t part_num, uint8_t *buffer, uint32_t start, uint32_t count) {
    switch (handle->device_type) {
    case PART_DEVICE_TYPE_ATA:
        return Part_read_ATA(handle, part_num, buffer, start, count);
    case PART_DEVICE_TYPE_SD:
        return Part_read_SD(handle, part_num, buffer, start, count);
    default:
        return 0;
    }
}

uint32_t Part_write(PartHandle *handle, uint8_t part_num, uint8_t *buffer, uint32_t start, uint32_t count) {
    switch (handle->device_type) {
    case PART_DEVICE_TYPE_ATA:
        return Part_write_ATA(handle, part_num, buffer, start, count);
    case PART_DEVICE_TYPE_SD:
        return Part_write_SD(handle, part_num, buffer, start, count);
    default:
        return 0;
    }
}

bool Part_valid(PartHandle *handle, uint8_t part) {
    // status seems unreliable in modern times (macOS MBRs set it 0), so just see if we have a type
    // TODO this can likely be tightened up quite a bit!
    return handle->parts[part].type > 0;
}
