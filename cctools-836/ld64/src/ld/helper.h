#ifndef _HELPER_H
#define _HELPER_H
#include <unistd.h>
extern "C" {
int _Local_NSGetExecutablePath(char *path, unsigned int *size);
};
#endif
