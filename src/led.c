#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "led.h"
#include "config.h"

int readLED(const char *path, const char *filename) {
    char fullPath[128]; //TODO: why 128, stop with magic numbers
    snprintf(fullPath, sizeof(fullPath), "%s/%s", path, filename);
    char value[10]; //TODO: WHY 10

    int fd = open(fullPath, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open %s for reading\n", fullPath);
        perror("Error");
        exit(EXIT_FAILURE);
    }
    if (read(fd, value, sizeof(value) - 1) == -1) {
        fprintf(stderr, "Failed to read value from %s\n", fullPath);
        perror("Error");
        close(fd);
        exit(EXIT_FAILURE);
    }
    int int_value = atoi(value);
    close(fd);
    return int_value; //TODO: is atoi safe?
}

// Function to write values to files in the LED control directory
void writeLED(const char *path, const char *filename, const char *value) {
    char fullPath[128]; //TODO: magic number
    snprintf(fullPath, sizeof(fullPath), "%s/%s", path, filename);
    
    int fd = open(fullPath, O_WRONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open %s for writing\n", fullPath);
        perror("Error");
        exit(EXIT_FAILURE);
    }
    if (write(fd, value, strlen(value)) == -1) {
        fprintf(stderr, "Failed to write value %s to %s\n", value, fullPath);
        perror("Error");
        exit(EXIT_FAILURE);
    }
    close(fd);
}

void toggleLED(const char *led_path) {
    if (IS_LED_ON(readLED(led_path, "brightness"))) {
        writeLED(led_path, "brightness", "0");
    }
    else {
        writeLED(led_path, "brightness", "1");
    }
}