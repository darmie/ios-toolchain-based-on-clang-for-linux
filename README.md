# ios-toolchain-based-on-clang-for-linux
Automatically exported from code.google.com/p/ios-toolchain-based-on-clang-for-linux

The purpose of this repo is just to hold the instructions for using this tool. However an updated version of this tool can be found at https://github.com/Tidal-Loop/linux-ios-toolchain


##Intro

1. **Compiler**: Clang/LLVM
2. **Assembler**: cctools-855 and ld64-236.3 ported from Apple opensource to linux.
3. **Debugger**: GDB 1822 from Apple opensource.
4. **iPhoneOS SDK**: iOS 4.x/5.x/6.x SDK directly from xcode.

**Utilities:** iphonesdk-utils with Projects templates, localization tool, xcodeproj convertor and more.



##Features
1. Same components as Xcode 4.5
2. Support iOS4.x, iOS5.x, iOS6.x
3. Support armv6, armv7, armv7s for iOS4/iOS5/iOS6.
4. Support Objective C ARC and Blocks.
5. Support Fat Mach-O binary.
6. Test with A lot of codes include RestKit, gdb-1822, etc.


## Installation Guide

NOTE: storyboard and xib files can not be compiled by this toolchain, since the format of these files is closed and undocumented, only nibtool shipped with xcode can handle these files and we had no chance to port it.

Clang for linux provides an almost full implementation for Objective C 2.0 includes blocks and other features gcc doesn't support.

New version of xcode also use clang as its compiler:

http://opensource.apple.com/release/developer-tools-45/

This URL shows the opensource components used in Xcode 4.5, Almost everything missing in linux had been ported and we got a Non-UI copy of Xcode toolchain for linux now.
This is the URL for opensource components used in Xcode 7 => http://opensource.apple.com/release/developer-tools-70/
###Here is an tutorial of 'How to setup iOS toolchain for linux'.

All these components shoule be easily port to Windows with cygwin except re-write some platform related functions. waiting for volunteers.

Actually, the iOS toolchain under linux is as same and simple as other crosscompile toolchain for embeded devices. It should include these components:

1. **Assembler and Linker**: cctools and ld64 from apple opensource.
2. **Compiler**: Clang/LLVM
3. **SDK**, include headers and libraries.
4. **Utilities**: such as ldid codesign tool.
5. **Debugger**: gdb/dsymutil.
6. **Documents**: API reference and related documents.
You also need an iOS device jailbreaked and with ssh installed for debug purposes.

By the way, the default ssh password is **alpine** .

Before we start, you need a workable C/C++ compiler installed.
####Step 1: The compiler

Clang/llvm >= 3.2 is highly recommended and tested.

If you want to build clang/llvm from scratch, Please refer to **Clang-LLVM-build.md** to build a svn version for your linux distribution.

If your distribution already provides clang/llvm packages，make sure it is 3.2 release or above. Lower version may work but isn't tested.

for Ubuntu 13.04 users, clang/llvm already provided in repos, please run:

`$sudo apt-get install gcc g++ clang libclang-dev uuid-dev libssl-dev libpng12-dev libicu-dev bison flex libsqlite3-dev`

to install some dev packages, other dev packages related to llvm/llvm-dev should be installed automatically.

####Step 2: The assembler and linker

The cctools-855 and ld64-236.3 had been ported from Apple opensource to linux. the porting process is a little bit complicated, also with a lot of codes modified for linux, let's just skip it.

please check out the codes from: **/cctools-porting**

You can get the latest version of cctools here: https://github.com/tpoechtrager/cctools-port

Build it: `$./cctools-ld64.sh $cd cctools-855-ld64-236.3 $./configure --target=arm-apple-darwin11 --prefix=/usr $make $make install`

For Ubuntu 13.04, since the clang/llvm 3.2 package use a customized libraries/headers path. please setup CFLAGS and CXXFLAGS first before run configure. 
```
$export CFLAGS="-I/usr/include/llvm-c-3.2" $export CXXFLAGS="-I/usr/include/llvm-c-3.2"
```


####Step 3: iPhone OS SDK

If you want to do this totally under linux, you may need these tools installed.

dmg2img can be downloaded from: http://vu1tur.eu.org/tools/

xar can be downloaded from: https://code.google.com/p/xar/

The old iPhone SDK with ARC support extracted from xcode had been provided in Download Sections. You can directly download it and extract it to /usr/share

For iOS 4.2: https://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/iPhoneOS4.2.sdk.tar.xz

For iOS 5.0: https://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/iPhoneOS5.0.sdk.tar.xz

