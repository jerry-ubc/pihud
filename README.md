# Pi HUD #
This repository contains the code and documentation for a custom-built Heads-Up Display (HUD) for cars, 
powered by a Raspberry Pi Zero 2W and connected through the car's OBD-II port. The HUD displays real-time 
vehicle information such as engine speed, vehicle speed, and other diagnostic data directly on the car's 
windshield.

## Features ##
__Real-Time Speed Display__: Shows the current speed of the vehicle.  
__Engine RPM__: Displays the revolutions per minute of the vehicle's engine.  
__Diagnostic Data__: Access and display various diagnostic data points provided by the vehicle's OBD-II 
system.  
__Customizable Layout__: Easily modify the layout and data points displayed on the HUD.  

## Methodology ##
### Vehicle Interface ###
There are 2 commonly used options to gather information from the vehicle, via the (1) OBD-II port, or (2)
CAN bus. Since opting for the CAN bus method requires a CAN DBC file (i.e., CAN database), which reequires
reverse engineering to decipher into human-readable information, I am going to start the project by using
the OBD-II port and potentially upgrade when the CAN DBC file for my vehicle is available or I have time
to reverse engineer it myself.

### Hardware System ###
The brains of the project is a Raspberry Pi Zero 2W. Eventually, I will use an OLED to project the 
information onto the windshield. If the project seems hopeful and I am able to establish a connection with
the ODB adapter, then I will look into options for implementing a transceiver to communicate wirelessly.  

## Hardware Setup ##
__Raspberry Pi Zero 2W__
__Breadboard__
__OLED Display__
TODO
## Software Setup ##
TODO

## Bugs Encountered ##
If you are also using a Pi Zero, you may run into issues running `sudo apt update` (specifically freezing
the device while "Reading changelogs... XX%", which may be a result of the swap space size. I was able to
fix this by running the following commands:
```
sudo dphys-swapfile swapoff
sudo nano /etc/dphys-swapfile
```
Then modify the line to `CONF_SWAPSIZE=1024`.
```
sudo dphys-swapfile setup
sudo dphys-swapfile swapon
```

## Design Decisions ##
### OBD-II vs. CAN Bus ###
TODO
### OLED vs. LCD Display ###
In this project, I opted to use organic light-emitting diodes (OLEDs) since they tend to provide better 
picture quality (e.g., greater contrast, deeper blacks, etc.) compared to their counterparts: liquid crystal 
displays (LCDs). Furthermore, OLEDs are able to maintain their picture quality at a much greater viewing 
angle due to its nature of having true blacks. However, they come with the side effect lower lifespan and 
potentially imprinting images into the display if lit for extended durations.
