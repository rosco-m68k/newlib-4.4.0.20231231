#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

char **environ; /* pointer to array of char * strings that define the current environment variables */

void _exit(int) {
}

int _close(int file) {
    return -1;
}

int _execve(char *name, char **argv, char **env) {
    return -1;
}

int _fork() {
    return -1;
}

int _fstat(int file, struct stat *st) {
    return -1;
}

int _getpid() {
    return -1;
}

int _isatty(int file) {
    return -1;
}

int _kill(int pid, int sig) {
    return -1;
}

int _link(char *old, char *new) {
    return -1;
}

int _lseek(int file, int ptr, int dir) {
    return -1;
}

int _open(const char *name, int flags, ...) {
    return -1;
}

int _read(int file, char *ptr, int len) {
    return -1;
}

caddr_t _sbrk(int incr) {
    return -1;
}

int _stat(const char *file, struct stat *st) {
    return -1;
}

clock_t _times(struct tms *buf) {
    return -1;
}

int _unlink(char *name) {
    return -1;
}

int _wait(int *status) {
    return -1;
}

int _write(int file, char *ptr, int len) {
    return -1;
}

int _gettimeofday(struct timeval *p, struct timezone *z) {
    return -1;
}

