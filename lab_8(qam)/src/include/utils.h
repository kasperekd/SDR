#ifndef UTILS_H
#define UTILS_H

#include <complex.h>
#include <stdio.h>

#include "qam.h"

void handle_error(const char *msg);
FILE *open_file(const char *filename, const char *mode);

// Запись массива символов QAM в файл
void write_qam_symbols_to_file(const complex float *symbols, int num_symbols,
                               const char *filename);

// Чтение символов QAM из файла в массив
int read_qam_symbols_from_file(const char *filename, complex float **symbols);

#endif  // UTILS_H
