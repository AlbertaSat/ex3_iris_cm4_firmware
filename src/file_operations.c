#include "error_handler.h"
#include "file_operations.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>


enum IRIS_ERROR sha256_checksum(const char *filename, uint8_t *checksum) {

    unsigned char buffer[1024];
    SHA256_CTX sha256;

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("File open error");
        return;
    }

    SHA256_Init(&sha256);
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file))) {
        SHA256_Update(&sha256, buffer, bytesRead);
    }
    SHA256_Final(checksum, &sha256);
    fclose(file);

    printf("SHA-256 checksum: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", checksum[i]);
    }
    printf("\n");
}