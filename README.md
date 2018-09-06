# snickerdoodle-hello-world
How to get an LED turning on and off on a Snickerdoodle (Zynq dev board)

## Prerequisites
* Xilinx Vivado tools
  * I am using 2018.2 on Ubuntu 16.04 LTS
* Snickerdoodle
  * I have a regular Snickerdoodle with Z7010 and 1 GB RAM, other models may or may not be covered in the future
* micro SD card
  * At least 4 GB suggested, faster = less painful

## Preparing SD card image
Download the appropriate zip from http://krtkl.com/downloads.
The instructions at https://wiki.krtkl.com/index.php?title=SD_Card work for the most part. However the `parted` commands in the script didn't work for me. I ended up doing the card partitioning manually and running the portion of the script below the `parted` commands. I had to install `kpartx` and maybe `losetup` for the script.

## Talking to Snickerdoodle
Assuming the SD card was built correctly, install the SD card and plug the Snickerdoodle in via USB. After waiting some tens of seconds (why so long?), I was able to run `screen /dev/ttyACM0 115200` on my Linux machine. There is some kind of shell that will come up if you hit enter. To boot into Linux, either hold the physical reset button until things start flashing, or type `reset` at the shell.

Placeholder: network configuration.

## Building FPGA bitstream
Open Vivado. In the tcl console, `cd` to this directory, then enter `source regen.tcl`. Vivado will regenerate the project from the tcl script.

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
This is a bit of a side quest. The most straightforward way to "upload" the bitstream is to copy it to the SD card by hand. But of course that gets old real quick. Either way, you'll want to add the line `bitstream_image=bitstream.bit` to `uEnv.txt` in the boot partition of the SD card.

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

(work in progress)
