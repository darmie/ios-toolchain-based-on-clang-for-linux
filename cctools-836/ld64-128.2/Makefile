###################################################
#
# Makefile for rebase
# Creator [Xcode -> Makefile Ver: Dec  2 2012 18:32:45]
# Created: [Thu Dec  6 22:49:12 2012]
#
###################################################

#
# Macros
#

PROJECTNAME:=ld

CC = clang++ -stdlib=libc++
CFLAGS = -fblocks -Wno-format -fno-stack-protector -Wno-trigraphs -Wno-deprecated -Wno-strict-aliasing -D__LP64__=1
LDFLAGS = -lpthread -ldl -lssl -lcrypto -luuid 
CFLAGS +=-I../include
CFLAGS +=-I../include/foreign
CFLAGS += -DHAVE_CONFIG_H -D__LITTLE_ENDIAN__=1
LDFLAGS        +=  -fno-builtin-round -fno-builtin-trunc

CFLAGS += -I.\
		-Isrc/ld\
		-Isrc/other -Isrc/abstraction/ -Isrc/ld/parsers

ld : \
		./ld.o\
		./tlvp.o\
		./order.o\
		./branch_island.o\
		./OutputFile.o\
		./objc.o\
		./branch_shim.o\
		./stubs.o\
		./macho_relocatable_file.o\
		./archive_file.o\
		./macho_dylib_file.o\
		./opaque_section_file.o\
		./dtrace_dof.o\
		./InputFiles.o\
		./SymbolTable.o\
		./Resolver.o\
		./got.o\
		./dylibs.o\
		./huge.o\
		./compact_unwind.o\
		./Options.o\
		./debugline.o\
		./strlcat.o\
		./qsort.o\
		./heapsort_r.o\
		./strlcpy.o
	$(CC) $(LDFLAGS) \
		./ld.o\
		./tlvp.o\
		./order.o\
		./branch_island.o\
		./OutputFile.o\
		./objc.o\
		./branch_shim.o\
		./stubs.o\
		./macho_relocatable_file.o\
		./archive_file.o\
		./macho_dylib_file.o\
		./opaque_section_file.o\
		./dtrace_dof.o\
		./InputFiles.o\
		./SymbolTable.o\
		./Resolver.o\
		./got.o\
		./dylibs.o\
		./huge.o\
		./compact_unwind.o\
		./Options.o\
		./debugline.o\
		./strlcat.o\
		./qsort.o\
		./heapsort_r.o\
		./strlcpy.o\
		-o ld 

clean : 
		rm -rf *.o
		rm -rf ld

# Item # 2 -- ld --
./ld.o : src/ld/ld.cpp
	$(CC) $(CFLAGS) src/ld/ld.cpp -c -o ./ld.o


# Item # 3 -- tlvp --
./tlvp.o : src/ld/passes/tlvp.cpp
	$(CC) $(CFLAGS) src/ld/passes/tlvp.cpp -c -o ./tlvp.o


# Item # 5 -- order --
./order.o : src/ld/passes/order.cpp
	$(CC) $(CFLAGS) src/ld/passes/order.cpp -c -o ./order.o


# Item # 6 -- branch_island --
./branch_island.o : src/ld/passes/branch_island.cpp
	$(CC) $(CFLAGS) src/ld/passes/branch_island.cpp -c -o ./branch_island.o


# Item # 7 -- OutputFile --
./OutputFile.o : src/ld/OutputFile.cpp
	$(CC) $(CFLAGS) src/ld/OutputFile.cpp -c -o ./OutputFile.o



# Item # 9 -- objc --
./objc.o : src/ld/passes/objc.cpp
	$(CC) $(CFLAGS) src/ld/passes/objc.cpp -c -o ./objc.o


# Item # 10 -- branch_shim --
./branch_shim.o : src/ld/passes/branch_shim.cpp
	$(CC) $(CFLAGS) src/ld/passes/branch_shim.cpp -c -o ./branch_shim.o


