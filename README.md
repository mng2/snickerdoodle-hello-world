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
The instructions at https://wiki.krtkl.com/index.php?title=SD_Card work for the most part. However the `parted` commands in the script didn't work for me. I ended up doing the card partitioning manually and running the script below the `parted` commands.

## Building FPGA bitstream
Open Vivado. In the tcl console, `cd` to this directory, then enter `source regen.tcl`. Vivado will regenerate the project from the tcl script.

The memory map is set up as follows. Note that this differs from how the official krtkl example sets things up.

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

## Compiling C example

(work in progress)
