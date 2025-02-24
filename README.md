# AlbertaSAT Ex-Alta3 Iris (Payload) Firmware

... Documentation is still being developed ...

This repository contains the firmware for the Payload, Iris, of Ex-Alta3. This firmware is used to interface with the custom 'Theia' PCB developed for Iris which is used to interface with the Camera Modules, monitor temperature and current flow, and transmit captured images back to OBC.

This firmware will run on a Raspberry Pi Compute Module 4 running 'Raspberry Pi OS Lite' as its operating system. 

# Repo Organization
## PCB Firmware

The PCB Firmware code has been divided into 4 folders.

General Inc Folder: [inc/](inc/)<br>
General C Folder:   [src/](src/)<br>
Main Folder:        [main_service/](main_service/)<br>
SPI Folder:         [spi_service/](spi_service/)<br>

The 'main_service/' folder contains the 'main' function for the Theia Service responsible for interfacing with the PCB. It is the primary service for the Payload and is the largest portion of this repository.<br>

The 'spi_service/' folder contains the 'main' function for the SPI Service responsibele for interfacing with the OBC. This service configures the SPI interface of the payload as the 'Host', and reads / writes data.<br>

The 'inc/' and 'src/' folders contain the majority of the Firmware code and are used in both services above.

## CM4 Config / Misc.

Within the folder 'misc_raspi' there are files used to configure the Raspberry Pi during boot 'config.txt' and additional bash scripts which help with file management and corruption protection. 

> [!WARNING]
> The location of these files within the Linux File System are important for their functionality. Care should be taken to validate the functionality of these files when placed in the system.

# Build C Firmware

The build process for the firmware utilizes a 'makefile' to automate the process. There are 4 possible build configurations available.

```
    make one_service
    make two_service
    make debug_one
    make debug_two
```

The Theia Service can either be a combined executable, or divided into two separate services. The two separate services allows the SPI Driver to be a separate service, while the single service combines the funcitonality.