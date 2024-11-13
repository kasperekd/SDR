import numpy as np
import matplotlib.pyplot as plt

def read_signal(filename):
    signal = []
    with open(filename, 'r') as file:
        for line in file:
            parts = line.strip().split(',')
            signal.append([int(parts[0]), int(parts[1])])
    return np.array(signal)

def low_pass_filter(data, kernel_size):
    kernel = np.ones(kernel_size) / kernel_size
    return np.convolve(data, kernel, mode='same')

configurations = [
    {'kernel_size': 3, 'threshold': 650, 'remove_dc': True, 'title': 'Kernel Size: 3, Threshold: 650, DC Removed'},
    {'kernel_size': 3, 'threshold': 650, 'remove_dc': False, 'title': 'Kernel Size: 3, Threshold: 650, DC Not Removed'},
    {'kernel_size': 5, 'threshold': 500, 'remove_dc': True, 'title': 'Kernel Size: 5, Threshold: 500, DC Removed'},
    {'kernel_size': 5, 'threshold': 500, 'remove_dc': False, 'title': 'Kernel Size: 5, Threshold: 500, DC Not Removed'},
    {'kernel_size': 7, 'threshold': 300, 'remove_dc': True, 'title': 'Kernel Size: 7, Threshold: 300, DC Removed'},
    {'kernel_size': 7, 'threshold': 300, 'remove_dc': False, 'title': 'Kernel Size: 7, Threshold: 300, DC Not Removed'},
]

# read
signal = read_signal('single_adalm_rx.txt')
# signal = read_signal('test(1).txt')
# signal = read_signal('qpsk_signal.txt')
# signal = read_signal('single_adalm_rx.txt')

I = signal[:, 0]
Q = signal[:, 1]

for config in configurations:
    kernel_size = config['kernel_size']
    threshold = config['threshold']
    remove_dc = config['remove_dc']

    I_filtered = low_pass_filter(I, kernel_size)
    Q_filtered = low_pass_filter(Q, kernel_size)

    if remove_dc:
        I_detrended = I_filtered - np.mean(I_filtered)
        Q_detrended = Q_filtered - np.mean(Q_filtered)
    else:
        I_detrended = I_filtered
        Q_detrended = Q_filtered

    mask = np.sqrt(I_detrended**2 + Q_detrended**2) > threshold
    I_cleaned = I_detrended[mask]
    Q_cleaned = Q_detrended[mask]

    plt.figure()
    plt.scatter(I_cleaned, Q_cleaned)
    plt.title(config['title'])
    plt.xlabel('I')
    plt.ylabel('Q')
    plt.grid()
    plt.axis('equal')
    plt.show()
