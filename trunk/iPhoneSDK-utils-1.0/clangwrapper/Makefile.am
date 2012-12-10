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

bin_PROGRAMS = ios-clang-wrapper 

ios_clang_wrapper_SOURCES = clangwrapper.cpp


ios_clang_wrapper_LDADD = $(XML_LIBS) -L$(top_srcdir)/libhelper -lhelper

INCLUDES = $(XML_CFLAGS) -I$(top_srcdir)/libhelper

install-data-hook:
	cd $(DESTDIR)/$(bindir)
	ln -sf $(bindir)/ios-clang-wrapper $(DESTDIR)/$(bindir)/ios-clang
	ln -sf $(bindir)/ios-clang-wrapper $(DESTDIR)/$(bindir)/ios-clang++
