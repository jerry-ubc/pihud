#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libserialport.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <time.h>

#include "led.h"
#include "config.h"

//Function declarations
void initialize_components();
void display_startup_message();
void update_display();
void speed_alert();
void delay(int milliseconds);

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
    return;
}
void display_startup_message() {
    return;
}
void update_display() {
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
