If you want to do this totally under linux, you may need these tools installed.


**dmg2img** can be downloaded from: http://vu1tur.eu.org/tools/

**xar** can be downloaded from: https://code.google.com/p/xar/

The following is for 5.0 sdk from "xcode\_4.2\_and\_ios\_5\_sdk\_for\_snow\_leopard.dmg" , which is almost same with 4.x/6.x sdk and other version of xcode.

### 1. Extract iPhoneSDK pkg from xcode-xxx.img ###
Download the xcode-xxx.dmg with iOS sdk from Apple.

Under Mac OSX, click the xcode img will mount it automaticallly, usually it is mounted under **/Volumes/Xcode/**, enter /Volumes/Xcode/Packages and find **iPhoneSDK5\_0.pkg**  for ios 5 or other versions according to your iOS version.

Under Linux, you will need dmg2img to manipulate the Xcode img:

1, Get the partition
```
$dmg2img -p xcode_4.2_and_ios_5_sdk_for_snow_leopard.dmg
```

You will get
```

dmg2img v1.6.4 (c) vu1tur (to@vu1tur.eu.org)

xcode_4.2_and_ios_5_sdk_for_snow_leopard.dmg --> (partition list)

partition 0: Driver Descriptor Map (DDM : 0)
partition 1:  (Apple_Free : 1)
partition 2: Apple (Apple_partition_map : 2)
partition 3: Macintosh (Apple_Driver_ATAPI : 3)
partition 4:  (Apple_Free : 4)
partition 5: disk image (Apple_HFS : 5)
partition 6:  (Apple_Free : 6)
```

Find the line contains "Apple HFS", it is partition 5 for this dmg, and run below command to convert it to loopback img.

```
$dmg2img -p 5 xcode_4.2_and_ios_5_sdk_for_snow_leopard.dmg
```

After it finished, you will get a img named "xcode\_4.2\_and\_ios\_5\_sdk\_for\_snow\_leopard.img".

Mount it:
```
$sudo modprobe hfsplus
$sudo mount -o loop -t hfsplus xcode_4.2_and_ios_5_sdk_for_snow_leopard.img /mnt
$cd /mnt/Packages.
```

You will find a lot of ".pkg" files in this dir, we need:

**iPhoneSDK5\_0.pkg** : this is iOS SDK.

**iPhoneSDKTools.pkg** : this pkg contains libarc\_iphoneos.a, it is needed by clang with "-fobjc-arc" flag to enable ARC support of Objective-C.

**DeveloperDiskImage5\_0.pkg** : this pkg contains debugserver for iOS, it will be used when you setup debugserver in Step 5.


### 2. Extract files  for iPhoneOS SDK ###
For SDK,
```
$cd /usr/share
$xar -xf iPhoneSDK5_0.pkg Payload; cat Payload | zcat | cpio -id
$mv Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS5.0.sdk .
$rm -rf Platforms
$rm -rf Payload
```

For ARC support
```
$xar -xf iPhoneSDKTools.pkg Payload
$cat Payload |zcat|cpio -id
$mkdir -p /usr/share/iPhoneOS5.0.sdk/usr/lib/arc
$cp Platforms/iPhoneOS.platform/Developer/usr/lib/arc/libarclite_iphoneos.a /usr/share/iPhoneOS5.0.sdk/usr/lib/arc
```
