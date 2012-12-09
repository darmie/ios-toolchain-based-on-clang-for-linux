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

bin_PROGRAMS = ios-fixpng 

ios_fixpng_SOURCES = fixpng.c 

ios_fixpng_LDADD = @ZLIB_LIBS@ @PNG_LIBS@ 
