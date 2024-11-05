#ifndef QAM_H
#define QAM_H

#include <complex.h>
#include <stdint.h>
#include <string.h>

#define M_PI 3.14159265358979323846

// Определение уровней QAM
typedef enum {
    QAM_4 = 4,
    QAM_8 = 8,
    QAM_16 = 16,
    QAM_32 = 32,
    QAM_64 = 64
} QAMLevel;

// Структура для хранения конфигурации QAM
typedef struct {
    QAMLevel level;       // Уровень QAM
    double carrier_freq;  // Частота несущей
    int samples_per_symbol;  // Количество сэмплов на символ
    int is_binary;  // Указатель на текстовый (0) или бинарный (1) режим данных
} QAMConfig;

// Функции для модуляции и демодуляции
void qam_modulate(const char *message, const QAMConfig *config,
                  const char *output_file);
void qam_demodulate(const char *input_file, const QAMConfig *config,
                    const char *output_file);

#endif  // QAM_H
