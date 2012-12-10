##############################
# The main binary
##############################

AM_CPPFLAGS =						\
	-DBINDIR=\"$(bindir)\"			 	\
	-DDATADIR=\"$(datadir)\"			\
	-DPREFIX=\""$(prefix)"\" 			\
	-DSYSCONFDIR=\""$(sysconfdir)"\" 		\
	-DLIBDIR=\""$(libdir)"\" 			\
	$(NULL)

bin_PROGRAMS = ldid

ldid_SOURCES = \
	       ldid.cpp	\
	       lookup2.c	\
	       sha1.c


ldid_LDADD = -L$(top_srcdir)/libhelper -lhelper 

INCLUDES = -I$(top_srcdir)/libhelper
EXTRA_DIST =	\
	sha1.h			\
	minimal/hexdump.h	\
	minimal/mapping.h	\
	minimal/sqlite3.h	\
	minimal/stdlib.h	\
	minimal/string.h
