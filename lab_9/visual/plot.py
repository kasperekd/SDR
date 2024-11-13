import argparse
import matplotlib.pyplot as plt

def read_complex_data(file_path, max_lines=None):
    real_part = []
    imag_part = []
    with open(file_path, 'r') as file:
        for i, line in enumerate(file):
            if max_lines is not None and i >= max_lines:
                break
            coords = line.strip().split(',')
            if len(coords) == 2:
                real_part.append(float(coords[0].strip()))
                imag_part.append(float(coords[1].strip()))
    return real_part, imag_part

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

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('file_path', type=str)
    parser.add_argument('--max_lines', type=int, default=None)
    args = parser.parse_args()

    real_part, imag_part = read_complex_data(args.file_path, args.max_lines)

    plot_signal(real_part, imag_part)

if __name__ == '__main__':
    main()
