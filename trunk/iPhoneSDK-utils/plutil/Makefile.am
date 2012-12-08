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

AM_CFLAGS = -DPLIST_BYTE_ORDER=0 

bin_PROGRAMS = plutil 

plutil_LDADD = $(XML_LIBS) 

plutil_SOURCES = base64.c \
	bplist.c \
	bytearray.c \
	hashtable.c \
	iterator.c \
	list.c \
	node.c \
	node_iterator.c \
	node_list.c \
	plist.c \
	plutil.c \
	ptrarray.c \
	xplist.c
	
INCLUDES = -I$(top_srcdir)/plutil $(XML_CFLAGS)
