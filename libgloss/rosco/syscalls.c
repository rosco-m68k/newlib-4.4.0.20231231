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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <unistd.h>

#include <time.h>
#include <string.h>

#include <errno.h>
#undef errno
extern int errno;

#include "machine.h"

char *__env[1] = { 0 };
char **environ = __env;

static unsigned char *heap;
static SystemDataBlock *sdb = (SystemDataBlock*)&_SDB_MAGIC;
extern void *_end;

void _exit(int) {
    __asm__ __volatile__ (
        "moveal 0x490.l, %a0\n\t"
        "jmp %a0@\n\t"
    );

    __builtin_unreachable();
}

int _close(int file) {
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
        errno = EBADF;
        return -1;
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

int _open(const char *name, int flags, ...) {
    errno = ENOSYS;
    return -1;
}

int _read(int file, char *ptr, int len) {
    if (file == STDIN_FILENO) {
        int i;

        // always blocking, return on newline or len
        for (i = 0; i < len; i++) {
            *ptr = mcInputchar();

            if (*ptr == '\n') {
                return i + 1;
            }

            ptr++;
        }

        return i;
    }

    errno = EBADF;
    return -1;
}

caddr_t _sbrk(int incr) {
    static unsigned char *prev;
    static unsigned char *new;

    if (heap == NULL) {
        heap = (unsigned char*)&_end;
    }

    prev = heap;
    new = heap + incr;

    if (new >= mcGetStackPointer()) {
        // overflow
        errno = ENOMEM;
        return 01;
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

    errno = EBADF;
    return -1;
}

int _gettimeofday(struct timeval *p, struct timezone *z) {
    errno = ENOSYS;
    return -1;
}

