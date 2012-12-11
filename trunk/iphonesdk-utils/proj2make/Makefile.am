bin_PROGRAMS = proj2make 

proj2make_CPPFLAGS =	\
	-I$(top_srcdir)/libhelper	\
	-I$(srcdir)/BASE_PC/	\
	-I$(srcdir)/BASE_PC/Dohickies

proj2make_SOURCES =	\
	main.cpp	\
	CMaker.cpp	\
	CTarget.cpp	\
	BASE_PC/cutil.cpp	\
	BASE_PC/CBufFile.cpp	\
	BASE_PC/Dohickies/CpbxLexFile.cpp	\
	BASE_PC/Dohickies/CSymbol.cpp	\
	BASE_PC/Dohickies/CLexFile.cpp	\
	BASE_PC/Dohickies/CTokFile.cpp

proj2make_LDADD = $(top_builddir)/libhelper/libhelper.la

EXTRA_DIST = 	\
	CMaker.h	\
	CPBPROJ.h	\
	CTarget.h	\
	BASE_PC/Dohickies/CcLexFile.cpp	\
	BASE_PC/Dohickies/CTokFile.cpp	\
	BASE_PC/Dohickies/CLexFile.h	\
	BASE_PC/Dohickies/CcLexFile.h	\
	BASE_PC/Dohickies/CpbxLexFile.cpp	\
	BASE_PC/Dohickies/CTokFile.h	\
	BASE_PC/Dohickies/CSymbol.cpp	\
	BASE_PC/Dohickies/CSymbol.c	\
	BASE_PC/Dohickies/CpbxLexFile.h	\
	BASE_PC/Dohickies/CLexFile.cpp	\
	BASE_PC/Dohickies/CSymbol.h	\
	BASE_PC/CBufFile.cpp	\
	BASE_PC/base.h	\
	BASE_PC/cutil.cpp	\
	BASE_PC/tgtypes.h	\
	BASE_PC/CBufFile.h	\
	BASE_PC/cutil.h
