#ifndef CMD_CTRL_H
#define CMD_CTRL_H

#include <stdint.h>

int cmd_extracter(char *cmd, char arg[][100], uint8_t *rx_buffer, uint8_t rx_len);
void cmd_center(int nargs, char *argv[]);

#endif //CMD_CTRL