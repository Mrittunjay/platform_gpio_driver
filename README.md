# platform_gpio_driver

Creating a custom Platform led device and driver binding:
Now steps of creating the device node in the device tree for BBB

FDT for BBB is known as am335x-boneblack.dtb
am335x-boneblack.dtb is compiled from three source files
1.am335x-boneblack.dts
2.am335x-bone-common.dtsi
3.am33xx.dtsi
source format for am33xx.dtsi
line 11-15:
/include/ "skeleton.dtsi"

/ {
	compatible = "ti,am33xx";
	interrupt-parent = <&intc>;



line 68-77:
am33xx_pinmux: pinmux@44e10800 {
		compatible = "pinctrl-single";
		reg = <0x44e10800 0x0238>;
		#address-cells = <1>;
		#size-cells = <0>;
pinctrl-single,register-width = <32>;
		pinctrl-single,function-mask = <0x7f>;	
	};
line 86-91:
ocp {
		compatible = "simple-bus";
		#address-cells = <1>;
		#size-cells = <1>;
		ranges;
		ti,hwmods = "l3_main";
	line 121-142:	
		gpio1: gpio@44e07000 {
			compatible = "ti,omap4-gpio";
			ti,hwmods = "gpio1";
			gpio-controller;
			#gpio-cells = <2>;
			interrupt-controller;
      	#interrupt-cells = <1>;
			reg = <0x44e07000 0x1000>;
			interrupts = <96>;
		};
    
		gpio2: gpio@4804c000 {
			compatible = "ti,omap4-gpio";
			ti,hwmods = "gpio2";
			gpio-controller;
			#gpio-cells = <2>;
			interrupt-controller;
			#interrupt-cells = <1>;
			reg = <0x4804c000 0x1000>;
			interrupts = <98>;
		};
gpio1 and gpio2 are childnodes in OCP node

In this file devices with explicit hardware resources like register-sets ,interrupt numbers etc are described.
As we are going to create a simple custom led device we will be using gpio pins which are implemented by nodes like gpio1 and gpio2. We dont have to explicitely describe seperate hardware resources for this device ,so we will not create our device in this file.

Source fomrat of am335x-bone-common.dtsi
line 9-13:
/include/ "am33xx.dtsi"

/ {
	model = "TI AM335x BeagleBone";
	compatible = "ti,am335x-bone", "ti,am33xx";

am33xx_pinmux: pinmux@44e10800 {
		pinctrl-names = "default";
		pinctrl-0 = <&userled_pins>;
		/*line 30-37*/
		userled_pins: pinmux_userled_pins {
			pinctrl-single,pins = <
				0x54 0x7	/* gpmc_a5.gpio1_21, OUTPUT | MODE7 */
				0x58 0x17	/* gpmc_a6.gpio1_22, OUTPUT_PULLUP | MODE7 */
				0x5c 0x7	/* gpmc_a7.gpio1_23, OUTPUT | MODE7 */
				0x60 0x17	/* gpmc_a8.gpio1_24, OUTPUT_PULLUP | MODE7 */
			>;
		};
/*After this subroutine we will create pinmux-settings for our custom-device*/
/*line 38-43*/	
my_leddevice_pins: pinmux_my_leddevice_pins {
			pinctrl-single,pins = <
				0x028 0x07 /* Configure our GPIO 0_26 */
				0x02c 0x07 /* Configure our GPIO 0_27 */
			>;
		};
/*Now it is time to create the device-node*/
/*line 117-122 before bone_capemgr node */
my_led_device {
			compatible = "led test";
			pinctrl-names = "default";
			pinctrl-0 = <&my_leddevice_pins>;
			
		};
/* compatible property is for binding this device with appropriate driver  using string-matching */

Now our device node is created in the device tree,so it is time to build the device tree blob or flattened device tree in .dtb format. For this go to the kenrel source directory and type
$ make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- am335x-boneblack.dtb
Now the device tree with added node is created it is time to create the driver. Driver code for bilking led
is proviede above.

For building the driver put my_leddriver.c  in linux_source_dir/Drivers/my_led .Now inside this folder open the terminal and type following
$sudo vim kconfig

/*and add those lines and save*/
config MY_LEDDRIVER
	tristate "DEMO DEVICETREE BINDED LED-DRIVER"
	depends on ARM
	default y if ARM
	help
		blinks led in boot time
$sudo vim Makefile
/*and add those lines and save*/
obj-$(CONFIG_MY_LEDDRIVER)	+=  my_leddriver.o
$cd ..
$sudo vim Makefile
/*and add those lines and save*/
obj-$(CONFIG_MY_LEDDRIVER)	+= my_led/
$sudo vim Kconfig
/*and add those lines and save*/
source "drivers/my_led/Kconfig"
Now before the building we can add any of these lines to .config
CONFIG_MY_LEDDRIVER = y #This will make the driver built-in
CONFIG_MY_LEDDRIVER = m #This will make the driver as module
Even if you dont add those lines before building the kernel prompt will ask you which option you want before building my_leddriver.c y/n/m
After selecting your desired option type
$ make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage -j $(nproc)
before making the modules you have to rebuild the kernel for updated module.symvers for updated symbol version.
Now,
$ make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules -j $(nproc)
$ sudo make ARCH=arm INSTALL_MOD_PATH=/path_to_rootfs  modules_install
If you want to make the driver load at boot time,
$ sudo vim /path_to_rootfs/etc/modules
Add line and save
my_leddriver

