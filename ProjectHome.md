# <font color='blue'>Intro</font> #

iOS toolchain for linux.

  1. **Compiler**:  Clang/LLVM
  1. **Assembler**: cctools-855 and ld64-236.3 ported from Apple opensource to linux.
  1. **Debugger**: GDB 1822 from Apple opensource.
  1. **iPhoneOS SDK**: iOS 4.x/5.x/6.x SDK directly from xcode.
  1. **Utilities**: iphonesdk-utils with Projects templates, localization tool, xcodeproj convertor and more.

# <font color='blue'>Features</font> #
  1. Same components as Xcode 4.5
  1. Support iOS4.x, iOS5.x, iOS6.x
  1. Support armv6, armv7, armv7s for iOS4/iOS5/iOS6.
  1. Support Objective C ARC and Blocks.
  1. Support Fat Mach-O binary.
  1. Test with A lot of codes include RestKit, gdb-1822, etc.


# <font color='blue'>Howto</font> #

Please follow the <a href='http://code.google.com/p/ios-toolchain-based-on-clang-for-linux/wiki/HowTo_en'> HOWTO  </a>to build a iOS toolchain for your linux distribution.


# <font color='blue'>Changes</font> #
2015 07-28 -for recent cctools/ld64 ports, please refer to github page of Thomas Pöchtrager: https://github.com/tpoechtrager/cctools-port. It's a fork and updated version of cctools-port.

2014 06-30 -update cctools to 855 and ld64 to 236.3, merge changes back from https://github.com/tpoechtrager/cctools-port

2013 09-24 -provide prebuilt gdb/cctools/iphonesdk-utils for ubuntu 13.04

2013 09-23 -tested with ubuntu 13.04 and clang/llvm 3.2 shipped in repos.

2013 09-23 -cctools update to 839 and iphonesdk-utils update to 2.0

2013 01-04 -fix a clang-wrapper bug in iphonesdk-utils when only one sdk exists.

2012 12-28 -update wiki, added sublimeClang configurations, update emacs-auto-complete to find Header of Frameworks properly, no need "fix sdk header position" anymore, remove it from WIKI page.

2012 12-26 -update iphonesdk-utils to 1.6, fix genLocalization build with llvm-3.2, fix mlinker-version issue with clang-3.2.

2012 12-23 -deprecated all downloads of SDK and binaries from Xcode, will remove all of them later, You can refer to HOWTO to generate all of them.

2012 12-23 -update gdb 1822 , set port version to 0.1, thanks titan.ufo for the patch of sdk root path.

2012 12-23 -iphonesdk-utils 1.5 for more project templates and xcbuild fix.

2012 12-19 -Have to release iphonesdk-utils 1.4, fix some issue(maybe dangerous issue) in xcbuild and remove proj2make. xcbuild can understand xcode project better than proj2make and produce more useful Makefile.

2012 12-19 -provide a 'ios-xcbuild' utility with iphonesdk-utils to build ios xcode project directly under linux.

2012 12-17 -provide a Makefile for restkit.

2012 12-15 -compiled a iOS native gdb with our toolchain.

2012 12-15 -gdb 1822 ported. working on lldb remote-ios debug.

2012 12-14 -provides emacs auto-complete support.

2012 12-14 -enable ld\_classic under 32bit linux.

2012 12-14 -switch project management to automake/autoconf.

2012 12-14 -clean codes, reduce the modification to original darwin codes.

2012 12-11 -call ldid when ld finished, now multi-arch FAT mach-o works.

2012 12-11 -port ld64-134.9 to linux.

2012 12-09 -merge pincrush from http://howett.net/pincrush into iPhoneSDK-utils.

2012 12-09 -rewrite ios-genLocalization, use clang lexer to extract Localizable strings from source files.

2012 12-08 -update iPhoneSDK-utils, merge proj2make, ldid, iphone-fixpng in it. a single package make life easy.

2012 12-07 -port ld64 128.2 to linux, armv7/armv7s works.

2012 12-05 -Invite yetist join this project, welcome yetist.

2012 12-05 -Update iPhoneSDK-utils, provide project template and Localization tool.

2012 11-28 -Update en/zh wiki pages.

2012 11-28 -Add proj2make, xcode to makefile converter.

2012 11-26 -Initial setup. upload ldid/cctools sourcecode and other related resources.