For iOS 6.0: https://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/iPhoneOS6.0.sdk.tar.xz

The following is for 5.0 sdk from "xcode_4.2_and_ios_5_sdk_for_snow_leopard.dmg" , which is almost same with 4.x/6.x sdk and other version of xcode.

#####1. Extract iPhoneSDK pkg from xcode-xxx.img

Download the xcode-xxx.dmg with iOS sdk from Apple.

Under Mac OSX, click the xcode img will mount it automaticallly, usually it is mounted under **/Volumes/Xcode/**, enter **/Volumes/Xcode/Packages** and find **iPhoneSDK5_0.pkg** for ios 5 or other versions according to your iOS version.

Under Linux, you will need dmg2img to manipulate the Xcode img:

1, Get the partition `$dmg2img -p xcode_4.2_and_ios_5_sdk_for_snow_leopard.dmg`

You will get

```
dmg2img v1.6.4 (c) vu1tur (to@vu1tur.eu.org)

xcode_4.2_and_ios_5_sdk_for_snow_leopard.dmg --> (partition list)

partition 0: Driver Descriptor Map (DDM : 0) partition 1: (Apple_Free : 1) partition 2: Apple (Apple_partition_map : 2) partition 3: Macintosh (Apple_Driver_ATAPI : 3) partition 4: (Apple_Free : 4) partition 5: disk image (Apple_HFS : 5) partition 6: (Apple_Free : 6) ```
```
Find the line contains "Apple HFS", it is partition 5 for this dmg, and run below command to convert it to loopback img.

`$dmg2img -p 5 xcode_4.2_and_ios_5_sdk_for_snow_leopard.dmg`

After it finished, you will get a img named "xcode_4.2_and_ios_5_sdk_for_snow_leopard.img".

Mount it: `$sudo modprobe hfsplus $sudo mount -o loop -t hfsplus xcode_4.2_and_ios_5_sdk_for_snow_leopard.img /mnt $cd /mnt/Packages.`

You will find a lot of ".pkg" files in this dir, we need:

**iPhoneSDK5_0.pkg** : this is iOS SDK.

**iPhoneSDKTools.pkg** : this pkg contains libarc_iphoneos.a, it is needed by clang with "-fobjc-arc" flag to enable ARC support of Objective-C.

**DeveloperDiskImage5_0.pkg** : this pkg contains debugserver for iOS, it will be used when you setup debugserver in Step 5.

#####2. Extract files for iPhoneOS SDK

For SDK, 
```
$cd /usr/share $xar -xf iPhoneSDK5_0.pkg Payload; cat Payload | zcat | cpio -id $mv Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS5.0.sdk . $rm -rf Platforms $rm -rf Payload
```

For ARC support 
```
$xar -xf iPhoneSDKTools.pkg Payload $cat Payload |zcat|cpio -id $mkdir -p /usr/share/iPhoneOS5.0.sdk/usr/lib/arc $cp Platforms/iPhoneOS.platform/Developer/usr/lib/arc/libarclite_iphoneos.a /usr/share/iPhoneOS5.0.sdk/usr/lib/arc
```

####Step 4: The utilities

iphonesdk-utils is a utility collection for iOS development, provides below utilities:

NOTE: (Some of them are collected from internet with some modifications.)

1. **ldid** : codesign tool, with armv7/armv7s support and other changes from orig version. it will be involked by ld64 after link complete.
2. **ios-clang-wrapper** : automatically find SDK and construct proper compilation args.
3. **ios-switchsdk** : switch sdk when multiple version of SDK exist.
4. **ios-pngcrush**: png crush/de-crush tool, like Apple's pngcrush.
5. **ios-createProject** : project templates
6. **ios-genLocalization** : iOS app localization tool based on clang lexer.
7. **ios-plutil** : plist compiler/decompiler.
8. **ios-xcbuild** : convert xcode project to makefile, build xcode project directly under linux.
Download the source tarball from: https://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/iphonesdk-utils-2.0.tar.gz

Build and install it: `$./configure --prefix=/usr $make $make install`

####Step 5: Debugger

Since we don't have an iOS emulator under linux, we have to use physical device to debug our iOS apps. so we need a debugserver installed into iOS device and a native gdb.

If you are so sure about the quality of your codes, this step can be just ignored:-D

NOTE: This may only work on a jail broken iPhone
#####1. Install debugserver

There are old version debugserver (extracted from xcode)provided in Download Section:

For iOS 5.x: https://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/debugserver-for-ios-5.x.tar.xz

For iOS 6.x: https://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/debugserver-for-ios-6.x.tar.xz

You can directly download it according to your device and extract it.

For other versions, please follow these steps in **/debug-server.md** to extract debugserver.

After extraction, upload "Developer" dir to the root of iOS device. `$scp -r ~/Developer root@<your iphone IP>:/`

And ssh to your iOS device and try it. 
`$ssh <Your iPhone IP> -l root $/Developer/usr/bin/debugserver 1000 /Applications/<Some>.app/<Some binary>`

#####2. Install gdb 1822

I had ported gdb-1822 from opendarwin to linux.

A package include opencflite/maloader/gdb-1822/dsymutil(copied from macosx) had been provided to make it build and install easily.

Download from:

http://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/gdb-1822-for-linux-0.2.tar.gz

Extract and build/install it: `$make $make install` It will install arm-apple-darwin11-gdb/dsymutil to `/usr/bin` and some libraries to `/usr/lib`

####Step 6: API reference. (Optional)

Download API reference from:

http://devimages.apple.com/docsets/20120109/com.apple.adc.documentation.AppleiOS5_0.iOSLibrary.xar

Extract it: 
```
xar -xf com.apple.adc.documentation.AppleiOS5_0.iOSLibrary.xar cd com.*iOSLibrary.docset/Contents/Resources/Documents/ firefox ./index.html
```
Test toolchain

I tested this toolchain with many opensource iOS projects, you can use RestKit, json-framework, fmdb or sample codes directly from API reference.

(NOTE, A lot of sampel code in API Reference from Apple use xib and storyboard, these files can not be converted to binary format, so it may be built but can not run directly in iOS device.)

Here provides an example of RestKit:

Download RestKit from: https://github.com/RestKit/RestKit

Prepare a iOS 6.x sdk, since the latest RestKit need some API only in iOS 6.x.

You may need to switch to 6.0 SDK $ios-switchsdk

then enter RestKit dir `$ios-xcbuild RestKit.xcodeproj $make`

or directly build it `$ios-xcbuild -b RestKit.xcodeproj`

You will get the final result (headers and librestkit.a) in "./xcbuild" directory.

Build an iOS App.

#####1. Create project
Run `ios-createProject $ios-createProject`

You will get below messages:
```
iPhone Project Createor

