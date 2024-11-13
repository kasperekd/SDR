import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import firwin, lfilter

# Параметры
N = 10  # oversampling factor
num_bits =200  # количество бит
bit_sequence = np.random.randint(0, 2, num_bits)  # случайная последовательность бит

# Преобразование битов в символы QPSK
def bits_to_qpsk(bits):
    symbols = []
    for i in range(0, len(bits), 2):
        if i + 1 < len(bits):
            # Формируем символы QPSK
            if bits[i] == 0 and bits[i + 1] == 0:
                symbols.append(1 + 1j)  # 00
            elif bits[i] == 0 and bits[i + 1] == 1:
                symbols.append(-1 + 1j)  # 01
            elif bits[i] == 1 and bits[i + 1] == 0:
                symbols.append(-1 - 1j)  # 10
            elif bits[i] == 1 and bits[i + 1] == 1:
                symbols.append(1 - 1j)  # 11
    return np.array(symbols)

# Генерация QPSK символов
qpsk_symbols = bits_to_qpsk(bit_sequence)

# Oversampling
oversampled_signal = np.repeat(qpsk_symbols, N)

# Согласованный фильтр (например, фильтр Ризера)
num_taps = 21  # количество коэффициентов фильтра
beta = 0.5  # параметр Ризера
taps = firwin(num_taps, 0.5, window='hamming')

# Применение фильтра
filtered_signal = lfilter(taps, 1.0, oversampled_signal)

# Преобразование в int16
int16_signal = np.int16(filtered_signal * 32767)  # нормализация и преобразование

# Сохранение в бинарный файл
# with open('qpsk_signal.txt', 'w') as f:
#     for i in range(0, len(int16_signal), 2):
#         if i + 1 < len(int16_signal):
#             f.write(f"{int16_signal[i]} {int16_signal[i + 1]}\n")
with open('qpsk_signal.bin', 'wb') as f:
    int16_signal.tofile(f)

# Визуализация созвездия
plt.figure(figsize=(8, 8))
plt.scatter(qpsk_symbols.real, qpsk_symbols.imag, color='blue', label='QPSK Symbols')
plt.title('QPSK Constellation Diagram')
plt.xlabel('In-Phase (I)')
plt.ylabel('Quadrature (Q)')
plt.grid()
plt.axis('equal')
plt.legend()
plt.show()
