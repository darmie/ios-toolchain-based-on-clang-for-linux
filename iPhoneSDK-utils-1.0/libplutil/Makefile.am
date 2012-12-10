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

noinst_LIBRARIES=libplutil.a

AM_CFLAGS = -DPLIST_BYTE_ORDER=0 

libplutil_a_SOURCES=base64.c \
	bplist.c \
	bytearray.c \
	hashtable.c \
	iterator.c \
	list.c \
	node.c \
	node_iterator.c \
	node_list.c \
	plist.c \
	ptrarray.c \
	xplist.c
	
INCLUDES = $(XML_CFLAGS)
