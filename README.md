# snickerdoodle-hello-world
How to get an LED turning on and off on a Snickerdoodle (Zynq dev board)

## Prerequisites
* Xilinx Vivado tools
  * I am using 2018.2 on Ubuntu 16.04 LTS
* Snickerdoodle
  * I have a regular Snickerdoodle with Z7010 and 1 GB RAM, other models may or may not be covered in the future
* micro SD card
  * At least 4 GB suggested, faster speed class = less painful

## Preparing SD card image
Download the appropriate zip from http://krtkl.com/downloads.
The instructions at https://wiki.krtkl.com/index.php?title=SD_Card work for the most part. However the `parted` commands in the script didn't work for me. I ended up doing the card partitioning manually and running the portion of the script below the `parted` commands. I had to install `kpartx` and maybe `losetup` for the script.

## Talking to Snickerdoodle
Assuming the SD card was built correctly, install the SD card and plug the Snickerdoodle in via USB. After waiting some tens of seconds (why so long?), I was able to run `screen /dev/ttyACM0 115200` on my Linux machine. There is some kind of shell that will come up if you hit enter. To boot into Linux, either hold the physical reset button until things start flashing, or type `reset` at the shell.

Placeholder: network configuration.

## Building FPGA bitstream
Open Vivado. In the tcl console, `cd` to where you've cloned this directory, then enter `source regen.tcl`. Vivado will regenerate the project from the tcl script.

The memory map is set up as follows. Note that this differs from the official krtkl example project.

| Address       | Register      |
| ------------- | ------------- |
| `0x4000_0000`    | GPIO 0 Data   |
| `0x4000_0004`    | GPIO 0 Tristate   |
| `0x4000_0008`    | GPIO 1 Data   |
| `0x4000_000c`    | GPIO 1 Tristate   |
| `0x4000_1000`    | GPIO 2 Data   |
| `0x4000_1004`    | GPIO 2 Tristate   |
| `0x4000_1008`    | GPIO 3 Data   |
| `0x4000_100c`    | GPIO 3 Tristate   |

## Uploading bitstream
This is a bit of a side quest. The most straightforward way to "upload" the bitstream is to copy it to the SD card by hand. But of course that gets old real quick. Either way, you'll want to add the line `bitstream_image=bitstream.bit` as the second line of `uEnv.txt` in the boot partition of the SD card.

You can mount the boot partion of the SD card and access it from the Snickerdoodle's Linux command line.
```
admin@snickerdoodle:~$ pwd
/home/admin
admin@snickerdoodle:~$ mkdir sdboot
admin@snickerdoodle:~$ sudo mount /dev/mmcblk0p1 /home/admin/sdboot
```
Once the bitstream has been generated, it can be copied over the network, assuming the Snickerdoodle is on the same local network:
```
$ scp ./snickerdoodle_GPIO/snickerdoodle_GPIO.runs/impl_1/base_block_design_wrapper.bit admin@snickerdoodle:~/bitstream.bit
```
Then, on the Snickerdoodle, you can `sudo cp` the bitstream to `sdboot`. Copying to the SD card in one command is left as an exercise for the reader. One might also use the `tcl.post` feature in Vivado to automate updating bitstreams.

## Compiling C example

A bit of exposition: if you've programmed microcontrollers before, you're probably used to accessing peripherals at their memory-mapped addresses, easy peasy. If you use Zynq at the bare metal level through SDK, it's the same way. However when an OS like Linux gets involved, you can't just access memory locations directly. You've got to go through the kernel. Historically this meant writing an honest-to-goodness kernel driver, but thankfully we can make use of the `uio` infrastructure to more easily access sections of memory.

Normally you would set up your `uio` device in the Linux devicetree, but for the time being we'll make use of the infrastructure krtkl has built into their images. They've made sixteen `uio` devices that can each cover addresses `0x4000_0000` and up. (Todo: find DTS)
```
admin@snickerdoodle:~$ ls /dev/uio*
/dev/uio0   /dev/uio11  /dev/uio14  /dev/uio3  /dev/uio6  /dev/uio9
/dev/uio1   /dev/uio12  /dev/uio15  /dev/uio4  /dev/uio7
/dev/uio10  /dev/uio13  /dev/uio2   /dev/uio5  /dev/uio8
admin@snickerdoodle:~$ ls /sys/devices/soc0/amba/amba\:fpga-axi\@0/
40000000.uio0   40000000.uio13  40000000.uio4  40000000.uio9    subsystem
40000000.uio1   40000000.uio14  40000000.uio5  driver_override  uevent
40000000.uio10  40000000.uio15  40000000.uio6  modalias
40000000.uio11  40000000.uio2   40000000.uio7  of_node
40000000.uio12  40000000.uio3   40000000.uio8  power
```
For the blinking LED, I've chosen pin 4 on connector JB1. This corresponds to the FPGA pin T19, called IO_25_34, and assigned to GPIO2[24] in the Vivado project. Connect this pin to a LED, with a series resistor of at least 1Kohm to ground. (Todo: diagram) Don't freak out when the LED appears to light up, that's just because the default setup has pullups enabled on the GPIOs.

Copy `uiotest.c` over to the Snickerdoodle and compile it.
```
admin@snickerdoodle:~$ gcc -Wall uiotest.c 
admin@snickerdoodle:~$ sudo ./a.out 
mmapped!
GPIO 0 DATA:        0
GPIO 0 TRIS:  1ffffff
GPIO 1 DATA:        0
GPIO 1 TRIS:  1ffffff
GPIO 2 DATA:  13fff03
GPIO 2 TRIS:   ffffff
GPIO 3 DATA:  1cc0fff
GPIO 3 TRIS:  1ffffff
LED state flipped!
```
(You will probably read different data since I have a custom board plugged into JB1/JB2.) The program operates as follows. The section of memory containing the GPIO registers is mapped via `mmap`. The value of each of the registers is read out. If the output direction (TRIS bit) of the LED pin is not set, it is set to output. Finally, the value of the LED output is inverted. 

## Further reading
[UIO: user-space drivers](https://lwn.net/Articles/232575/), LWN 2007
[How to Design and Access a Memory-Mapped Device in Programmable Logic from Linaro Ubuntu Linux on Xilinx Zynq on the ZedBoard, Without Writing a Device Driver – Part One](http://fpga.org/2013/05/28/how-to-design-and-access-a-memory-mapped-device-part-one/) and [Part 2](http://fpga.org/2013/05/28/how-to-design-and-access-a-memory-mapped-device-part-two/), Jan Gray
