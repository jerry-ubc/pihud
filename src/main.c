#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libserialport.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <time.h>
#include <errno.h>
#include "led.h"
#include "config.h"

#define I2C_DEVICE "/dev/i2c-1"  // Check your Pi version, might be i2c-0
#define OLED_ADDR  0x3C         // Typically 0x3C or 0x3D

//Function declarations
void initialize_components();
void display_startup_message();
void update_display();
void speed_alert();
void delay(int milliseconds);

//Globals
int i2c_fd; //TODO: should we make this an extern in config.h?

int main() {
    initialize_components();
    display_startup_message();
    
    while (1) {
        update_display();
        speed_alert();
        toggleLED(main_led);
        delay(1000);  //delay for 1 second between updates
    }
    return 0;
}

void initialize_components() {
    if ((i2c_fd = open(I2C_DEVICE, O_RDWR)) < 0) {
        perror("Failed to open the i2c bus");
        exit(1);
    }
    if (ioctl(i2c_fd, I2C_SLAVE, OLED_ADDR) < 0) {
        perror("Failed to communicate with the OLED device");
        exit(1);
    }
    return;
}
void display_startup_message() {
    return;
}
void speed_alert() {
    return;
}

void delay(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;                //milliseconds to seconds
    ts.tv_nsec = (milliseconds % 1000) * 1000000L;  //remainder to nanoseconds

    nanosleep(&ts, NULL);
}

void write_i2c(int command) {
    if (write(i2c_fd, &command, 1) != 1) {
        perror("Failed to write to the i2c device");
    }
}

void update_display() {
    // Commands to control what to display
    // For instance, clearing the display, setting cursor position, etc.
    write_i2c(0x00);  // Example command
}
