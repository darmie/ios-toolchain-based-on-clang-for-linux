#ifndef STRLCAT_H
#define STRLCAT_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
//#include "config.h"

#ifndef HAVE_STRLCAT
size_t strlcat(char *dst, const char *src, size_t siz);
#endif

#ifdef __cplusplus
}
#endif

#endif

