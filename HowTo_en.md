# iOS toolchain based on clang for linux #

<font color='blue'>Cjacker <cjacker@gmail.com> 2014-06-30 updated</font>

<font color='red'>NOTE: storyboard and xib files can not be compiled by this toolchain, since the format of these files is closed and undocumented, only nibtool shipped with xcode can handle these files and we had no chance to port it.</font>

Clang for linux provides an almost full implementation for Objective C 2.0 includes blocks and other features gcc doesn't support.

New version of xcode also use clang as its compiler:

http://opensource.apple.com/release/developer-tools-45/

This URL shows the opensource components used in Xcode 4.5, Almost everything missing in linux had been ported and we got a Non-UI copy of Xcode toolchain for linux now.

Here is an tutorial of  'How to setup iOS toolchain for linux'.

All these components shoule be easily port to Windows with cygwin except re-write some platform related functions. waiting for volunteers.

Actually, the iOS toolchain under linux is as same and simple as other crosscompile toolchain for embeded devices. It should include these components:

  1. **Assembler and Linker**: cctools and ld64 from apple opensource.
  1. **Compiler**:  Clang/LLVM
  1. **SDK**, include headers and libraries.
  1. **Utilities**: such as ldid codesign tool.
  1. **Debugger**: gdb/dsymutil.
  1. **Documents**: API reference and related documents.

You also need an iOS device jailbreaked and with ssh installed.

By the way, the default ssh password is <font color='red'> <b>alpine</b> </font>.

Before we start, you need a workable C/C++ compiler installed.



## <font color='blue'> Step 1 : The compiler </font> ##

<font color='red'>Clang/llvm >= 3.2 </font>is **highly** recommended and tested.

If you want to build clang/llvm from scratch, Please refer to <a href='http://code.google.com/p/ios-toolchain-based-on-clang-for-linux/wiki/build_clang_en'> this link</a> to build a svn version for your linux distribution.

If your distribution already provides  clang/llvm packages，make sure it is 3.2 release or above.  Lower version may work but isn't tested.

<font color='blue'>for Ubuntu 13.04 users, clang/llvm already provided in repos, please run:</font>

```
$sudo apt-get install gcc g++ clang libclang-dev uuid-dev libssl-dev libpng12-dev libicu-dev bison flex libsqlite3-dev
```

to install some dev packages, other dev packages related to llvm/llvm-dev should be installed automatically.


## <font color='blue'> Step 2 : The assembler and linker </font> ##
The latest cctools-855 and ld64-236.3 had been ported from Apple opensource to linux. the porting process is a little bit complicated, also with a lot of codes modified for linux, let's just skip it.

please check out the codes from:
```
svn checkout http://ios-toolchain-based-on-clang-for-linux.googlecode.com/svn/trunk/cctools-porting
```


Build it:
```
$./cctools-ld64.sh
$cd cctools-855-ld64-236.3
$./configure --target=arm-apple-darwin11 --prefix=/usr
$make
$make install
```

<font color='blue'>For Ubuntu 13.04</font>, since the clang/llvm 3.2 package use a customized libraries/headers path. please setup CFLAGS and CXXFLAGS first before run configure.
```
$export CFLAGS="-I/usr/include/llvm-c-3.2"
$export CXXFLAGS="-I/usr/include/llvm-c-3.2"
```


## <font color='blue'> Step 3: The iPhoneOS SDK. </font> ##
The old iPhone SDK with ARC support extracted from xcode had been provided in Download Sections.
You can directly download it and extract it to /usr/share

For iOS 4.2: https://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/iPhoneOS4.2.sdk.tar.xz

For iOS 5.0: https://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/iPhoneOS5.0.sdk.tar.xz

For iOS 6.0: https://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/iPhoneOS6.0.sdk.tar.xz

For other iOS versions, You may need follow <a href='https://code.google.com/p/ios-toolchain-based-on-clang-for-linux/wiki/iphoneos_sdk_en'> these steps </a>to get the SDK for your self.


## <font color='blue'> Step 4: The utilities </font> ##
iphonesdk-utils is a utility collection for iOS development, provides below utilities:

