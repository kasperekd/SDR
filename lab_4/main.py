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
    b_n, _ = quad(lambda t: rectangular_signal(t) * np.sin(2 * np.pi * n * t / T), 0, T)
    return a_n / T, b_n / T

def reconstructed_signal(t, N, a_coeffs, b_coeffs):
    result = a_coeffs[0] / 4
    for n in range(1, N):
        result += a_coeffs[n] * np.cos(2 * np.pi * n * t / T) + b_coeffs[n] * np.sin(2 * np.pi * n * t / T)
    return result * 2

# Основной код
N = 25
a_coeffs = [2 * tau / T]  # a_0
b_coeffs = [0]  # b_0

for n in range(1, N):
    a, b = fourier_coefficients(n)
    a_coeffs.append(a)
    b_coeffs.append(b)

A_n = [np.sqrt(a**2 + b**2) for a, b in zip(a_coeffs, b_coeffs)]
phi_n = [np.arctan2(b, a) for a, b in zip(a_coeffs, b_coeffs)]

# Вывод коэффициентов
for n in range(N):
    print(f"a_{n} = {a_coeffs[n]:.4f}, b_{n} = {b_coeffs[n]:.4f}, A_{n} = {A_n[n]:.4f}, φ_{n} = {phi_n[n]:.4f}")

# Построение спектров
frequencies = np.arange(N)
plt.figure(figsize=(12, 6))

plt.subplot(2, 1, 1)
plt.stem(frequencies, A_n)
plt.title('Спектр амплитуд')
plt.xlabel('Гармоника n')
plt.ylabel('A_n')
plt.grid()

plt.subplot(2, 1, 2)
plt.stem(frequencies, phi_n)
plt.title('Спектр фаз')
plt.xlabel('Гармоника n')
plt.ylabel('φ_n (рад)')
plt.grid()

plt.tight_layout()
plt.show()

# Синтез временного колебания
t = np.linspace(0, 4 * T, 1000)
original_signal = [rectangular_signal(ti) for ti in t]
reconstructed_signals = [reconstructed_signal(t, n, a_coeffs, b_coeffs) for n in [3, 5, 7]]

plt.figure(figsize=(12, 6))
plt.plot(t, original_signal, label='Оригинальный сигнал', linestyle='--')
for n, rec_signal in zip([2, 4, 6,], reconstructed_signals):
    plt.plot(t, rec_signal, label=f'Восстановленный сигнал ({n} коэффициента)', alpha=0.7)
plt.title('Сравнение оригинального и восстановленных сигналов')
plt.xlabel('Время (s)')
plt.ylabel('Амплитуда')
plt.ylim(-0.5, 1.5)
plt.grid()
plt.legend()
plt.show()

# Проверка ортогональности
def check_orthogonality(k, n):
    return quad(lambda t: np.sin(2 * np.pi * k * t / T) * np.sin(2 * np.pi * n * t / T), 0, T)[0]

for k in range(1, 11):
    for n in range(1, 11):
        if k != n:
            print(f"Интеграл от s_{k}(t) и s_{n}(t): {check_orthogonality(k, n):.4f} (k={k}, n={n})")

# Изменение частоты и интервала интегрирования
f1, f2 = 1 / T, 2 / T
s1, s2 = lambda t: np.sin(2 * np.pi * f1 * t), lambda t: np.sin(2 * np.pi * f2 * t)

integral_value = quad(lambda t: s1(t) * s2    (t), 0, T)[0]
print(f"Интеграл от s1(t) и s2(t) с измененной частотой: {integral_value:.4f}")

# Изменение интервала интегрирования
integral_value_half = quad(lambda t: s1(t) * s2(t), 0, T/2)[0]
print(f"Интеграл от s1(t) и s2(t) на интервале [0, T/2]: {integral_value_half:.4f}")

# Формирование комплексного сигнала
def complex_signal(n, t):
    return np.exp(1j * 2 * np.pi * n * f1 * t)

# Проверка ортогональности для комплексных сигналов
for k in range(1, 11):
    for n in range(1, 11):
        if k != n:
            integral_value = quad(lambda t: s1(t) * complex_signal(n, t), 0, T)[0]
            print(f"Интеграл от s1(t) и e^(j*2πnt): {integral_value:.4f} (k={k}, n={n})")
