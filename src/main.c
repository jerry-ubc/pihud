//TODO: remove can-isotp folder and its related git stuff
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
#include <ssd1306_graphics.h>       //TODO: is this even doing anything
#include <ssd1306_i2c.h>
#include "led.h"
#include "config.h"

ssd1306_i2c_t *oled = NULL;
ssd1306_framebuffer_t *fbp = NULL;  //TODO: should these be extern or put in config?

#define I2C_DEVICE "/dev/i2c-1"
#define OLED_ADDR  0x3C

//Function declarations
int init_display();
void update_display(const char *text);
void update_values(int speed, int throttle);

void cleanup_display();
void delay(int milliseconds);

//Globals
int i2c_fd; //TODO: should we make this an extern in config.h?

ssd1306_framebuffer_box_t bbox;

int main() {
    init_display();
    while (1) {
        sleep(1);
        update_values(888, 70);
        sleep(1);
    }
    return 0;
}

void update_display(const char *text) {
    if (!oled || !fbp) {
        fprintf(stderr, "ERROR: Display not initialized.\n");
        return;
    }
    //TODO: check max length of text
    ssd1306_framebuffer_clear(fbp);
    ssd1306_framebuffer_draw_text(fbp, text, 0, 0, 16, SSD1306_FONT_DEFAULT, 5, &bbox);
    ssd1306_i2c_display_update(oled, fbp);
}

void update_values(int speed, int throttle) {
    if (!oled || !fbp) {
        fprintf(stderr, "ERROR: Display not initialized.\n");
        return;
    }
    char speed_str[3]; //concat string to 3 digits
    sprintf(speed_str, "%d", speed);
    char throttle_str[4];
    sprintf(throttle_str, "%d%%", throttle);

    ssd1306_framebuffer_draw_text(fbp, "Speed", 0, 0, 12, SSD1306_FONT_DEFAULT, 4, &bbox);
    ssd1306_framebuffer_draw_text(fbp, "Throttle", 0, 0, 28, SSD1306_FONT_DEFAULT, 4, &bbox);
    ssd1306_framebuffer_draw_text(fbp, speed_str, 0, 96, 12, SSD1306_FONT_DEFAULT, 4, &bbox);
    if (throttle < 100)
        ssd1306_framebuffer_draw_text(fbp, throttle_str, 0, 90, 28, SSD1306_FONT_DEFAULT, 4, &bbox);
    else
        ssd1306_framebuffer_draw_text(fbp, throttle_str, 0, 79, 28, SSD1306_FONT_DEFAULT, 4, &bbox);

    ssd1306_i2c_display_update(oled, fbp);
}

int init_display() {
    const char *filename = I2C_DEVICE;
    oled = ssd1306_i2c_open(filename, 0x3c, 128, 32, NULL);
    if (!oled) {
        printf("ERROR: Failed to open I2C device.\n");
        return -1;
    }
    if (ssd1306_i2c_display_initialize(oled) < 0) {
        fprintf(stderr, "ERROR: Failed to initialize the display. Check if it is connected!\n");
        ssd1306_i2c_close(oled);
        return -1;
    }
    fbp = ssd1306_framebuffer_create(oled->width, oled->height, oled->err);
    if (!fbp) {
        fprintf(stderr, "ERROR: Failed to create framebuffer for keys.\n");
        ssd1306_i2c_close(oled);
        return -1;
    }
    ssd1306_i2c_display_clear(oled);
    ssd1306_framebuffer_clear(fbp);

    ssd1306_framebuffer_draw_text(fbp, "Speed", 0, 0, 12, SSD1306_FONT_DEFAULT, 4, &bbox);
    ssd1306_framebuffer_draw_text(fbp, "Throttle", 0, 0, 28, SSD1306_FONT_DEFAULT, 4, &bbox);
    ssd1306_i2c_display_update(oled, fbp);
    return 0;
}

void delay(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;                //milliseconds to seconds
    ts.tv_nsec = (milliseconds % 1000) * 1000000L;  //remainder to nanoseconds

    nanosleep(&ts, NULL);
}