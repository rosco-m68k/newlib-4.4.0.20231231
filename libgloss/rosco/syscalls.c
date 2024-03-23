#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include <errno.h>
#undef errno
extern int errno;

#include "machine.h"

char *__env[1] = { 0 };
char **environ = __env;

static unsigned char *heap;
extern void *_end;

void _exit(int) {
    __asm__ __volatile__ (
        "moveal 0x490.l, %a0\n\t"
        "jmp %a0@\n\t"
    );

    __builtin_unreachable();
}

int _close(int file) {
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
    st->st_mode = S_IFCHR;
    return 0;
}

int _getpid() {
    return 1;
}

int _isatty(int file) {
    return 1;
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
    return 0;
}

int _open(const char *name, int flags, ...) {
    return -1;
}

int _read(int file, char *ptr, int len) {
    return 0;
}

caddr_t _sbrk(int incr) {
    static unsigned char *prev;

    if (heap == NULL) {
        heap = (unsigned char*)&_end;
    }
    prev = heap;
    heap += incr;   // let it crash... 💣...💣...💣

    return prev;
}

int _stat(const char *file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

clock_t _times(struct tms *buf) {
    return -1;
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
    if (file != 1) {
        return -1;
    }

    for (int i = 0; i < len; i++) {
        mcPrintchar(*ptr++);
    }

    return len;
}

int _gettimeofday(struct timeval *p, struct timezone *z) {
    return -1;
}

