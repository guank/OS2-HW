#qemu-system-i386 options
-gdb tcp::6808
-S
-nographic
-kernel linux-yocto- 3.14/arch/x86/boot/bzImage  
-drive file=core-image-lsb-sdk-qemux86.ext3,if=virtio
-enable- kvm
-net none
-usb
-localtime
-- no-reboot
-- append "root=/dev/vda rw console=ttyS0 debug"