**NOTE:**
(Some of them are collected from internet with some modifications.)

  1. **ldid** : codesign tool, with armv7/armv7s support and other changes from orig version. it will be involked by ld64 after link complete.
  1. **ios-clang-wrapper** : automatically find SDK and construct proper compilation args.
  1. **ios-switchsdk** : switch sdk when multiple version of SDK exist.
  1. **ios-pngcrush**: png crush/de-crush tool, like Apple's pngcrush.
  1. **ios-createProject** : project templates
  1. **ios-genLocalization** : iOS app localization tool based on clang lexer.
  1. **ios-plutil** : plist compiler/decompiler.
  1. **ios-xcbuild** : convert xcode project to makefile, build xcode project directly under linux.

Download the source tarball from:
https://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/iphonesdk-utils-2.0.tar.gz

Build and install it:
```
$./configure --prefix=/usr
$make
$make install
```

## <font color='blue'> Step 5: Debugger </font> ##
Since we don't have an iOS emulator under linux, we have to use physical device to debug our iOS apps.  so we need a debugserver installed into iOS device and a native gdb.

<font color='red'>If you are so sure about the quality of your codes, this step can be just ignored:-D</font>

### 1. Install debugserver ###
There are old version debugserver (extracted from xcode)provided in Download Section:

For iOS 5.x: https://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/debugserver-for-ios-5.x.tar.xz

For iOS 6.x: https://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/debugserver-for-ios-6.x.tar.xz

You can directly download it according to your device and extract it.

For other versions, please follow <a href='https://code.google.com/p/ios-toolchain-based-on-clang-for-linux/wiki/debugserver_en'>these steps</a> to extract debugserver.

After extraction, upload "Developer" dir to the root of iOS device.
```
$scp -r ~/Developer root@<your iphone IP>:/
```

And ssh to your iOS device and try it.
```
$ssh <Your iPhone IP> -l root
$/Developer/usr/bin/debugserver 1000 /Applications/<Some>.app/<Some binary>
```

### 2. Install gdb 1822 ###
I had ported gdb-1822 from opendarwin to linux.

A package include opencflite/maloader/gdb-1822/dsymutil(copied from macosx) had been provided to make it build and install easily.


Download from:

http://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/gdb-1822-for-linux-0.2.tar.gz

Extract and build/install it:
```
$make
$make install
```
It will install arm-apple-darwin11-gdb/dsymutil to "/usr/bin" and some libraries to "/usr/lib"


## <font color='blue'> Step 6: API reference. (Optional) </font> ##
Download API reference from:

http://devimages.apple.com/docsets/20120109/com.apple.adc.documentation.AppleiOS5_0.iOSLibrary.xar

Extract it:
```
xar -xf com.apple.adc.documentation.AppleiOS5_0.iOSLibrary.xar
cd com.*iOSLibrary.docset/Contents/Resources/Documents/
firefox ./index.html
```




## <font color='blue'> Test toolchain </font> ##
I tested this toolchain with many opensource iOS projects, you can use RestKit, json-framework, fmdb or sample codes directly from API reference.

(NOTE, A lot of sampel code in API Reference from Apple use xib and storyboard, these files can not be converted to binary format, so it may be built but can not run directly in iOS device.)

Here provides an example of RestKit:

Download RestKit from:
https://github.com/RestKit/RestKit

Prepare a iOS 6.x sdk, since the latest RestKit need some API only in iOS 6.x.

You may need to switch to 6.0 SDK
```
$ios-switchsdk
```

then enter RestKit dir
```
$ios-xcbuild RestKit.xcodeproj
$make
```

or directly build it
```
$ios-xcbuild -b RestKit.xcodeproj
```

You will get the final result (headers and librestkit.a) in "./xcbuild" directory.


## <font color='blue'> Build an iOS App. </font> ##
#### 1. Create project ####
Run ios-createProject
```
$ios-createProject
```

You will get below messages:
```
iPhone Project Createor
---------------------------
[0.] Application
[1.] Command line utility
[2.] Dynamic Framework
[3.] Dynamic Library
[4.] Notification Center widget
[5.] Static Framework
[6.] Static Library
[7.] Example Player
[8.] Example UICatalog
Choose a Template:
```

Input 0 and the Project name "HelloWorld" to create a helloworld App.


