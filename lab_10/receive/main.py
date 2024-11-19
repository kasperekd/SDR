import argparse
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

    # QPSK
    plt.figure()
    plt.scatter(real_part, imag_part)
    plt.xlabel('I')
    plt.ylabel('Q')
    plt.grid()
    plt.axis('equal')

    plt.tight_layout()
    plt.show()


def read_signal(filename):
    signal = []
    with open(filename, 'r') as file:
        for line in file:
            parts = line.strip().split(',')
            # signal.append([np.int16(parts[0]), np.int16(parts[1])])
            signal.append(np.complex128(np.int16(parts[0]) + 1j * np.int16(parts[1])))
    return np.array(signal)

def raised_cosine_filter(beta, sps, num_taps):
    t = np.arange(-num_taps / 2, num_taps / 2 + 1) / sps
    h = np.zeros_like(t)

    for i in range(len(t)):
        if t[i] == 0:
            h[i] = 1 - beta + (4 * beta / np.pi)
        elif abs(t[i]) == 1 / (4 * beta):
            h[i] = (beta / np.sqrt(2)) * ((1 + np.cos(np.pi * (4 * beta))) / 2)
        else:
            h[i] = (np.sin(np.pi * t[i] * (1 - beta)) + (4 * beta * t[i]) * np.cos(np.pi * t[i] * (1 + beta))) / (np.pi * t[i] * (1 - (4 * beta * t[i]) ** 2))

    h /= np.sqrt(np.sum(h ** 2))
    return h

def convolve_with_filter(symbols, filter_coefficients):
    result = np.convolve(symbols, filter_coefficients, mode='full')
    return result

def extract_every_nth(signal, n, offset=0):
    return signal[offset::n]

# read
parser = argparse.ArgumentParser()
parser.add_argument('file_path', type=str)
parser.add_argument('--max_lines', type=int, default=None)
args = parser.parse_args()

N = 10
signal = read_signal(args.file_path)

signal /= 2**9

filter_ones = np.ones(N)
signal = convolve_with_filter(signal, filter_ones)

# beta = 0.25
# num_taps = 64
# filter_coefficients = raised_cosine_filter(beta, N, num_taps)
# signal = convolve_with_filter(signal, filter_coefficients)

signal = extract_every_nth(signal, N, 0)

plot_signal(signal.real, signal.imag)
