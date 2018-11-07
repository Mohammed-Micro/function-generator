#ifndef _dbg_h
#define _dbg_h

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef NCOLOR
#define YELLOW
#define MAGENTA
#define RED
#define CYAN
#define RESET
#else
#define YELLOW "\x1B[33m"
#define MAGENTA "\x1B[35m"
#define RED "\x1B[31m"
#define CYAN "\x1B[36m"
#define RESET "\x1B[0m"
#endif

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr,YELLOW "[DEBUG] (%s:%s:%d) " M RESET "\n",__FILE__,__func__,__LINE__,##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) fprintf(stderr,RED "[ERROR] (%s:%s:%d: errno: %s) " M RESET "\n",__FILE__,__func__,__LINE__,clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr,MAGENTA "[WARNING] (%s:%s:%d: errno: %s) " M RESET "\n",__FILE__,__func__,__LINE__,clean_errno(),##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr,CYAN "[INFO] (%s:%s:%d) " M RESET "\n",__FILE__,__func__,__LINE__,##__VA_ARGS__)

#define check(A, M, ...) if(!(A)){ log_err(M,##__VA_ARGS__); errno = 0; goto error; }

#define sentinel(M, ...) { log_err(M,##__VA_ARGS__); errno = 0; goto error; }

#define check_mem(P) check(P, "Out of memory.")

#define check_debug(A,M, ...) if(!(A)) {debug(M,##__VA_ARGS__); errno = 0; goto error; }

#endif
