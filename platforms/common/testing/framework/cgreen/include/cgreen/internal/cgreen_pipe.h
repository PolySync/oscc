#ifndef CGREEN_PIPE_HEADER
#define CGREEN_PIPE_HEADER

#include <unistd.h>

#ifdef __cplusplus
namespace cgreen {
    extern "C" {
#endif

int cgreen_pipe_open(int pipes[2]);
void cgreen_pipe_close(int p);
ssize_t cgreen_pipe_read(int p, void *buf, size_t count);
ssize_t cgreen_pipe_write(int p, const void *buf, size_t count);

#ifdef __cplusplus
    }
}
#endif

#endif
