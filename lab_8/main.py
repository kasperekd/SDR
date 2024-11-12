# import numpy as np
# import matplotlib.pyplot as plt

# def generate_qpsk_symbols(num_symbols):
#     # Генерация случайных битов
#     bits = np.random.randint(0, 2, num_symbols * 2)
    
#     # Преобразование битов в символы QPSK
#     symbols = []
#     for i in range(0, len(bits), 2):
#         if bits[i] == 0 and bits[i+1] == 0:
#             symbols.append(1 + 1j)  # 00
#         elif bits[i] == 0 and bits[i+1] == 1:
#             symbols.append(-1 + 1j)  # 01
#         elif bits[i] == 1 and bits[i+1] == 1:
#             symbols.append(-1 - 1j)  # 11
#         elif bits[i] == 1 and bits[i+1] == 0:
#             symbols.append(1 - 1j)  # 10

#     return np.array(symbols)

# # Параметры
# num_symbols = 10  # Количество символов QPSK

# # Генерация символов
# qpsk_symbols = generate_qpsk_symbols(num_symbols)

# # Вывод символов
# print("Сгенерированные символы QPSK:")
# for symbol in qpsk_symbols:
#     print(symbol)

# # Визуализация символов
# plt.figure(figsize=(6, 6))
# plt.scatter(qpsk_symbols.real, qpsk_symbols.imag)
# plt.title('QPSK Symbols')
# plt.xlabel('In-Phase')
# plt.ylabel('Quadrature')
# plt.xlim(-2, 2)
# plt.ylim(-2, 2)
# plt.grid()
# plt.axhline(0, color='black', lw=0.5)
# plt.axvline(0, color='black', lw=0.5)
# plt.show()


import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import max_len_seq

# Параметры
fs = 1000000  # Частота дискретизации
rs = 100000   # Скорость передачи
ns = fs // rs

# Генерация данных
data = max_len_seq(8)[0] 
data = np.concatenate((data, np.zeros(1)))

# Формирование символов
m = 2 * data - 1

# QPSK
x = np.reshape(m, (2, 128))
xi = x[0, :]
xq = x[1, :]
x_bb = (xi + 1j * xq) / np.sqrt(2)

# Вывод графика символов
plt.figure(1)
plt.scatter(x_bb.real, x_bb.imag)
plt.title('QPSK Symbols')
plt.xlabel('In-Phase')
plt.ylabel('Quadrature')
plt.grid()
plt.axis('equal')
plt.show()