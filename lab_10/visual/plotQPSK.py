import numpy as np
import matplotlib.pyplot as plt

def plot_signal(real_part, imag_part):
    time_indices = range(len(real_part))

    plt.figure(figsize=(12, 6))

    # real
    plt.subplot(2, 1, 1)
    plt.plot(time_indices, real_part, color='blue', label='Real Part')
    plt.title('Real Part')
    plt.xlabel('Index')
    plt.ylabel('Amplitude')
    plt.grid(True)
    plt.axhline(0, color='black', linewidth=0.5, ls='--')
    plt.legend()

    # img
    plt.subplot(2, 1, 2)
    plt.plot(time_indices, imag_part, color='red', label='Imaginary Part')
    plt.title('Imaginary Part')
    plt.xlabel('Index')
    plt.ylabel('Amplitude')
    plt.grid(True)
    plt.axhline(0, color='black', linewidth=0.5, ls='--')
    plt.legend()

    plt.tight_layout()
    plt.show()


def read_signal(filename):
    signal = []
    with open(filename, 'r') as file:
        for line in file:
            parts = line.strip().split(',')
            signal.append([int(parts[0]), int(parts[1])])
    return np.array(signal)

def low_pass_filter(data, kernel_size):
    kernel = np.ones(kernel_size)
    return np.convolve(data, kernel, mode='same')

configurations = [
    {'kernel_size': 10, 'threshold': 0, 'remove_dc': True, 'title': 'Kernel Size: 10, Threshold: 0, DC Removed'},
]

# read
signal = read_signal('4ghz_10mhz.txt')
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

    plot_signal(I, Q)
    plot_signal(I_filtered, Q_filtered)

    if remove_dc:
        I_detrended = I_filtered - np.mean(I_filtered)
        Q_detrended = Q_filtered - np.mean(Q_filtered)
    else:
        I_detrended = I_filtered
        Q_detrended = Q_filtered

    mask = np.sqrt(I_detrended**2 + Q_detrended**2) > threshold
    I_cleaned = I_detrended[mask]
    Q_cleaned = Q_detrended[mask]

    plot_signal(I_cleaned, Q_cleaned)

    plt.figure()
    plt.scatter(I_cleaned, Q_cleaned)
    plt.title(config['title'])
    plt.xlabel('I')
    plt.ylabel('Q')
    plt.grid()
    plt.axis('equal')
    plt.show()
