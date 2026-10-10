/* Probe driver for the SQLite block: links sqlite3.c the way the app does and keeps the whole
 * public API alive through -sEXPORTED_FUNCTIONS=@exports.txt (every sqlite3_* the amalgamation
 * defines with the pinned -D set).
 * keep() gives the libc functions sqlite3.c calls a second caller with non-constant arguments and
 * a used return value: in the real app those functions have many callers, and without this
 * binaryen's dead-argument elimination would specialise them for sqlite's single call site
 * (dropping a constant argument or the return value inside the sqlite function body too). */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <dlfcn.h>
#include <math.h>
#include "sqlite3.h"
volatile int v = 3;
volatile long long acc;
static void keep(void) {
  char buf[256]; struct stat st; struct timeval tv; struct timespec ts = {v, v}; struct tm tm; time_t t = v;
  acc += strerror_r(v, buf, v);
  acc += gettimeofday(&tv, 0) + tv.tv_sec;
  acc += nanosleep(&ts, &ts);
  acc += (long long)localtime_r(&t, &tm) + (long long)gmtime_r(&t, &tm) + time(&t);
  acc += open(buf, v, v) + close(v) + read(v, buf, v) + write(v, buf, v) + (long long)lseek(v, v, v);
  acc += fstat(v, &st) + stat(buf, &st) + lstat(buf, &st) + ftruncate(v, v) + fsync(v) + fdatasync(v);
  acc += fcntl(v, v, v) + fchmod(v, v) + fchown(v, v, v) + unlink(buf) + rmdir(buf) + mkdir(buf, v) + access(buf, v);
  acc += (long long)readlink(buf, buf, v) + (long long)getcwd(buf, v) + getpid() + geteuid() + sysconf(v) + posix_fallocate(v, v, v);
  acc += (long long)mmap(0, v, v, v, v, v) + munmap(buf, v) + utimes(buf, &tv);
  acc += (long long)dlopen(buf, v) + (long long)dlsym(buf, buf) + dlclose(buf) + (long long)dlerror();
  acc += (long long)malloc(v) + (long long)realloc(buf, v); free((void *)(long)acc);
  acc += (long long)memchr(buf, v, v) + memcmp(buf, buf + 1, v) + (long long)strchr(buf, v) + (long long)strrchr(buf, v);
  acc += strcmp(buf, buf + v) + strncmp(buf, buf + 1, v) + strcspn(buf, buf + v) + strspn(buf, buf + v) + strlen(buf + v);
  acc += (long long)getenv(buf) + (long long)(sin((double)v) + cos(v) + tan(v) + asin(v) + acos(v) + atan(v) + atan2(v, v) + sinh(v) + cosh(v) + tanh(v) + asinh(v) + acosh(v) + atanh(v) + exp(v) + log(v) + log2(v) + log10(v) + pow(v, v) + fmod(v, v) + trunc(v) + sqrt(v));
  pthread_mutexattr_t a; pthread_mutex_t m; pthread_t th;
  acc += pthread_mutexattr_init(&a) + pthread_mutexattr_settype(&a, v) + pthread_mutexattr_destroy(&a) + pthread_mutex_init(&m, &a);
  acc += pthread_mutex_lock(&m) + pthread_mutex_trylock(&m) + pthread_mutex_unlock(&m) + pthread_mutex_destroy(&m);
  acc += pthread_create(&th, 0, (void *(*)(void *))keep, buf) + pthread_join(th, 0) + errno;
}
int main(void) {
  sqlite3 *db = 0;
  if (v > 100) keep();
  if (sqlite3_open(":memory:", &db) != SQLITE_OK) return 1;
  sqlite3_exec(db, "CREATE TABLE t(x); INSERT INTO t VALUES(1);", 0, 0, 0);
  printf("%s %d\n", sqlite3_libversion(), sqlite3_changes(db));
  sqlite3_close(db);
  return 0;
}
