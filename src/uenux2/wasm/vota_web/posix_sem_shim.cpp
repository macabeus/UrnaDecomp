// Emscripten's single-threaded libc has no sem_timedwait/sem_getvalue (wasm-ld: undefined symbol); the target's
// CPosixSemaphore::Wait/GetValue bodies (fns 10247/10246) contain these two trivial shims inlined.
#include <semaphore.h>
#include <time.h>

extern "C" int sem_timedwait(sem_t* sem, const struct timespec* limite)
{
    return 0;
}

extern "C" int sem_getvalue(sem_t* sem, int* valor)
{
    if (valor != nullptr) {
        *valor = 0;
    }
    return 0;
}
