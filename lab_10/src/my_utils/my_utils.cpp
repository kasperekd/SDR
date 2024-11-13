#include "my_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void read_qpsk_signal(const char *filename, int16_t **tx_i, int16_t **tx_q, size_t *num_samples) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Ошибка открытия файла");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size % (2 * sizeof(int16_t)) != 0) {
        fprintf(stderr, "Ошибка: размер файла не кратен размеру комплексного числа\n");
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

    size_t read_count = fread(combined, sizeof(int16_t), *num_samples * 2, file);
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
        (*tx_i)[i] = combined[2 * i];
        (*tx_q)[i] = combined[2 * i + 1];
    }

    free(combined);
    for (size_t i = 0; i < *num_samples; i++) {
        printf("Sample %zu: %d + %di\n", i, (*tx_i)[i], (*tx_q)[i]);
    }
}
// void read_qpsk_signal(const char *filename, int16_t **tx_i, int16_t **tx_q, size_t *num_samples) {
//     FILE *file = fopen(filename, "rb");
//     if (!file) {
//         perror("Ошибка открытия файла");
//         exit(EXIT_FAILURE);
//     }

//     fseek(file, 0, SEEK_END);
//     long file_size = ftell(file);
//     fseek(file, 0, SEEK_SET);

//     if (file_size % (2 * sizeof(int16_t)) != 0) {
//         fprintf(stderr, "Неверный размер файла\n");
//         fclose(file);
//         exit(EXIT_FAILURE);
//     }

//     *num_samples = file_size / (2 * sizeof(int16_t));

//     *tx_i = (int16_t *)malloc(*num_samples * sizeof(int16_t));
//     *tx_q = (int16_t *)malloc(*num_samples * sizeof(int16_t));
//     if (*tx_i == NULL || *tx_q == NULL) {
//         perror("Ошибка выделения памяти");
//         fclose(file);
//         exit(EXIT_FAILURE);
//     }

//     size_t read_count = fread(*tx_i, sizeof(int16_t), *num_samples, file);
//     if (read_count != *num_samples) {
//         fprintf(stderr, "Ошибка чтения данных из файла\n");
//         free(*tx_i);
//         free(*tx_q);
//         fclose(file);
//         exit(EXIT_FAILURE);
//     }

//     read_count = fread(*tx_q, sizeof(int16_t), *num_samples, file);
//     if (read_count != *num_samples) {
//         fprintf(stderr, "Ошибка чтения данных из файла\n");
//         free(*tx_i);
//         free(*tx_q);
//         fclose(file);
//         exit(EXIT_FAILURE);
//     }

//     fclose(file);

//     for (size_t i = 0; i < *num_samples; i++)
//     {
//         std::cout << *tx_i[i] << "\t" << *tx_q[i] << "\n";
//     }
    
// }
// void read_qpsk_signal(const char *filename, int16_t **tx_i, int16_t **tx_q, size_t *num_samples) {
//     // Открываем бинарный файл для чтения
//     FILE *file = fopen(filename, "rb");
//     if (!file) {
//         perror("Ошибка открытия файла");
//         exit(EXIT_FAILURE);
//     }

//     // Определяем размер файла
//     fseek(file, 0, SEEK_END);
//     long file_size = ftell(file);
//     fseek(file, 0, SEEK_SET);

//     // Проверяем, что размер файла четный
//     if (file_size % (2 * sizeof(int16_t)) != 0) {
//         fprintf(stderr, "Неверный размер файла\n");
//         fclose(file);
//         exit(EXIT_FAILURE);
//     }

//     // Вычисляем количество пар (I, Q)
//     *num_samples = file_size / (2 * sizeof(int16_t));

//     *tx_i = (int16_t *)malloc(*num_samples * sizeof(int16_t));
//     *tx_q = (int16_t *)malloc(*num_samples * sizeof(int16_t));
//     if (*tx_i == NULL || *tx_q == NULL) {
//         perror("Ошибка выделения памяти");
//         fclose(file);
//         exit(EXIT_FAILURE);
//     }

//     for (size_t i = 0; i < *num_samples; i++) {
//         fread(&(*tx_i)[i], sizeof(int16_t), 1, file);
//         fread(&(*tx_q)[i], sizeof(int16_t), 1, file);
//     }

//     fclose(file);
// }