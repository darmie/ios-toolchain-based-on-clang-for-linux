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


bin_PROGRAMS = proj2make 

proj2make_SOURCES = main.cpp CMaker.cpp helper.cpp CTarget.cpp \
	BASE_PC/cutil.cpp BASE_PC/CBufFile.cpp BASE_PC/Dohickies/CpbxLexFile.cpp \
	BASE_PC/Dohickies/CSymbol.cpp BASE_PC/Dohickies/CLexFile.cpp BASE_PC/Dohickies/CTokFile.cpp

INCLUDES = -I$(top_srcdir)/proj2make/BASE_PC/ -I$(top_srcdir)/proj2make/BASE_PC/Dohickies/
