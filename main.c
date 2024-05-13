#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libserialport.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

//Initialize serial port
// struct sp_port* init_serial(const char *port_name) {
//     struct sp_port *port;
//     sp_get_port_by_name(port_name, &port);
//     sp_open(port, SP_MODE_READ_WRITE);
//     sp_set_baudrate(port, 9600);
//     sp_set_bits(port, 8);
//     sp_set_parity(port, SP_PARITY_NONE);
//     sp_set_stopbits(port, 1);
//     sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);
//     return port;
// }

//Send OBD command and read response
// void send_obd_command(struct sp_port *port, const char *cmd) {
//     sp_blocking_write(port, cmd, strlen(cmd), 1000);
//     char buf[128];
//     int bytes_read = sp_blocking_read(port, buf, sizeof(buf) - 1, 2000);
//     buf[bytes_read] = '\0';
//     printf("Response: %s\n", buf);
// }

//Initialize OLED display
// int init_oled(const char *filename, int address) {
//     int file;
//     if ((file = open(filename, O_RDWR)) < 0) {
//         perror("Failed to open the i2c bus");
//         exit(1);
//     }
//     // if (ioctl(file, I2C_SLAVE, address) < 0) {
//     //     perror("Failed to acquire bus access and/or talk to slave.");
//     //     exit(1);
//     // }
//     return file;
// }

//Display messages on the OLED
// void display_on_oled(int file, const char *message) {
//     write(file, message, strlen(message));  // Simplified for example
// }

int main() {
    // struct sp_port *port = init_serial("/dev/ttyUSB0");

    printf("Hello world!\n");

    // Test each component separately:
    // To test the serial communication, uncomment the following lines one at a time:
    // send_obd_command(port, "ATZ\r");  //reset command, check device response
    // send_obd_command(port, "010C\r"); //engine RPM
    // send_obd_command(port, "010D\r"); //vehicle speed
    // send_obd_command(port, "0105\r"); //coolant temp

    // int oled_file = init_oled("/dev/i2c-1", 0x3C);  //TODO: adjust I2C address if necessary

    //to test OLED
    // display_on_oled(oled_file, "TEST");

    // Full Integration Testing (after individual tests pass):
    // Comment out the tests and uncomment below to integrate OBD-II read with OLED display
    // send_obd_command(port, "010C\r");  // Uncomment this and adjust to display on OLED

    //close serial port and OLED file descriptor
    // sp_close(port);
    // sp_free_port(port);
    // close(oled_file);

    return 0;
}
