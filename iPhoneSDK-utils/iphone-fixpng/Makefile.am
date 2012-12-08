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

bin_PROGRAMS = fixpng 

fixpng_SOURCES = fixpng.c 

fixpng_LDADD = @ZLIB_LIBS@ @PNG_LIBS@ 
