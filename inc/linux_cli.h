#ifndef LINUX_CLI_H
#define LINUX_CLI_H

#define LINUX_CLI_OUT_BUFFER_LEN 32768

int test_cli();
enum IRIS_ERROR exec_linux_cli_cmd(char* input_cmd, char* output_response);

#endif //LINUX_CLI_H