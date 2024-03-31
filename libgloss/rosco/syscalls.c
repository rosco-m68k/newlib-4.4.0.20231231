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
 * Syscall implementations for newlib libc
 * ------------------------------------------------------------
 */

#include <stdint.h>
#include <stdnoreturn.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <unistd.h>
#include <fcntl.h>

#include <time.h>
#include <string.h>

#include <errno.h>
#undef errno
extern int errno;

#include "machine.h"
#include "vfs.h"
#include "sdfat.h"
#include "fat_filelib.h"

char *__env[1] = { 0 };
char **environ = __env;

static unsigned char *heap;
static SystemDataBlock *sdb = (SystemDataBlock*)&_SDB_MAGIC;
static int stdin_nonblock;

static int fat_initialized;
static uint32_t files_bmp;
static void* files[32];

extern void *_end;

#define GUARD_FAT_INIT()                    \
    do {                                    \
        if (!SD_FAT_get_sd_card()) {        \
            if (!SD_FAT_initialize()) {     \
                errno = ENXIO;              \
                return -1;                  \
            }                               \
            fat_initialized = 1;            \
        }                                   \
    } while (0)


noreturn void _exit(int) {
    if (fat_initialized) {
        fl_shutdown();
    }

    __asm__ __volatile__ (
        "moveal 0x490.l, %a0\n\t"
        "jmp %a0@\n\t"
    );

    __builtin_unreachable();
}

static inline int _sd_close(int file) {
    GUARD_FAT_INIT();

    if (files_bmp & (1 << file) == 0) {
        errno = EBADF;
        return -1;
    }

    fl_fclose(files[file]);
    files_bmp &= ~(1 << file);
    return 0;
}

int _close(int file) {
    if (file >= REAL_FILE_OFS) {
        return _sd_close(file - REAL_FILE_OFS);
    }

    errno = EBADF;
    return -1;
}

int _execve(char *name, char **argv, char **env) {
    errno = ENOMEM;
    return -1;
}

int _fork() {
    errno = EAGAIN;
    return -1;
}

int _fstat(int file, struct stat *st) {
    switch (file) {
    case STDIN_FILENO:
    case STDOUT_FILENO:
    case STDERR_FILENO:
        st->st_mode = S_IFCHR;
        return 0;
    default:
        errno = EBADF;
        return -1;
    }
}

int _getpid() {
    return 1;
}

int _isatty(int file) {
    switch (file) {
    case STDIN_FILENO:
    case STDOUT_FILENO:
    case STDERR_FILENO:
        return 1;
    default:
        return 0;
    }
}

int _kill(int pid, int sig) {
    errno = EINVAL;
    return -1;
}

int _link(char *old, char *new) {
    errno = EMLINK;
    return -1;
}

static int _sd_lseek(int file, int ptr, int dir) {
    if (file < REAL_FILE_OFS) {
        errno = EBADF;
        return -1;
    }

    file -= REAL_FILE_OFS;

    if (files_bmp & (1 << file) == 0) {
        errno = EBADF;
        return -1;
    }

    void *f = files[file];

    return fl_fseek(f, ptr, dir);
}


int _lseek(int file, int ptr, int dir) {
    switch (file) {
    case STDIN_FILENO:
    case STDOUT_FILENO:
    case STDERR_FILENO:
        return 0;
    default:
        errno = EBADF;
        return -1;
    }
}

int _sd_open(const char *name, int flags) {
    GUARD_FAT_INIT();

    if (files_bmp == 0xFFFFFFFF) {
        errno = EMFILE;
        return -1;
    }

    int num = -1;

    for (int i = 0; i < 32; i++) {
        if ((files_bmp & (1 << i)) == 0) {
            num = i;
            break;
        }
    }

    if (num == -1) {
        errno = EMFILE;
        return -1;
    }

    void *file = fl_fopen(name + (strlen(SD_FN_PREFIX) - 1), flags);

    if (!file) {
        // fl_fopen should have set errno...
        return -1;
    }

    files_bmp |= (1 << num);
    files[num] = file;

    return num + REAL_FILE_OFS;
}

