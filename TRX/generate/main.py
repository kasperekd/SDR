import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import firwin, lfilter

def text_to_bit_sequence(text):
    bit_sequence = []
    
    for char in text:
        ascii_value = ord(char)
        bits = format(ascii_value, '08b')
        bit_sequence.extend(int(bit) for bit in bits)
    
    bit_sequence_array = np.array(bit_sequence)
    return bit_sequence_array, len(bit_sequence_array)

def bits_to_qpsk(bits):
    symbols = []
    for i in range(0, len(bits), 2):
        if i + 1 < len(bits):
            if bits[i] == 0 and bits[i + 1] == 0:
                symbols.append(1 + 1j)  # 00
            elif bits[i] == 0 and bits[i + 1] == 1:
                symbols.append(-1 + 1j)  # 01
            elif bits[i] == 1 and bits[i + 1] == 0:
                symbols.append(-1 - 1j)  # 10
            elif bits[i] == 1 and bits[i + 1] == 1:
                symbols.append(1 - 1j)  # 11
    return np.array(symbols)

def oversampling(symbols, n):
    zeros = np.array([0 + 0j] * (n-1), dtype=np.complex128)
    new_symbols = []
    for symbol in symbols:
        new_symbols.append(symbol)
        new_symbols.extend(zeros)
    
    return np.array(new_symbols, dtype=np.complex128)

def convolve_with_one(symbols, n):
    # filter_ones = np.array([1 + 1j] * n, dtype=np.complex128)
    filter_ones = np.ones(n)
    # filter_ones = np.linspace(1, 0, n)
    result = np.convolve(symbols, filter_ones, mode='full')
    return result

N = 10
# num_bits = 20
# bit_sequence = np.random.randint(0, 2, num_bits)
text = "text!"
# text = "?? text to tx !!"
bit_sequence, num_bits = text_to_bit_sequence(text)

qpsk_symbols = bits_to_qpsk(bit_sequence)
qpsk_symbols_app = oversampling(qpsk_symbols, N)
qpsk_symbols_convolve = convolve_with_one(qpsk_symbols_app, N)

# qpsk_symbols_convolve *= 2**9

real_part = (qpsk_symbols_convolve.real).astype(np.int16)
imag_part = (qpsk_symbols_convolve.imag).astype(np.int16) 

combined = np.empty(real_part.size + imag_part.size, dtype=np.int16)
combined[0::2] = real_part
combined[1::2] = imag_part

print(combined)
print(real_part.size)

with open('qpsk_signal.bin', 'wb') as f:
    combined.tofile(f)

# print(qpsk_symbols_convolve)

# Визуализация
plt.figure(figsize=(10, 5))

# Действительная часть
plt.subplot(1, 2, 1)
plt.plot(qpsk_symbols_convolve.real, marker='o', linestyle='-', color='b')
plt.title('Действительная часть')
plt.xlabel('Индекс')
plt.ylabel('Значение')
plt.grid()

# Мнимая часть
plt.subplot(1, 2, 2)
plt.plot(qpsk_symbols_convolve.imag, marker='o', linestyle='-', color='r')
plt.title('Мнимая часть')
plt.xlabel('Индекс')
plt.ylabel('Значение')
plt.grid()



# plt.figure(figsize=(8, 8))
# plt.scatter(qpsk_symbols.real, qpsk_symbols.imag, color='blue', label='QPSK Symbols')
# plt.title('QPSK Constellation Diagram')
# plt.xlabel('In-Phase (I)')
# plt.ylabel('Quadrature (Q)')
# plt.grid()
# plt.axis('equal')
# plt.legend()
plt.show()
