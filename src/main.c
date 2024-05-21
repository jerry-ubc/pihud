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
ssd1306_framebuffer_t *fbp_keys = NULL;  //TODO: should these be extern or put in config?
ssd1306_framebuffer_t *fbp_values = NULL;

#define I2C_DEVICE "/dev/i2c-1"  // Check your Pi version, might be i2c-0
#define OLED_ADDR  0x3C         // Typically 0x3C or 0x3D

//Function declarations
void update_display(const char *text);
void delay(int milliseconds);
int init_display();
void cleanup_display();
void update_values(int speed, int engine_speed);

//Globals
int i2c_fd; //TODO: should we make this an extern in config.h?

ssd1306_framebuffer_box_t bbox;

int main() {
    init_display();

    while (1) {
        // update_display("1234567891234\n1234567891");
        sleep(1);
        update_values(888, 77);
        sleep(1);
    }

    // ssd1306_framebuffer_draw_line(fbp, 0, 0, 64, 32, true);
    // ssd1306_framebuffer_draw_line(fbp, 64, 31, 128, 31, true);
    // ssd1306_framebuffer_draw_line(fbp, 0, 0, 0, 32, true);
    // ssd1306_framebuffer_bitdump(fbp);
    // ssd1306_i2c_display_update(oled, fbp);
    return 0;
}


void update_display(const char *text) {
    if (!oled || !fbp_keys) {
        fprintf(stderr, "ERROR: Display not initialized.\n");
        return;
    }
    //TODO: check max length of text
    ssd1306_framebuffer_clear(fbp_keys);
    ssd1306_framebuffer_draw_text(fbp_keys, text, 0, 0, 16, SSD1306_FONT_DEFAULT, 5, &bbox);
    ssd1306_i2c_display_update(oled, fbp_keys);
}

void update_values(int speed, int engine_speed) {
    if (!oled || !fbp_keys) {
        fprintf(stderr, "ERROR: Display not initialized.\n");
        return;
    }
    char speed_str[3]; //concat string to 3 digits
    sprintf(speed_str, "%d", speed);
    char engine_speed_str[3];
    sprintf(engine_speed_str, "%dk", engine_speed);

    // ssd1306_framebuffer_clear(fbp_values);

    ssd1306_framebuffer_draw_text(fbp_keys, "Speed:", 0, 0, 12, SSD1306_FONT_DEFAULT, 4, &bbox);
    ssd1306_framebuffer_draw_text(fbp_keys, "eSpeed:", 0, 0, 28, SSD1306_FONT_DEFAULT, 4, &bbox);
    ssd1306_framebuffer_draw_text(fbp_keys, speed_str, 0, 90, 12, SSD1306_FONT_DEFAULT, 4, &bbox);
    ssd1306_framebuffer_draw_text(fbp_keys, engine_speed_str, 0, 90, 28, SSD1306_FONT_DEFAULT, 4, &bbox);

    ssd1306_i2c_display_update(oled, fbp_keys);
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
    fbp_keys = ssd1306_framebuffer_create(oled->width, oled->height, oled->err);
    if (!fbp_keys) {
        fprintf(stderr, "ERROR: Failed to create framebuffer for keys.\n");
        ssd1306_i2c_close(oled);
        return -1;
    }
    fbp_values = ssd1306_framebuffer_create(oled->width, oled->height, oled->err);
    if (!fbp_values) {
        fprintf(stderr, "ERROR: Failed to create framebuffer for values.\n");
        ssd1306_i2c_close(oled);
        return -1;
    }
    ssd1306_i2c_display_clear(oled);
    ssd1306_framebuffer_clear(fbp_keys);
    ssd1306_framebuffer_clear(fbp_values);

    ssd1306_framebuffer_draw_text(fbp_keys, "Speed:", 0, 0, 12, SSD1306_FONT_DEFAULT, 4, &bbox);
    ssd1306_framebuffer_draw_text(fbp_keys, "eSpeed:", 0, 0, 28, SSD1306_FONT_DEFAULT, 4, &bbox);
    ssd1306_i2c_display_update(oled, fbp_keys);
    return 0;
}

void delay(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;                //milliseconds to seconds
    ts.tv_nsec = (milliseconds % 1000) * 1000000L;  //remainder to nanoseconds

    nanosleep(&ts, NULL);
}