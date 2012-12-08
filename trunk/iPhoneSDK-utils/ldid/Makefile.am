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
	       finder.cpp	\
	       lookup2.c	\
	       sha1.c


ldid_LDADD = \
	$(NULL)

EXTRA_DIST =	\
	sha1.h			\
	finder.h			\
	xalloc.h			\
	minimal/hexdump.h	\
	minimal/mapping.h	\
	minimal/sqlite3.h	\
	minimal/stdlib.h	\
	minimal/string.h