#### 2. Build & install App ####
```
$cd HelloWorld
$make
$make install IPHONE_IP=<your own device IP>
```

If you want to build a multi-arch FAT mach-o binary, add multiple "-arch armv(n)" to CFLAGS/CXXFLAGS in Makefile. armv(n) means armv6/armv7/armv7s.

For iOS 4.x/5.x, armv6 armv7 can be used.
For iOS 6.x, armv7/armv7s can be used. (armv6 droped in iOS 6.x)

#### 3. Debug App ####
Remove -g0 from the CFLAGS in Makefile if it exists.

Add "-g" to the CFLAGS in Makefile and recompile your project.

and run:
```
$cd <binary>.app
$dsymutil -o <binary>.dSYM <binary>
```
it will generate a "`<`binary`>`.dSYM" directory.

Upload the bundle to your iOS device and also cp to the same path of SDK dir.

for example, if your project is named "Hello.app", you should upload it to /Applications of your iOS device and also cp it to <SDK root path>/Applications.

in iOS device, run
```
#/Developer/usr/bin/debugserver 1000 /Applications/Hello.app/Hello
```

It will launch debugserver, open port 1000 and waiting for a connection.

in Linux, run
```
$arm-apple-darwin11-gdb
```

when (gdb) prompt appear, enter
```
(gdb)file /usr/share/iPhoneOS5.0.sdk/Applications/Hello.app/Hello
(gdb)target remote-macosx <Your iPhone IP>:1000
```


#### 4. Localize App ####
Run ios-genLocalization in Project folder.
```
$ios-genLocalization
```

```
Localization tool for iOs App

Languages
[ 0.] English   (English)
[ 1.] zh_CN     (Simp. Chinese)
[ 2.] zh_TW     (Trad. Chinese)
[ 3.] ko        (Korean)
[ 4.] Japanese  (Japanese)
[ 5.] German    (German)
[ 6.] French    (French)
[ 7.] Italian   (Italian)
[ 8.] Spanish   (Spanish)
Enter your number : 
```

Please choose which language your App want to support, it will generate the related plist files. translate them and re-install your App to iOS device.




---


---

## <font color='blue'> Emacs codes auto complete. (Optional) </font> ##
I modified emacs-auto-complete-clang-async to run clang-complete only one instance and have a significant performance improvement.(The patch had been submitted to original author)

here is a tarball include auto-complete/yasnippet/autopair.el/auto-complete-clang-async to provide a convenient development environment with Emacs.

Download :

http://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/emacs-auto-complete.tar.gz

Please refer to "README" in tarball to setup by yourself.

## <font color='blue'> VIM codes auto complete. (Optional) </font> ##
If you are a vim user, you can use clang\_complete plugin for vim to get code completion support. make sure your vim/gvim already enable python support.

Download it from:
https://github.com/Rip-Rip/clang_complete

extract all files to your ~/.vim dir.

then create a App Project. and add these lines to `<`Project folder`>`/.clang\_complete
```
-ccc-host-triple arm-apple-darwin11
-I/usr/share/iPhoneOS5.0.sdk/usr/include
-F/usr/share/iPhoneOS5.0.sdk/System/Library/Frameworks
```

when you want to get the code completion hint, you can type "Ctrl-x Ctrl-u" to get a popup menu.

You may also need autopair/snipMate plugin for VIM.

## <font color='blue'> Sublime Text 2 codes auto complete. (Optional) </font> ##
Sublime text 2 is another good choice if you do not use vim/Emacs.
For iOS code complete in Sublime text 2, you need install SublimeClang from  Package Control or by your self and make sure it is works.

Then configure it's options like:
```
        "-isysroot", "/usr/share/iPhoneOS5.0.sdk",
        "-F/usr/share/iPhoneOS5.0.sdk/System/Library/Frameworks",
        "-I/usr/share/iPhoneOS5.0.sdk/usr/include",
        "-target", "arm-apple-darwin11",
        "-fblocks",
        "-fobjc-nonfragile-abi",
        "-fno-builtin",
        "-fobjc-arc",
        "-D__IPHONE_OS_VERSION_MIN_REQUIRED=50100"
```

## Enjoy it. ##
