AM_CPPFLAGS =                       \
    -DBINDIR=\"$(bindir)\"              \
    -DDATADIR=\"$(datadir)\"            \
    -DPREFIX=\""$(prefix)"\"            \
    -DSYSCONFDIR=\""$(sysconfdir)"\"        \
    -DLIBDIR=\""$(libdir)"\"            \
    $(NULL)


AM_CFLAGS = @LLVM_CXX_FLAGS@ -fno-rtti 
AM_CXXFLAGS = @LLVM_CXX_FLAGS@ -fno-rtti 

INCLUDES = -I$(top_srcdir)/libplutil -I$(top_srcdir)/libhelper $(XML_CFLAGS)

bin_PROGRAMS = ios-genLocalization

CLANGLIBS = \
                -lclangTooling\
                -lclangFrontendTool\
                -lclangFrontend\
                -lclangDriver\
                -lclangSerialization\
                -lclangCodeGen\
                -lclangParse\
                -lclangSema\
                -lclangStaticAnalyzerFrontend\
                -lclangStaticAnalyzerCheckers\
                -lclangStaticAnalyzerCore\
                -lclangAnalysis\
                -lclangARCMigrate\
                -lclangRewriteFrontend\
                -lclangRewriteCore\
                -lclangEdit\
                -lclangAST\
                -lclangLex\
                -lclangBasic

CLANGLIBS += @LLVM_LIBS@

ios_genLocalization_LDADD = @LLVM_COMPONENTS@ $(CLANGLIBS) $(XML_LIBS) -L$(top_srcdir)/libplutil -lplutil  -L$(top_srcdir)/libhelper -lhelper

ios_genLocalization_SOURCES = main.cpp getLocalizedStringFromFile.cpp
