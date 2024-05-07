#include <stdio.h>  // Standard input/output definitions
#include <stdlib.h> // Standard library functions
#include <string.h> // String handling functions
// #include <libserialport.h> // Serial port handling library

int main() {
    struct sp_port *port; // Declare a pointer to a serial port structure
    char *port_name = "/dev/ttyUSB0"; // Serial port device file (adjust as necessary for your setup)

    // Try to get the serial port by its name (e.g., "/dev/ttyUSB0")
    // enum sp_return result = sp_get_port_by_name(port_name, &port);
    // if (result != SP_OK) {
    //     fprintf(stderr, "Error finding serial port\n"); // If there's an error, print it
    //     return -1; // Exit the program with an error code
    // }

    // sp_open(port, SP_MODE_READ);

    //9600 bps is common for many OBD adapters TODO: double check
    sp_set_baudrate(port, 9600);

    sp_set_bits(port, 8);

    //parity commonly None in OBD TODO: double check
    // sp_set_parity(port, SP_PARITY_NONE);

    // Set the number of stop bits used to 1 (standard in many serial communications)
    sp_set_stopbits(port, 1);

    //disable flow control as not typically used for OBD
    // sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);

    //send command to OBD port
    // "ATZ\r" is a command to reset the OBD adapter to its defaults
    const char *cmd = "ATZ\r"; 
    sp_blocking_write(port, cmd, strlen(cmd), 1000); // Write the command, waiting up to 1000 ms

    //buffer reads the response
    char buf[1024];
    int bytes_read = sp_blocking_read(port, buf, sizeof(buf)-1, 2000); //read response, wait up to 2000ms
    buf[bytes_read] = '\0';

    printf("Received: %s\n", buf);

    //close serial port and free associated resources
    sp_close(port);
    sp_free_port(port);

    return 0;
}
