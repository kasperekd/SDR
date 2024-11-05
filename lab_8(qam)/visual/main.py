import numpy as np
import matplotlib.pyplot as plt
import argparse

def read_qam_data(file_path):
    data = np.loadtxt(file_path)
    return data[:, 0], data[:, 1]

def main():
    parser = argparse.ArgumentParser(description='Visualize QAM symbols from a data file.')
    parser.add_argument('file_path', type=str, help='Path to the QAM data file')

    args = parser.parse_args()

    x, y = read_qam_data(args.file_path)

    plt.figure(figsize=(10, 6))
    plt.scatter(x, y, color='blue', marker='o')
    plt.title('QAM Symbols Visualization')
    plt.xlabel('In-Phase (I)')
    plt.ylabel('Quadrature (Q)')
    plt.grid()
    plt.axhline(0, color='black', linewidth=0.5, ls='--')
    plt.axvline(0, color='black', linewidth=0.5, ls='--')
    plt.axis('equal')
    plt.show()

if __name__ == '__main__':
    main()
