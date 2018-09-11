#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define MAP_SIZE 0x2000
#define GPIO2DATA 0x1000/4
#define GPIO2TRIS 0x1004/4

int main(int argc, char *argv[])
{
    int fd = 0;
    char *uiod = "/dev/uio0";

    volatile unsigned *gpio;
    off_t offset = 0; // 0x01200000;
    unsigned gpio2data,gpio2tris;

    fd = open(uiod, O_RDWR);
    if (fd < 1) {
        perror(argv[0]);
        printf("Invalid UIO device file: '%s'n", uiod);
        return -1;
    }

    gpio = (volatile unsigned *)mmap(NULL, MAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, offset);
    if (!gpio) {
        perror(argv[0]);
        printf("mmapn");
        return -1;
    } else {
        printf("mmapped!\n");
    }

    printf("GPIO 0 DATA: %8x\n", gpio[0]);
    printf("GPIO 0 TRIS: %8x\n", gpio[1]);
    printf("GPIO 1 DATA: %8x\n", gpio[2]);
    printf("GPIO 1 TRIS: %8x\n", gpio[3]);
    printf("GPIO 2 DATA: %8x\n", gpio[0x1000/4+0]);
    printf("GPIO 2 TRIS: %8x\n", gpio[0x1000/4+1]);
    printf("GPIO 3 DATA: %8x\n", gpio[0x1000/4+2]);
    printf("GPIO 3 TRIS: %8x\n", gpio[0x1000/4+3]);

    gpio2tris = gpio[GPIO2TRIS]; 
    if ((gpio2tris >> 24 ) & 0x1) {
        printf("Setting LED TRIS to OUT\n");
        gpio[GPIO2TRIS] = gpio2tris & ~(0x1 << 24);
    }
    gpio2data = gpio[GPIO2DATA]; 
    gpio[GPIO2DATA] = gpio2data ^ (0x1 << 24); 
    printf("LED state flipped!\n");

    munmap((void*)gpio, MAP_SIZE);
    return 0;
}

