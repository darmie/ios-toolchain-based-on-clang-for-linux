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

bin_PROGRAMS = iPhoneSDK 

iPhoneSDK_SOURCES = iPhoneSDK.cpp


iPhoneSDK_LDADD = \
	$(NULL)

install-data-hook:
	cd $(DESTDIR)/$(bindir)
	ln -sf $(bindir)/iPhoneSDK $(DESTDIR)/$(bindir)/ios-clang
	ln -sf $(bindir)/iPhoneSDK $(DESTDIR)/$(bindir)/ios-clang++
EXTRA_DIST =	\
	xalloc.h
