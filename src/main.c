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
ssd1306_framebuffer_t *fbp = NULL;  //TODO: should these be extern?

#define I2C_DEVICE "/dev/i2c-1"  // Check your Pi version, might be i2c-0
#define OLED_ADDR  0x3C         // Typically 0x3C or 0x3D

//Function declarations
void initialize_components();
void display_startup_message();
void update_display(const char *text);
void speed_alert();
void delay(int milliseconds);
int init_display();
void cleanup_display();
void display_text(const char *text, int x, int y);

//Globals
int i2c_fd; //TODO: should we make this an extern in config.h?

int main() {
    init_display();  // Initialize the display

    // Your application's main loop
    while (1) {
        // update_display("goodnight annie :)");
        // sleep(5);
        // update_display("secondasdfasdf");
    }
    // const char *filename = "/dev/i2c-1";
    // ssd1306_i2c_t *oled = ssd1306_i2c_open(filename, 0x3c, 128, 32, NULL);
    // if (!oled) {
    //     printf("ERROR!\n");;
    // }
    // if (ssd1306_i2c_display_initialize(oled) < 0) {
    //     fprintf(stderr, "ERROR: Failed to initialize the display. Check if it is connected !\n");
    //     ssd1306_i2c_close(oled);
    //     return -1;
    // }
    // sleep(3);
    // ssd1306_framebuffer_t *fbp = ssd1306_framebuffer_create(oled->width, oled->height, oled->err);

    // ssd1306_i2c_display_clear(oled);
    // ssd1306_framebuffer_clear(fbp);
    // ssd1306_framebuffer_box_t bbox;

    // ssd1306_framebuffer_draw_text(fbp, "My text", 0, 32, 16, SSD1306_FONT_DEFAULT, 4, &bbox);
    // ssd1306_i2c_display_update(oled, fbp);

    // ssd1306_framebuffer_draw_line(fbp, 0, 0, 64, 32, true);
    // ssd1306_framebuffer_draw_line(fbp, 64, 31, 128, 31, true);
    // ssd1306_framebuffer_draw_line(fbp, 0, 0, 0, 32, true);
    // ssd1306_framebuffer_bitdump(fbp);
    // ssd1306_i2c_display_update(oled, fbp);
    // sleep(3);
    // ssd1306_i2c_run_cmd(oled, SSD1306_I2C_CMD_DISP_INVERTED, 0, 0);
    // sleep(3);
    // ssd1306_i2c_run_cmd(oled, SSD1306_I2C_CMD_DISP_NORMAL, 0, 0);
    // cleanup_display();  // Clean up resources
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

// void write_i2c(int command) {
//     if (write(i2c_fd, &command, 1) != 1) {
//         perror("Failed to write to the i2c device");
//     }
// }

void update_display(const char *text) {
    if (!oled || !fbp) {
        fprintf(stderr, "ERROR: Display not initialized.\n");
        return;
    }
    // ssd1306_framebuffer_clear(fbp);
    // ssd1306_i2c_display_clear(oled);

    ssd1306_framebuffer_box_t bbox;
    ssd1306_framebuffer_draw_text(fbp, text, 0, 0, 16, SSD1306_FONT_DEFAULT, 3, &bbox);
    ssd1306_i2c_display_update(oled, fbp);

    // Optionally draw additional graphics or update display further here
}


int init_display() {
    const char *filename = "/dev/i2c-1";
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
        fprintf(stderr, "ERROR: Failed to create framebuffer.\n");
        ssd1306_i2c_close(oled);
        return -1;
    }
    ssd1306_i2c_display_clear(oled);
    ssd1306_framebuffer_clear(fbp);
    return 0;
}