int _open(const char *name, int flags, ...) {
    if (strncmp(SD_FN_PREFIX, name, strlen(SD_FN_PREFIX))) {
        return _sd_open(name, flags);
    } else if (strncmp(STDIN_FN, name, strlen(STDIN_FN))) {
        // reopen stdin
        stdin_nonblock = flags & O_NONBLOCK;
        return STDIN_FILENO;
    } else if (strncmp(STDOUT_FN, name, strlen(STDOUT_FN))) {
        return STDOUT_FILENO;
    } else if (strncmp(STDERR_FN, name, strlen(STDERR_FN))) {
        return STDERR_FILENO;
    }

    errno = ENOENT;
    return -1;
}

static char backspace[4] = { 0x08, 0x20, 0x08, 0x00 };
static char sendbuf[2] = { 0x00, 0x00 };

static int _stdin_read(char *buf, int len) {
    int i;

    // line buffered unless O_NONBLOCK, return on newline or len
    for (i = 0; i < len; i++) {
        if (stdin_nonblock && !mcCheckInput()) {
            return i;
        }

        char c = mcInputchar();

        switch (c) {
        case 0x08:
        case 0x7F:  /* DEL */
            if (i > 0) {
                buf[i-1] = 0;
                i = i - 1;
                mcPrint(backspace);
            }
            break;
        case 0x0A:
            // throw this away...
            i -= 1;
            break;
        case 0x0D:
            // return
            buf[i] = '\n';
            mcPrintln("");
            return i + 1;
        default:
            buf[i] = c;
            sendbuf[0] = c;
            mcPrint(sendbuf);
        }
    }

    return i;
}

static int _sd_read(int file, char *buf, int len) {
    if (file < REAL_FILE_OFS) {
        errno = EBADF;
        return -1;
    }

    file -= REAL_FILE_OFS;

    if (files_bmp & (1 << file) == 0) {
        errno = EBADF;
        return -1;
    }

    void *f = files[file];

    return fl_fread(buf, 1, len, f);
}

int _read(int file, char *ptr, int len) {
    if (file == STDIN_FILENO) {
        return _stdin_read(ptr, len);
    }

    return _sd_read(file, ptr, len);
}

caddr_t _sbrk(int incr) {
    static unsigned char *prev;
    static unsigned char *new;

    if (heap == NULL) {
        heap = (unsigned char*)&_end;
    }

    prev = heap;
    new = heap + incr;

    if (((uint32_t)new) >= mcGetStackPointer()) {
        // overflow
        errno = ENOMEM;
        return (caddr_t)-1;
    }

    heap = new;

    return prev;
}

int _stat(const char *file, struct stat *st) {
    // TODO this should _probably_ be consistent with lstat!
    errno = ENOENT;
    return -1;
}

clock_t _times(struct tms *buf) {
    buf->tms_utime = sdb->upticks;
    buf->tms_stime = 0;
    buf->tms_cstime = 0;
    buf->tms_cutime = 0;

    return buf->tms_utime;
}

int _unlink(char *name) {
    errno = ENOENT;
    return -1;
}

int _wait(int *status) {
    errno = ECHILD;
    return -1;
}

static int _sd_write(int file, char *buf, int len) {
    if (file < REAL_FILE_OFS) {
        errno = EBADF;
        return -1;
    }

    file -= REAL_FILE_OFS;

    if (files_bmp & (1 << file) == 0) {
        errno = EBADF;
        return -1;
    }

    void *f = files[file];

    return fl_fwrite(buf, 1, len, f);
}

int _write(int file, char *ptr, int len) {
    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
        for (int i = 0; i < len; i++) {

            // TODO we should _probably_ be doing this at the firmware level,
            // so as not to force it on all output devices....
            if (*ptr == '\n') {
                mcPrintchar('\r');
            }

            mcPrintchar(*ptr++);
        }

        return len;
    }

    return _sd_write(file, ptr, len);
}

int _gettimeofday(struct timeval *p, struct timezone *z) {
    errno = ENOSYS;
    return -1;
}

