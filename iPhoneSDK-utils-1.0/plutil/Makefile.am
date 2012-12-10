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

bin_PROGRAMS = ios-plutil 

ios_plutil_LDADD = $(XML_LIBS) -L$(top_srcdir)/libplutil -lplutil

ios_plutil_SOURCES = plutil.c 
	
INCLUDES = -I$(top_srcdir)/libplutil $(XML_CFLAGS)
