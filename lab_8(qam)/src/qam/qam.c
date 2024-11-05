#include "qam.h"

#include <complex.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

const complex float QAM_4_CONSTELLATION[4] = {1 + 1 * I, 1 - 1 * I, -1 + 1 * I,
                                              -1 - 1 * I};
const complex float QAM_16_CONSTELLATION[16] = {
    3 + 3 * I,  3 + 1 * I,  1 + 3 * I,  1 + 1 * I, -1 + 3 * I, -1 + 1 * I,
    -3 + 3 * I, -3 + 1 * I, 3 - 3 * I,  3 - 1 * I, 1 - 3 * I,  1 - 1 * I,
    -1 - 3 * I, -1 - 1 * I, -3 - 3 * I, -3 - 1 * I};

const complex float *get_constellation(QAMLevel level, int *size) {
    switch (level) {
        case QAM_4:
            *size = 4;
            return QAM_4_CONSTELLATION;
        case QAM_16:
            *size = 16;
            return QAM_16_CONSTELLATION;
        default:
            *size = 0;
            return NULL;
    }
}

void qam_modulate(const char *message, const QAMConfig *config,
                  const char *output_file) {
    int constellation_size;
    const complex float *constellation =
        get_constellation(config->level, &constellation_size);
    if (!constellation) {
        fprintf(stderr, "Unsupported QAM level.\n");
        return;
    }

    int message_length = strlen(message);
    complex float *symbols = malloc(
        message_length * config->samples_per_symbol * sizeof(complex float));
    if (!symbols) {
        handle_error("Memory allocation error");
    }

    int symbol_index = 0;
    for (int i = 0; i < message_length; i++) {
        uint8_t symbol = message[i] % constellation_size;
        complex float qam_symbol = constellation[symbol];

        for (int t = 0; t < config->samples_per_symbol; t++) {
            double time = (double)t / config->samples_per_symbol;
            symbols[symbol_index++] =
                creal(qam_symbol) *
                    cos(2 * M_PI * config->carrier_freq * time) +
                cimag(qam_symbol) *
                    sin(2 * M_PI * config->carrier_freq * time) * I;
        }
    }

    write_qam_symbols_to_file(symbols, symbol_index, output_file);
    free(symbols);
}

void qam_demodulate(const char *input_file, const QAMConfig *config,
                    const char *output_file) {
    int constellation_size;
    const complex float *constellation =
        get_constellation(config->level, &constellation_size);
    if (!constellation) {
        fprintf(stderr, "Unsupported QAM level.\n");
        return;
    }

    complex float *symbols;
    int num_symbols = read_qam_symbols_from_file(input_file, &symbols);
    FILE *output = open_file(output_file, "wb");

    for (int i = 0; i < num_symbols; i++) {
        int closest_symbol = 0;
        double min_distance = cabs(symbols[i] - constellation[0]);

        for (int j = 1; j < constellation_size; j++) {
            double distance = cabs(symbols[i] - constellation[j]);
            if (distance < min_distance) {
                min_distance = distance;
                closest_symbol = j;
            }
        }

        uint8_t decoded_symbol = (uint8_t)closest_symbol;
        fwrite(&decoded_symbol, sizeof(decoded_symbol), 1, output);
    }

    fclose(output);
    free(symbols);
}
