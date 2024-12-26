import numpy as np
import matplotlib.pyplot as plt

f_m = 300
f_c = 10 * f_m
t = np.arange(0, 0.025, 1/(2 * f_c))

tau = 4 * np.cos(2 * np.pi * f_m * t)
Q = np.sin(2 * np.pi * f_m * t)

s_t = tau * np.cos(2 * np.pi * f_c * t) - Q * np.sin(2 * np.pi * f_c * t)

# z = np.arctan2(Q/tau)
z = np.arctan(Q/tau)

mod_sqrt = (tau**2 + Q**2)**0.5

plt.figure(figsize=(10, 5))

plt.subplot(2, 1, 1)
plt.plot(t, s_t, label='s(t)')
plt.plot(t, z, label='z(t)')
plt.title('Колебания s(t)')
plt.xlabel('Время (с)')
plt.ylabel('Амплитуда')
plt.grid()
plt.legend()

plt.subplot(2, 1, 2)
plt.plot(t, s_t, label='s(t)')
plt.plot(t, mod_sqrt, label='|s(t)|')
plt.plot(t, -mod_sqrt, label='-|s(t)|')
plt.title('Колебания s(t)')
plt.xlabel('Время (с)')
plt.ylabel('Амплитуда')
plt.grid()
plt.legend()

plt.tight_layout()
plt.show()
