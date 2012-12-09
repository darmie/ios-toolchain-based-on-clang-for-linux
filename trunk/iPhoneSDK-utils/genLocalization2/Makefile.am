AM_CPPFLAGS =                       \
    -DBINDIR=\"$(bindir)\"              \
    -DDATADIR=\"$(datadir)\"            \
    -DPREFIX=\""$(prefix)"\"            \
    -DSYSCONFDIR=\""$(sysconfdir)"\"        \
    -DLIBDIR=\""$(libdir)"\"            \
    $(NULL)


AM_CFLAGS = -DPLIST_BYTE_ORDER=0 @LLVM_CXX_FLAGS@ -fno-rtti 
AM_CXXFLAGS = -DPLIST_BYTE_ORDER=0 @LLVM_CXX_FLAGS@ -fno-rtti 

INCLUDES = -I./plutil $(XML_CFLAGS)

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

ios_genLocalization_LDADD = @LLVM_COMPONENTS@ $(CLANGLIBS) $(XML_LIBS) 

ios_genLocalization_SOURCES = main.cpp getLocalizedStringFromFile.cpp helper.cpp plutil/base64.c \
    plutil/bplist.c \
    plutil/bytearray.c \
    plutil/hashtable.c \
    plutil/iterator.c \
    plutil/list.c \
    plutil/node.c \
    plutil/node_iterator.c \
    plutil/node_list.c \
    plutil/plist.c \
    plutil/ptrarray.c \
    plutil/xplist.c