[0.] Application [1.] Command line utility [2.] Dynamic Framework [3.] Dynamic Library [4.] Notification Center widget [5.] Static Framework [6.] Static Library [7.] Example Player [8.] Example UICatalog Choose a Template: ```
```
Input 0 and the Project name "HelloWorld" to create a helloworld App.

#####2. Build & install App
`$cd HelloWorld $make $make install IPHONE_IP=<your own device IP>`

If you want to build a multi-arch FAT mach-o binary, add multiple `-arch armv(n)` to `CFLAGS/CXXFLAGS` in Makefile. armv(n) means armv6/armv7/armv7s.

For iOS 4.x/5.x, armv6 armv7 can be used. For iOS 6.x, armv7/armv7s can be used. (armv6 droped in iOS 6.x)

#####3. Debug App
Remove `-g0` from the `CFLAGS` in Makefile if it exists.

Add `-g` to the CFLAGS in Makefile and recompile your project.

and run: `$cd <binary>.app $dsymutil -o <binary>.dSYM <binary> it will generate a "<binary>.dSYM" directory.`

Upload the bundle to your iOS device and also cp to the same path of SDK dir.

for example, if your project is named "Hello.app", you should upload it to /Applications of your iOS device and also cp it to /Applications.

in iOS device, run 

```
/Developer/usr/bin/debugserver 1000 /Applications/Hello.app/Hello

```

It will launch debugserver, open port 1000 and waiting for a connection.

in Linux, run `$arm-apple-darwin11-gdb`

when `(gdb)` prompt appear, enter `(gdb)file /usr/share/iPhoneOS5.0.sdk/Applications/Hello.app/Hello (gdb)target remote-macosx <Your iPhone IP>:1000`

#####4. Localize App
Run ios-genLocalization in Project folder. `$ios-genLocalization`

``` 
Localization tool for iOs App

Languages [ 0.] English (English) [ 1.] zh_CN (Simp. Chinese) [ 2.] zh_TW (Trad. Chinese) [ 3.] ko (Korean) [ 4.] Japanese (Japanese) [ 5.] German (German) [ 6.] French (French) [ 7.] Italian (Italian) [ 8.] Spanish (Spanish) Enter your number : 
```

Please choose which language your App want to support, it will generate the related plist files. translate them and re-install your App to iOS device.
