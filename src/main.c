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
#include <ctype.h>
#include "led.h"
#include "config.h"

struct sp_port *port;

ssd1306_i2c_t *oled = NULL;
ssd1306_framebuffer_t *fbp = NULL;  //TODO: should these be extern or put in config?

#define I2C_DEVICE "/dev/i2c-1"
#define OLED_ADDR  0x3C

//Function declarations
int init_display();
void update_display(const char *text);
void update_values(int speed, int throttle);
void update_display(const char *text);
void init_serial();
void send_command(const char *service, const char *cmd);
void read_response(char *buffer, int len);

void cleanup_display();
void delay(int milliseconds);

//Globals
int i2c_fd; //TODO: should we make this an extern in config.h?

int speed, throttle;

ssd1306_framebuffer_box_t bbox;

int main() {
    init_display();
    init_serial();
    while (1) {
        send_command("01", "0D");   //vehicle speed
        send_command("01", "11");   //throttle
        update_values(speed, throttle);
    }
    sp_close(port);
    sp_free_port(port);
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
    sprintf(speed_str, "%03d", speed);
    char throttle_str[4];
    sprintf(throttle_str, "%03d", throttle);

    ssd1306_framebuffer_draw_text(fbp, "Speed", 0, 0, 12, SSD1306_FONT_DEFAULT, 4, &bbox);
    ssd1306_framebuffer_draw_text(fbp, "Throttle", 0, 0, 28, SSD1306_FONT_DEFAULT, 4, &bbox);
    ssd1306_framebuffer_draw_text(fbp, speed_str, 0, 96, 12, SSD1306_FONT_DEFAULT, 4, &bbox);
    // if (throttle < 100)
        ssd1306_framebuffer_draw_text(fbp, throttle_str, 0, 96, 28, SSD1306_FONT_DEFAULT, 4, &bbox);
    // else
    //     ssd1306_framebuffer_draw_text(fbp, throttle_str, 0, 79, 28, SSD1306_FONT_DEFAULT, 4, &bbox);

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

void init_serial() {
    sp_get_port_by_name("/dev/ttyUSB0", &port);
    enum sp_return result = sp_open(port, SP_MODE_READ_WRITE);
    if (result != SP_OK) {
        // fprintf(stderr, "Error opening port: %s\n", sp_last_error_message());
        printf("ERROR OPENING\n");
        sp_free_port(port); // Always free the port if not needed anymore
        return;
    }
    result = sp_set_baudrate(port, 38400);
    if (result != SP_OK) {
        // fprintf(stderr, "Failed to set baudrate: %s\n", sp_last_error_message());
        printf("Failed to set baudrate\n");
        sp_close(port);
        sp_free_port(port);
        return;
    }
    result = sp_set_bits(port, 8);
    result |= sp_set_parity(port, SP_PARITY_NONE);
    result |= sp_set_stopbits(port, 1);
    result |= sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);
    if (result != SP_OK) {
        // fprintf(stderr, "Failed to configure port settings: %s\n", sp_last_error_message());
        printf("FAILED TO OPEN PORT\n");
        sp_close(port);
        sp_free_port(port);
        return;
    }
}

void send_command(const char *service, const char *cmd) {
    //TODO: sometimes this will return ">AN ERROR 01 11", make sure you handle this case
    //TODO: also make sure you handle the case where the simulator gets turned off after starting connected
    //TODO: also consider making a log for faults so that you can trace what happened
    char command[256];
    sprintf(command, "%s %s\r", service, cmd); // Append carriage return for ELM327 command
    sp_blocking_write(port, command, strlen(command), 1000);

    char response[1024];
    int bytes_read = sp_blocking_read(port, response, sizeof(response)-1, 1000);
    int A = 0, B = 0;
    int prefix = 0;
    int pid = 0;
    int garbage = 0;
    response[bytes_read] = '\0';
    // printf("%s\n", response);

    // Use strtok to split the response by spaces
    char *token = strtok(response, " ");

    if (token != NULL) {
        // printf("%s\n", token);
        prefix = strtol(token, NULL, 16);
        token = strtok(NULL, " ");
    }
    if (token != NULL) {
        // printf("%s\n", token);
        garbage = strtol(token, NULL, 16);
        token = strtok(NULL, " ");
    }
    if (token != NULL) {
        // printf("%s\n", token);
        pid = strtol(token, NULL, 16);
        token = strtok(NULL, " ");
    }
    if (token != NULL) {
        // printf("%s\n", token);
        A = strtol(token, NULL, 16);
        token = strtok(NULL, " ");
    }
    if (token != NULL) {
        // printf("%s\n", token);
        B = strtol(token, NULL, 16);
    }

    switch (pid) {
        case 0x0D:
            speed = A;
            break;
        case 0X11:
            throttle = (double) 100 / 255 * A;
            break;
    }

    // int args_scanned = sscanf(response, "%x %x %x %x", &prefix, &pid, &garbage, &A);
    // printf("%x\n", prefix);
    // printf("%x\n", pid);
    // printf("%x\n", garbage);
    // printf("%x\n", A);
    // printf("%x\n", B);
    // printf("args scanned: %d\n", args_scanned);
}

void read_response(char *buffer, int len) {
    sp_blocking_read(port, buffer, len, 2000);
}

void delay(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;                //milliseconds to seconds
    ts.tv_nsec = (milliseconds % 1000) * 1000000L;  //remainder to nanoseconds

    nanosleep(&ts, NULL);
}