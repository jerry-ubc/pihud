#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "led.h"

int readLED(const char *path, const char *filename) {
    char fullPath[128]; //TODO: why 128, stop with magic numbers
    snprintf(fullPath, sizeof(fullPath), "%s/%s", path, filename);
    char value[10]; //TODO: WHY 10

    int fd = open(fullPath, O_RDONLY);
    printf("middle2\n");
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

// int main(int argc, char *argv[]) {
//     if (argc < 3) {
//         fprintf(stderr, "Usage: %s <brightness|trigger> <value>\n", argv[0]);
//         return 1;
//     }

//     const char *ledBasePath = "/sys/class/leds/ACT";  // Change to your specific LED path if different

//     // Choosing action based on the command-line argument
//     if (strcmp(argv[1], "brightness") == 0) {
//         writeLED(ledBasePath, "brightness", argv[2]);
//     } else if (strcmp(argv[1], "trigger") == 0) {
//         writeLED(ledBasePath, "trigger", argv[2]);
//     } else {
//         fprintf(stderr, "Invalid command. Use 'brightness' or 'trigger'.\n");
//         return 1;
//     }

//     return 0;
// }
