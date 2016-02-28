Here is a User guide about how to catch the llvm/clang source codes via svn:

http://llvm.org/docs/GettingStarted.html#checkout

###1, Get the source.
```
$cd where-you-want-llvm-to-live $svn co http://llvm.org/svn/llvm-project/llvm/trunk llvm-svn

$cd where-you-want-llvm-to-live $cd llvm-svn/tools $svn co http://llvm.org/svn/llvm-project/cfe/trunk clang

$cd where-you-want-llvm-to-live $cd llvm-svn/projects $svn co http://llvm.org/svn/llvm-project/compiler-rt/trunk compiler-rt 
```

###2. Configure/build and install it.

`$./configure --prefix=/usr --enable-optimized $make $make install`

###3, Build lldb(Optional)

After you have a workable clang compiler, you can also install lldb for linux.

####3.1. Install libc++

Get the codes: `$svn co http://llvm.org/svn/llvm-project/libcxx/trunk libcxx`

#####Build and Install it:

`libcxx` can be use `libcxxabi` or `libsupc++` from `libstdc++` as `c++abi` library. the svn code was forced to `libsupc++`. I use libcxxabi+libcxx as a toolchain support libraries, It need patch the project files. Here I give a example how to build `libcxx` with `libsupc++`.

NOTE the `LIBCXX_LIBSUPCXX_INCLUDE_PATHS` is some paths seperated by semicolon, it point to the `libstdc++` header files. change it according to your environment.

```
$cd libcxx $mkdir build && cd build $cmake -DCMAKE_INSTALL_PREFIX=/usr \ -DCMAKE_C_COMPILER=clang \ -DCMAKE_CXX_COMPILER=clang++ \ -DLIBCXX_CXX_ABI=libsupc++ \ -DLIBCXX_LIBSUPCXX_INCLUDE_PATHS="/usr/include/c++/4.7.2/x86_64-pure64-linux;/usr/include/c++/4.7.2" ..
```

`$make && make install`

####3.2.Re-Build clang/llvm/lldb

Checkout SVN codes. 
``` 
$cd where-you-want-llvm-to-live $svn co http://llvm.org/svn/llvm-project/llvm/trunk llvm-svn

$cd where-you-want-llvm-to-live $cd llvm-svn/tools $svn co http://llvm.org/svn/llvm-project/cfe/trunk clang $svn co http://llvm.org/svn/llvm-project/lldb/trunk lldb

$cd where-you-want-llvm-to-live $cd llvm-svn/projects $svn co http://llvm.org/svn/llvm-project/compiler-rt/trunk compiler-rt 
```

Patch it, lldb had some bugs under linux and we also want to enable remote-ios support. 

```
$cd where-you-want-llvm-to-live $svn checkout http://ios-toolchain-based-on-clang-for-linux.googlecode.com/svn/trunk/lldb-ios $for i in lldb-ios/patches/*.patch; do cat $i|patch -p1; done $cp -r lldb-ios/externalheaders/* include
```

Build it, we need remove external headers we added before installation to avoid install them to system. 

```
$export LDFLAGS+="-lsupc++" $./configure --enable-optimized --enable-libcpp --enable-cxx11 --prefix=/usr $make $rm -rf include/{architecture,Availability.h,i386,libkern,mach,mach_debug,machine,mach-o,sys} $make install
```
After installation, you will get a command `lldb` in `/usr/bin`.

####3.3. Debug

Launch lldb, you will get a `(lldb)` promt, try: `(lldb)platform list`

You will get: 
```
(lldb) platform list 
Available platforms: PlatformLinux: Local Linux user platform plug-in. remote-freebsd: Remote FreeBSD user platform plug-in. remote-linux: Remote Linux user platform plug-in. remote-ios: Remote iOS platform plug-in. remote-macosx: Remote Mac OS X user platform plug-in. ios-simulator: iOS simulator platform plug-in. remote-gdb-server: A platform that uses the GDB remote protocol as the communication transport. (lldb)
```
