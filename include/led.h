#ifndef LED_H
#define LED_H

#define MAX_BRIGHTNESS 255
#define IS_LED_ON(brightness) ((brightness) > 0 ? 1 : 0)

int readLED(const char *path, const char *filename);
void writeLED(const char *path, const char *filename, const char *value);
void toggleLED(const char *led_path);

#endif
