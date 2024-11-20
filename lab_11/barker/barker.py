import numpy as np
import matplotlib.pyplot as plt


b7 = np.array([1,1,1,-1,-1,-1,1,-1,-1,1,-1])

def autocorrelation(x):
    n = len(x)
    mean = np.mean(x)
    c0 = np.sum((x - mean) ** 2)
    return [np.sum((x[:n-k] - mean) * (x[k:] - mean)) / c0 for k in range(n)]

autocorr = autocorrelation(b7)

plt.plot(autocorr)
plt.title('Автокорреляция')
plt.xlabel('Лаг')
plt.ylabel('Автокорреляция')
plt.grid()
plt.show()
