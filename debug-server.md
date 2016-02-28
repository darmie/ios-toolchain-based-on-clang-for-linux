#Prepare debugserver

Apple provides a "debugserver" with XCode, for iOS 5.x, it is in "DeveloperDiskImage5_0.pkg"

Extract debugserver 
```
$xar -xf DeveloperDiskImage5_0.pkg PayLoad $cat Payload |zcat|cpio -id $cp Platforms/iPhoneOS.platform/DeviceSupport/Latest/DeveloperDiskImage.dmg . $dmg2img DeveloperDiskImage.dmg $mount -o loop DeveloperDiskImage.`img /mnt $mkdir ~/Developer $cd /mnt $cp -r * ~/Developer/
```

Now, the `$HOME/Developer` dir contains all related files of debugserver.

For security issue, by default, you can not use debugserver directly in iOS device, it need to be signed.

prepare a ent.xml 
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

make sure the debugserver is not FAT mach-o, otherwise, use lipo to extract it.

sign it. `$cd ~/Developer/usr/bin $ldid -Sent.xml debugserver`
