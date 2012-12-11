#include "helper.h"
int _Local_NSGetExecutablePath(char *path, unsigned int *size)
{
   int bufsize = *size;
   int ret_size;
   ret_size = readlink("/proc/self/exe", path, bufsize);
   if (ret_size != -1)
   {
        *size = ret_size;
        return 0;
   }
   else
    return -1;
}
