CC  := clang
CXX := clang++
LLVMCOMPONENTS := cppbackend
RTTIFLAG := -fno-rtti
LLVMCONFIG := llvm-config

CXXFLAGS := $(shell $(LLVMCONFIG) --cxxflags) $(RTTIFLAG) $(shell pkg-config --cflags libxml-2.0) -DPLIST_BYTE_ORDER=0 -I./plutil
LLVMLDFLAGS := $(shell $(LLVMCONFIG) --ldflags --libs $(LLVMCOMPONENTS)) $(shell pkg-config --libs libxml-2.0)

SOURCES = main.cpp getLocalizedStringFromFile.cpp helper.cpp
plist_SOURCES = plutil/base64.c \
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

OBJECTS = $(SOURCES:.cpp=.o)
OBJECTS += $(plist_SOURCES:.c=.o)
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
				-lclangBasic\
				$(shell llvm-config --libs)

all: genstrings 
 
.cpp.o:
	$(CXX) -c -o $@ $< $(CXXFLAGS)
.c.o:
	$(CC) -c -o $@ $< $(CXXFLAGS)

genstrings: $(OBJECTS)
	$(CXX) -o $@ $^ $(CLANGLIBS) $(LLVMLDFLAGS)

clean:
	-rm -f genstrings $(OBJECTS) *~
