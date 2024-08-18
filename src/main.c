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

// Function declarations
void init_serial();
int init_display();
void update_values(int speed, int throttle);
void send_command(const char *service, const char *cmd);

// Globals
int i2c_fd; //TODO: should we make this an extern in config.h?

int speed = 999, throttle = 999;

ssd1306_framebuffer_box_t bbox;

int main() {
    init_display();
    init_serial();
    while (1) {
        send_command("01", "0D");   // Vehicle speed (km/h)
        send_command("01", "11");   // Throttle (%)
        update_values(speed, throttle);
        // sleep(1);
    }
    sp_close(port);
    sp_free_port(port);
    return 0;
}

void update_values(int speed, int throttle) {
    if (!oled || !fbp) {
        fprintf(stderr, "ERROR: Display not initialized.\n");
        printf("ERROR: DISPLAY NOT INITIALIZED\n");
        return;
    }
    char speed_str[3]; //concat string to 3 digits
    sprintf(speed_str, "%03d", speed);
    char throttle_str[4];
    sprintf(throttle_str, "%03d", throttle);

    ssd1306_framebuffer_draw_text(fbp, "Speed", 0, 0, 12, SSD1306_FONT_DEFAULT, 4, &bbox);
    ssd1306_framebuffer_draw_text(fbp, "Throttle", 0, 0, 28, SSD1306_FONT_DEFAULT, 4, &bbox);
    ssd1306_framebuffer_draw_text(fbp, speed_str, 0, 96, 12, SSD1306_FONT_DEFAULT, 4, &bbox);
    ssd1306_framebuffer_draw_text(fbp, throttle_str, 0, 96, 28, SSD1306_FONT_DEFAULT, 4, &bbox);

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
        printf("ERROR: FAILED INITIALIZING\n");
        ssd1306_i2c_close(oled);
        return -1;
    }
    fbp = ssd1306_framebuffer_create(oled->width, oled->height, oled->err);
    if (!fbp) {
        fprintf(stderr, "ERROR: Failed to create framebuffer for keys.\n");
        printf("ERROR: FAILED FRAMEBUFFER\n");
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
        printf("ERROR: OPENING\n");
        sp_free_port(port); // Always free the port if not needed anymore
        return;
    }
    result = sp_set_baudrate(port, 38400);
    result = sp_set_bits(port, 8);
    result |= sp_set_parity(port, SP_PARITY_NONE);
    result |= sp_set_stopbits(port, 1);
    result |= sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);
    if (result != SP_OK) {
        // fprintf(stderr, "Failed to configure port settings: %s\n", sp_last_error_message());
        printf("ERROR: FAILED TO OPEN PORT\n");
        sp_close(port);
        sp_free_port(port);
        return;
    }

    // send_command("ATZ", "");    // Reset the device
    // send_command("ATE0", "");   // Disable echo
    // send_command("ATL0", "");   // Disable linefeed
    // send_command("ATSP0", "");  // Set protocol to auto
}

// To test commands directly, use 'minicom -b 38400 -D /dev/ttyUSB0'
// To exit minicom, enter Ctrl+A then X
void send_command(const char *service, const char *cmd) {
    //TODO: sometimes this will return ">AN ERROR 01 11", make sure you handle this case
    //TODO: sometimes will send ">TOPPED"
    //TODO: will send "SEARCHING..." if sim is disconnected but program is running
    //TODO: also make sure you handle the case where the simulator gets turned off after starting connected
    //TODO: also consider making a log for faults so that you can trace what happened
    char command[256];
    sprintf(command, "%s %s\r", service, cmd); // Append carriage return for ELM327 command
    sp_blocking_write(port, command, strlen(command), 1000);
    // usleep(100000); // Delay to ensure ELM327 processes the command


    char response[1024];
    int bytes_read = sp_blocking_read(port, response, sizeof(response)-1, 1000);
    int A = 0, B = 0;
    int prefix = 0;
    int pid = 0;
    int garbage = 0;
    response[bytes_read] = '\0';
    printf("%s\n", response);

    // Error detection
    if (strstr(response, ">STOPPED")) {
        printf("Device stopped. Attempting to reset...\n");
        send_command("ATZ", "");  // Reset the device if it stopped
        return;
    }
    else if (strstr(response, "SEARCHING...")) {
        printf("Simulator not found. Ensure the simulator is powered on and connected.\n");
        return;
    }
    else if (strstr(response, ">ERROR")) {
        printf("Received error from ELM327. Command may be invalid.\n");
        return;
    }
    else if (strstr(response, ">NO DATA")) {
        printf("No data available for the requested PID.\n");
        return;
    }

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

    // This solution is more 'elegant', but garbage value in the middle ruins it
    // int args_scanned = sscanf(response, "%x %x %x %x", &prefix, &pid, &garbage, &A);
    // printf("%x\n", prefix);
    // printf("%x\n", pid);
    // printf("%x\n", garbage);
    // printf("%x\n", A);
    // printf("%x\n", B);
    // printf("args scanned: %d\n", args_scanned);
}