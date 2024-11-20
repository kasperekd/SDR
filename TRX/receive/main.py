import argparse
import numpy as np
import matplotlib.pyplot as plt

from matplotlib.widgets import Slider

def plot_signal_interactive(signal, N):
    fig_real_imag, (ax_real, ax_imag) = plt.subplots(2, 1, figsize=(12, 8))
    plt.subplots_adjust(left=0.1, bottom=0.3)

    real_line, = ax_real.plot([], [], color='blue', label='Real Part')
    imag_line, = ax_imag.plot([], [], color='red', label='Imaginary Part')

    for ax in [ax_real, ax_imag]:
        ax.grid(True)
        ax.axhline(0, color='black', linewidth=0.5, linestyle='--')
        ax.legend()

    ax_real.set_title('Real Part')
    ax_real.set_xlabel('Index')
    ax_real.set_ylabel('Amplitude')

    ax_imag.set_title('Imaginary Part')
    ax_imag.set_xlabel('Index')
    ax_imag.set_ylabel('Amplitude')

    fig_scatter, ax_scatter = plt.subplots(figsize=(8, 8))
    scatter = ax_scatter.scatter([], [], s=5)
    ax_scatter.set_title('QPSK Constellation')
    ax_scatter.set_xlabel('I')
    ax_scatter.set_ylabel('Q')
    ax_scatter.grid(True)
    ax_scatter.axis('equal')

    def update_graph(offset):
        offset = int(offset)
        extracted_signal = extract_every_nth(signal, N, offset)
        real_part = extracted_signal.real
        imag_part = extracted_signal.imag

        real_line.set_data(range(len(real_part)), real_part)
        imag_line.set_data(range(len(imag_part)), imag_part)

        scatter.set_offsets(np.c_[real_part, imag_part])

        ax_real.set_xlim(0, len(real_part))
        ax_real.set_ylim(min(real_part), max(real_part))

        ax_imag.set_xlim(0, len(imag_part))
        ax_imag.set_ylim(min(imag_part), max(imag_part))

        ax_scatter.set_xlim(min(real_part), max(real_part))
        ax_scatter.set_ylim(min(imag_part), max(imag_part))

        fig_real_imag.canvas.draw_idle()
        fig_scatter.canvas.draw_idle()

    ax_slider = plt.axes([0.2, 0.1, 0.65, 0.03])
    slider = Slider(ax_slider, 'Offset', 0, N-1, valinit=0, valstep=1)

    slider.on_changed(update_graph)

    update_graph(0)

    plt.show()

def plot_signal(real_part, imag_part):
    time_indices = range(len(real_part))

    plt.figure(figsize=(12, 6))

    # real
    plt.subplot(2, 1, 1)
    plt.plot(time_indices, real_part, color='blue', label='Real Part')
    # plt.plot(time_indices, imag_part, color='red', label='Imaginary Part')
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
    plt.scatter(real_part, imag_part, s=5)
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
# r = read_signal(args.file_path)
# signal = r[1:300]
# signal /= 2**9
# plot_signal(signal.real, signal.imag)
filter_ones = np.ones(N)
signal = convolve_with_filter(signal, filter_ones)


# beta = 0.75
# num_taps = 128
# filter_coefficients = raised_cosine_filter(beta, N, num_taps)
# signal = convolve_with_filter(signal, filter_coefficients)

# signal = extract_every_nth(signal, N, 3) # нужно менять этот 0 в пределах от 0 до N
# plot_signal(signal.real, signal.imag)
plot_signal_interactive(signal, N)
