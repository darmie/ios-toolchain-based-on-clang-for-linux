<table border='1'>
<td>
<h2><font color='blue'>Install llvm-gcc(Optional)</font></h2>
Currently, you already had a iOS toolchain include assembler/linker, compiler, SDK and utilities, you can start development now.<br>
For some special situation, for example, you need porting some codes with GCC-extensions, you may need a gcc compiler, this is the root cause why we may need llvm-gcc.<br>
<br>
Acctually, llvm provides a GCC plugin named 'dragonegg' to replaces GCC's optimizers and code generators with those from the LLVM project. But unfortunately,  gcc does support 'arm-apple-darwin11' as target， so we can not use GCC+dragonegg to support iOS development.<br>
<br>
Download llvm-gcc-4.2<br>
<a href='http://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/llvm-gcc-for-ios-4.2.tar.xz'>http://ios-toolchain-based-on-clang-for-linux.googlecode.com/files/llvm-gcc-for-ios-4.2.tar.xz</a>

Build it, NOTE!!!, the installation path is /opt/llvm-gcc to avoid replace files already installed by other packages. you should adjust configure args according to your environment.<br>
<br>
<pre><code>$cd llvm-gcc-4.2<br>
$mkdir build<br>
$cd build<br>
$../configure  --target=arm-apple-darwin11 \<br>
    --prefix=/opt/llvm-gcc \<br>
    --with-sysroot=/usr/share/iPhoneOS6.0.sdk \<br>
    --enable-languages=c,c++,objc,obj-c++ \<br>
    --with-as=/usr/bin/arm-apple-darwin11-as \<br>
    --with-ld=/usr/bin/arm-apple-darwin11-ld \<br>
    --enable-wchar_t=no \<br>
    --with-gxx-include-dir=/usr/include/c++/4.2.1<br>
$make<br>
$make install<br>
</code></pre>

You will get arm-apple-darwin11-gcc/arm-apple-darwin11-g++ in /opt/llvm-gcc/bin folders.<br>
Remember add your installation path to PATH env var.<br>
<br>
</td>
</table>