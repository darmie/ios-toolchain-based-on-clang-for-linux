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


ios_clang_wrapper_LDADD = $(XML_LIBS)

INCLUDES = $(XML_CFLAGS)

install-data-hook:
	cd $(DESTDIR)/$(bindir)
	ln -sf $(bindir)/ios-clang-wrapper $(DESTDIR)/$(bindir)/ios-clang
	ln -sf $(bindir)/ios-clang-wrapper $(DESTDIR)/$(bindir)/ios-clang++