# Item # 11 -- stubs --
./stubs.o : src/ld/passes/stubs/stubs.cpp
	$(CC) $(CFLAGS) src/ld/passes/stubs/stubs.cpp -c -o ./stubs.o


# Item # 12 -- macho_relocatable_file --
./macho_relocatable_file.o : src/ld/parsers/macho_relocatable_file.cpp
	$(CC) $(CFLAGS) src/ld/parsers/macho_relocatable_file.cpp -c -o ./macho_relocatable_file.o


# Item # 13 -- archive_file --
./archive_file.o : src/ld/parsers/archive_file.cpp
	$(CC) $(CFLAGS) src/ld/parsers/archive_file.cpp -c -o ./archive_file.o


# Item # 15 -- macho_dylib_file --
./macho_dylib_file.o : src/ld/parsers/macho_dylib_file.cpp
	$(CC) $(CFLAGS) src/ld/parsers/macho_dylib_file.cpp -c -o ./macho_dylib_file.o


# Item # 16 -- opaque_section_file --
./opaque_section_file.o : src/ld/parsers/opaque_section_file.cpp
	$(CC) $(CFLAGS) src/ld/parsers/opaque_section_file.cpp -c -o ./opaque_section_file.o


# Item # 17 -- dtrace_dof --
./dtrace_dof.o : src/ld/passes/dtrace_dof.cpp
	$(CC) $(CFLAGS) src/ld/passes/dtrace_dof.cpp -c -o ./dtrace_dof.o


# Item # 18 -- InputFiles --
./InputFiles.o : src/ld/InputFiles.cpp
	$(CC) $(CFLAGS) src/ld/InputFiles.cpp -c -o ./InputFiles.o


# Item # 19 -- SymbolTable --
./SymbolTable.o : src/ld/SymbolTable.cpp
	$(CC) $(CFLAGS) src/ld/SymbolTable.cpp -c -o ./SymbolTable.o


# Item # 20 -- Resolver --
./Resolver.o : src/ld/Resolver.cpp
	$(CC) $(CFLAGS) src/ld/Resolver.cpp -c -o ./Resolver.o


# Item # 21 -- got --
./got.o : src/ld/passes/got.cpp
	$(CC) $(CFLAGS) src/ld/passes/got.cpp -c -o ./got.o


# Item # 22 -- dylibs --
./dylibs.o : src/ld/passes/dylibs.cpp
	$(CC) $(CFLAGS) src/ld/passes/dylibs.cpp -c -o ./dylibs.o



# Item # 25 -- huge --
./huge.o : src/ld/passes/huge.cpp
	$(CC) $(CFLAGS) src/ld/passes/huge.cpp -c -o ./huge.o


# Item # 26 -- compact_unwind --
./compact_unwind.o : src/ld/passes/compact_unwind.cpp
	$(CC) $(CFLAGS) src/ld/passes/compact_unwind.cpp -c -o ./compact_unwind.o


# Item # 27 -- Options --
./Options.o : src/ld/Options.cpp
	$(CC) $(CFLAGS) src/ld/Options.cpp -c -o ./Options.o


# Item # 30 -- debugline --
./debugline.o : src/ld/debugline.c
	clang $(CFLAGS) src/ld/debugline.c -c -o ./debugline.o
./strlcat.o : src/ld/strlcat.c
	clang $(CFLAGS) src/ld/strlcat.c -c -o ./strlcat.o
./strlcpy.o : src/ld/strlcpy.c
	clang $(CFLAGS) src/ld/strlcpy.c -c -o ./strlcpy.o
./qsort.o : src/ld/qsort.c
	clang $(CFLAGS) src/ld/qsort.c -c -o ./qsort.o
./heapsort_r.o : src/ld/heapsort_r.c
	clang $(CFLAGS) src/ld/heapsort_r.c -c -o ./heapsort_r.o


##### END RUN ####
