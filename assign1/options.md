#qemu-system-i386 options
-gdb tcp::6808
    -gdb takes in a network protocol and a port of the form "tcp::6808" for example and sets up a gdbserver and waits for a remote connection. This connection can be started from within gdb by running the command `target remote PORT_NUMBER`

-S
    This option starts the VM in a paused state forcing a continue input from the user. In our case we do run the `continue` command from within our remote gdb connection to do so.
-nographic

    -nographic disables the SDL based graphical backend and reverts the utility to a simple terminal based application. The emulated serial output 

-kernel linux-yocto-3.14/arch/x86/boot/bzImage  
    The -kernel flag lets us specifcy which Linux kernel we would like to boot instead of making us bake the kernel into the diskimage. This effectively lets us make quick changes to the kernel for rapid debugging purposes. For this option we supply it with the yocto build we have compiled.

-drive file=core-image-lsb-sdk-qemux86.ext3,if=virtio
    The drive file gives us the ability to specify which disk image we'd like the vm to run with. The if=virtio sub option instructs the drive to behave as a full virtualization drive that is optimized to work with the underlying virtual machine host. This prevents the drive from performing as if its depending on a physical disk.

-enable-kvm
    This enables the Kernel based virtual machine support withn qemu allowing for kernel specific options to be used. The KVM model provides are more native experience/performance for the guest VM's compared to other models of virtualization.
-net none
    The net option is intended to create a network interface card for the guest vm to use. In our case that is not needed thus we provide it with the none value to prevent a NIC from being created.
-usb
    The usb flag enables the USB driver to be used by the guest vm.
-localtime
    This flag syncs the guest vm's clock to the localtime of the host machine.
-- no-reboot
    The no reboot flag prevents the vm from rebooting and forces it to simply exit. This lets us shut the down vm with reboot from within the guest if needed.

-- append "root=/dev/vda rw console=ttyS0 debug"
    The append command accepts a string that gets appended to the kernel as boot options. In our case `root=/dev/vda` moves the root partition to the Virtio virtual disk that we setup earlier as read-writable. Then the `console=ttyS0` option sets the console to use the ttyS0 serial port that gets emulated. Finally the `debug` command enables kernel debugging at the event logs level.

