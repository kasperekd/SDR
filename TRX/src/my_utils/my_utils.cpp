#include "my_utils.h"

void read_qpsk_signal(const char *filename, int16_t **tx_i, int16_t **tx_q,
                      size_t *num_samples) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Ошибка открытия файла");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size % (2 * sizeof(int16_t)) != 0) {
        fprintf(stderr,
                "Ошибка: размер файла не кратен размеру комплексного числа\n");
        fclose(file);
        return;
    }

    *num_samples = file_size / (2 * sizeof(int16_t));

    int16_t *combined = (int16_t *)malloc(file_size);
    if (combined == NULL) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        fclose(file);
        return;
    }

    size_t read_count =
        fread(combined, sizeof(int16_t), *num_samples * 2, file);
    if (read_count != *num_samples * 2) {
        fprintf(stderr, "Ошибка чтения данных из файла\n");
        free(combined);
        fclose(file);
        return;
    }

    fclose(file);

    *tx_i = (int16_t *)malloc(*num_samples * sizeof(int16_t));
    *tx_q = (int16_t *)malloc(*num_samples * sizeof(int16_t));
    if (*tx_i == NULL || *tx_q == NULL) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        free(combined);
        return;
    }
    for (size_t i = 0; i < *num_samples; i++) {
        (*tx_i)[i] = 0;
        (*tx_q)[i] = 0;
    }

    for (size_t i = 0; i < *num_samples; i++) {
        (*tx_i)[i] = combined[2 * i];
        (*tx_q)[i] = combined[2 * i + 1];
    }

    free(combined);
    // for (size_t i = 0; i < *num_samples; i++) {
    //     printf("Sample %zu: %d + %di\n", i, (*tx_i)[i], (*tx_q)[i]);
    // }
}

std::unordered_map<std::string, std::string> parse_args(int argc,
                                                        char *argv[]) {
    std::unordered_map<std::string, std::string> args;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg[0] == '-') {
            std::string key = arg.substr(1);
            std::string value;

            if ((i + 1) < argc && argv[i + 1][0] != '-') {
                value = argv[++i];
            }

            args[key] = value;
        } else {
            std::cerr << "Warning: Unexpected argument format: " << arg
                      << std::endl;
        }
    }

    return args;
}