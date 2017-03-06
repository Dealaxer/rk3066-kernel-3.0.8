# rk3066-kernel-3.0.8(+)
Kernel for Cube U30GT-H (Android 4.1)

<strong>CMDLINE for Android</strong>:<br>
CONFIG_CMDLINE="console=ttyFIQ0 androidboot.console=ttyFIQ0 init=/init"<br>
CONFIG_CMDLINE_FROM_BOOTLOADER=y

<strong>CMDLINE for Linux</strong>:<br>
CONFIG_CMDLINE="root=LABEL=linuxroot init=/sbin/init loglevel=8 rootfstype=ext4 rootwait mtdparts=rk29xxnand:0x00002000@0x00000000(parameter),0x00002000@0x00002000(misc),0x00004000@0x00004000(kernel),0x00008000@0x00008000(boot),0x00008000@0x00010000(recovery)"<br>
CONFIG_CMDLINE_FORCE=y<br>
<br>or<br><br>
CONFIG_CMDLINE="root=LABEL=linuxroot init=/sbin/init loglevel=8 rootfstype=ext4 rootwait<br>
CONFIG_CMDLINE_FORCE=y<br>
<br>
<strong>How to compile a kernel for Cube U30GT-H</strong>:<br>
(To build the kernel 3.0.8 Android 4.1, use toolchain with GCC 4.4.3)<br>
Command:<br>
<i>export ARCH=arm</i><br>
<i>export CROSS_COMPILE=(<strong>Your Path</strong>)/toolchain/arm-eabi-4.4.3/bin/arm-eabi-</i><br><br>

<strong>For Cube U30GT-H should be used RK30 SDK config</strong>:<br>
Command:<br>
<i>make rk30_sdk_defconfig</i>

<strong>Then You can configure kernel with command</strong>:<br>
(Initial configuration includes settings as in the original kernel from the manufacturer Cube)<br>
Command:<br>
<i>make menuconfig</i><br>

<strong>Building the kernel</strong>:<br>
Command:<br>
<i>make -j4</i>

(-j "4" number of threads, that is usually the number of cores of your processor)
