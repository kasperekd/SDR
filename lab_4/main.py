import numpy as np
import matplotlib.pyplot as plt
from scipy.integrate import quad

# Параметры сигнала
T = 2.0  # Период
tau = 1.0  # Длительность прямоугольного сигнала

def rectangular_signal(t):
    return 1 if (t % T) < tau else 0

def fourier_coefficients(n):
    a_n, _ = quad(lambda t: rectangular_signal(t) * np.cos(2 * np.pi * n * t / T), 0, T)
    a_n /= T
    
    b_n, _ = quad(lambda t: rectangular_signal(t) * np.sin(2 * np.pi * n * t / T), 0, T)
    b_n /= T
    
    return a_n, b_n

def reconstructed_signal(t, N, a_coeffs, b_coeffs):
    result = a_coeffs[0] / 4
    for n in range(1, N):
        result += a_coeffs[n] * np.cos(2 * np.pi * n * t / T) + b_coeffs[n] * np.sin(2 * np.pi * n * t / T)
    return result * 2

# main
N = 25
a_coeffs = []
b_coeffs = []

a_0 = (2 * tau) / T
b_0 = 0
a_coeffs.append(a_0)
b_coeffs.append(b_0)

for n in range(1, N):
    a, b = fourier_coefficients(n)
    a_coeffs.append(a)
    b_coeffs.append(b)

for n in range(N):
    print(f"a_{n} = {a_coeffs[n]:.4f}, b_{n} = {b_coeffs[n]:.4f}")

t = np.linspace(0, 4 * T, 1000)
original_signal = [rectangular_signal(ti) for ti in t]
reconstructed = reconstructed_signal(t, N, a_coeffs, b_coeffs)

plt.figure(figsize=(12, 6))
plt.plot(t, original_signal, label='Оригинальный сигнал', linestyle='--')
plt.plot(t, reconstructed, label='Восстановленный сигнал', alpha=0.7)
plt.title('Сравнение оригинального и восстановленного сигналов')
plt.xlabel('Время (s)')
plt.ylabel('Амплитуда')
plt.ylim(-0.5, 1.5)
plt.grid()
plt.legend()
plt.show()
