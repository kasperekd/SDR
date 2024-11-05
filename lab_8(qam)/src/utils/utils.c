#include "utils.h"

#include <errno.h>
#include <stdlib.h>

void handle_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

FILE *open_file(const char *filename, const char *mode) {
    FILE *file = fopen(filename, mode);
    if (!file) {
        handle_error("Error opening file");
    }
    return file;
}

void write_qam_symbols_to_file(const complex float *symbols, int num_symbols,
                               const char *filename) {
    FILE *file = open_file(filename, "wb");
    if (fwrite(symbols, sizeof(complex float), num_symbols, file) !=
        num_symbols) {
        handle_error("Error writing QAM symbols to file");
    }
    fclose(file);
}

int read_qam_symbols_from_file(const char *filename, complex float **symbols) {
    FILE *file = open_file(filename, "rb");
    fseek(file, 0, SEEK_END);
    int num_symbols = ftell(file) / sizeof(complex float);
    rewind(file);

    *symbols = malloc(num_symbols * sizeof(complex float));
    if (!*symbols) {
        handle_error("Memory allocation error");
    }

    if (fread(*symbols, sizeof(complex float), num_symbols, file) !=
        num_symbols) {
        handle_error("Error reading QAM symbols from file");
    }
    fclose(file);
    return num_symbols;
}
