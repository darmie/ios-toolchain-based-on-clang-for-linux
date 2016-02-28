Here is a simple step by step tutorial for how to enable App debug for iOS toolchain:

### 1. Prepare debugserver ###
Apple provides a "debugserver" with XCode, for iOS 5.x, it is in "DeveloperDiskImage5\_0.pkg"

1. Extract debugserver
```
$xar -xf DeveloperDiskImage5_0.pkg PayLoad
$cat Payload |zcat|cpio -id
$cp Platforms/iPhoneOS.platform/DeviceSupport/Latest/DeveloperDiskImage.dmg .
$dmg2img DeveloperDiskImage.dmg
$mount -o loop DeveloperDiskImage.img /mnt
$mkdir ~/Developer
$cd /mnt
$cp -r * ~/Developer/
```

Now, the $HOME/Developer dir contains all related files of debugserver.

For security issue, by default, you can not use debugserver directly in iOS device, it need to be signed.

1. prepare a ent.xml
```
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>com.apple.springboard.debugapplications</key>
    <true/>
    <key>get-task-allow</key>
    <true/>
    <key>task_for_pid-allow</key>
    <true/>
    <key>run-unsigned-code</key>
    <true/>
</dict>
</plist>
```

2. make sure the debugserver is not FAT mach-o, otherwise, use lipo  to extract it.

3. sign it.
```
$cd ~/Developer/usr/bin
$ldid -Sent.xml debugserver
```

4, upload "Developer" dir to the root of iOS device.
```
$scp -r ~/Developer root@<your iphone IP>:/
```

5, ssh to your iOS device and try it.
```
$ssh <Your iPhone IP> -l root
$/Developer/usr/bin/debugserver 1000 /Applications/<Some>.app/<Some binary>
```

### 2. Install gdb 1822 ###
Install gdb 1822 and dsymutil according to the <a href='http://code.google.com/p/ios-toolchain-based-on-clang-for-linux/wiki/HowTo_en'>HOWTO</a>.

## 3. Enable debug informations when build ##
Remove -g0 from the CFLAGS in Makefile if it exists.

Add "-g" to the CFLAGS in Makefile and recompile your project.

and run:
```
dsymutil -o <binary>.dSYM <binary>
```
it will generate a "`<`binary`>`.dSYM" directory.

## 4.Deploy ##
Upload the bundle to your iOS device and also cp to the same path of SDK dir.

for example, if your project is named "Hello.app", you should upload it to /Applications of your iOS device and also cp it to <SDK root path>/Applications.

## 5.Debug ##
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
(gdb)set shlib-path-substitutions / /usr/share/iPhoneOS5.0.sdk/
(gdb)file /usr/share/iPhoneOS5.0.sdk/Applications/Hello.app/Hello
(gdb)target remote-macosx <Your iPhone IP>:1000
```

If you are boring to enter this text everytime when you debug a App, you can add it to "~/.gdbinit", it will be loaded  automatically when "arm-apple-darwin11-gdb" launched.

## Enjoy debug! ##