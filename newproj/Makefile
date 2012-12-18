all:
	cd libxcodeutils && make
	clang++ -g -o pbxproj pbxproj.cpp -I./libxcodeutils ./libxcodeutils/libxcodeutils.a

clean:
	cd libxcodeutils && make clean
	rm -rf pbxproj
