import numpy as np
import matplotlib.pyplot as plt

# y_| Q
# 1 | -1
# 0 | -1
# -1 | 0
# 1 | 0
# -1 |1

# f_c = 10**3 несущая
# T_s = от 0 до 10**-3 длин символа
# T_sp = от 0 до 5*10**-3  длина пяти символов

# Нужно получить 5 сигналов полученные из 
# С помощью convolve нужно сначала создать сигнал из y_ и Q на T_s а потом соединить все пять на T_sp

f_c = 10**3
T_s = np.linspace(0, 10**-3, 1000)
T_sp = np.linspace(0,10**-3, 1000*5)

I = [1, 0, -1, 1, -1]
Q = [-1, -1, 0, 0, 1]

X_t = []

for i in range(5):
    S_cos = I[i] * np.cos(2 * np.pi * f_c * T_s) 
    S_sin = Q[i] * np.sin(2 * np.pi * f_c * T_s) 

    X_t_ = S_cos - S_sin
    X_t.append(X_t_)

result = np.concatenate(X_t)
T_long = np.linspace(0, 10**-3 * len(X_t), len(result))

print(len(result))
print(len(T_sp), len(T_long))

plt.figure(figsize=(12, 8))
for i in range(5):
    plt.subplot(5, 1, i + 1)
    plt.plot(T_s, X_t[i])
    plt.title(f'Signal {i + 1}')
    plt.xlabel('Time (s)')
    plt.ylabel('Amplitude')
    plt.grid()

plt.tight_layout()

plt.figure()
plt.plot(T_sp,result)
plt.xlabel('Time (s)')
plt.ylabel('Amplitude')
plt.grid()
plt.show()
