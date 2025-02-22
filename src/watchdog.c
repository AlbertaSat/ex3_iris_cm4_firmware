// #include "watchdog.h"
// #include "logger.h"

// #include <sys/ioctl.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <errno.h>
// #include <stdio.h>

// #include <linux/watchdog.h>

// //! NEED TO FIX PERMISSIONS SO THE CODE CAN OPEN WATCHDOG FILE

// void setup_watchdog(void){

//     int fd;
//     int timeout = WATCHDOGTIMEOUT_S;
//     int timeout_rd = 0;

//     log_write(LOG_INFO, "SETUP-WATCHDOG: Starting to setup Watchdog");

//     // Open the watchdog device
//     fd = open(WATCHDOG, O_RDWR);
//     if (fd == -1) {
//         perror("Error opening /dev/watchdog");
//         return;
//     }

//     // Set the timeout for the watchdog
//     if (ioctl(fd, WDIOC_SETTIMEOUT, &timeout) == -1) {
//         perror("Error setting timeout");
//         close(fd);
//         return;
//     }

//     log_write(LOG_INFO, "SETUP-WATCHDOG: Configured Watchdog timeout value");

//     // Get the timeout to confirm
//     if (ioctl(fd, WDIOC_GETTIMEOUT, &timeout_rd) == -1) {
//         perror("Error getting timeout");
//         close(fd);
//         return;
//     }

//     if (timeout_rd != timeout){
//         log_write(LOG_ERROR, "SETUP-WATCHDOG: Watchdog Timeout value not set correctly");
//         close(fd);
//         return;
//     }

//     log_write(LOG_INFO, "SETUP-WATCHDOG: Successfully setup Watchdog");

//     // Close the device
//     close(fd);
//     return;
// }


// void kick_watchdog(void){

//     int fd;

//     log_write(LOG_INFO, "KICK-WATCHDOG: Starting to kick Watchdog");

//     // Open the watchdog device
//     fd = open(WATCHDOG, O_RDWR);
//     if (fd == -1) {
//         perror("Error opening /dev/watchdog");
//         return;
//     }

//     // Write to the watchdog to "kick" it
//     if (write(fd, "\0", 1) == -1) {
//         perror("Error writing to /dev/watchdog");
//         close(fd);
//         return;
//     }
    
//     log_write(LOG_INFO, "KICK-WATCHDOG: Watchdog was kicked");

//     // Close the device
//     close(fd);
//     return;
// }

// void disable_watchdog(void){

//     int fd;

//     log_write(LOG_INFO, "DISABLE-WATCHDOG: Starting to disable Watchdog");

//     // Open the watchdog device
//     fd = open(WATCHDOG, O_RDWR);
//     if (fd == -1) {
//         perror("Error opening /dev/watchdog");
//         return;
//     }

//     // If you want to disable the watchdog before exiting:
//     // (This only works if the watchdog daemon allows it)
//     if (ioctl(fd, WDIOC_SETOPTIONS, WDIOS_DISABLECARD) == -1) {
//         perror("Error disabling watchdog");
//     }

//     log_write(LOG_INFO, "DISABLE-WATCHDOG: Successfully disabled Watchdog");
    
//     // Close the device
//     close(fd);
//     return;
// }