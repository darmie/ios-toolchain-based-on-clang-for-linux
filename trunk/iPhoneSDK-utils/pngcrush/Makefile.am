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

AM_CFLAGS = -Wno-format -std=c99 -DHAVE_CONFIG_H -I. -DPNG_CONFIGURE_LIBPNG -DPNG_NO_PROGRESSIVE_READ -DPNG_NO_EASY_ACCESS 

bin_PROGRAMS = ios-pngcrush

ios_pngcrush_LDADD = -lm 

ios_pngcrush_SOURCES = adler32.c \
compress.c \
crc32.c \
deflate.c \
gzclose.c \
gzlib.c \
gzread.c \
gzwrite.c \
infback.c \
inffast.c \
inflate.c \
inftrees.c \
pincrush.c \
png.c \
pngerror.c \
pngget.c \
pngmem.c \
pngpread.c \
pngread.c \
pngrio.c \
pngrtran.c \
pngrutil.c \
pngset.c \
pngtrans.c \
pngwio.c \
pngwrite.c \
pngwtran.c \
pngwutil.c \
trees.c \
uncompr.c \
zutil.c
