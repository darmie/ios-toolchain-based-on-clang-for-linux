#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(iPhoneSDK, 0.4, http://www.i-soft.com.cn)

AM_INIT_AUTOMAKE([1.10 no-define no-dist-gzip dist-bzip2 tar-ustar])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stddef.h stdint.h stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_FORK
AC_CHECK_FUNCS([memset strdup strrchr strtoul])
AC_CHECK_LIB([z],[crc32],[
         ZLIB_LIBS=-lz
         ])
AC_SUBST(ZLIB_LIBS)

AC_CHECK_LIB([png],[png_create_read_struct],[
         PNG_LIBS=-lpng
         ])
AC_SUBST(PNG_LIBS)


AC_OUTPUT([
	   Makefile
	   ldid/Makefile
	   clang-wrapper/Makefile
	   proj2make/Makefile
	   createProject/Makefile
	   genLocalization/Makefile
	   iphone-fixpng/Makefile
])
