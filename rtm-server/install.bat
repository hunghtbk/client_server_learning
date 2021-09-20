adb shell mount -o rw,remount /

adb push \\10.158.14.149\tuan1.nguyen\a35\build\tmp\work\aarch64-poky-linux\rtmbackend-bin\1.0-r0\build\RTMBackend /usr/bin
adb shell chmod 777 /usr/bin/RTMBackend
adb push certificate/certificate.pem /var/lib/iRTM
adb push certificate/key.pem /var/lib/iRTM
adb shell c_rehash /var/lib/iRTM
