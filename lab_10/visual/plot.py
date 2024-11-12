# import argparse
# import matplotlib.pyplot as plt
# import numpy as np

# def read_data(file_path, max_lines=None):
#     """Читает данные из текстового файла и возвращает списки x и y координат."""
#     x = []
#     y = []
#     with open(file_path, 'r') as file:
#         for i, line in enumerate(file):
#             if max_lines is not None and i >= max_lines:
#                 break  # Прекращаем чтение, если достигли максимального количества строк
#             # Разделяем строку по запятой и преобразуем в числа
#             coords = line.strip().split(',')
#             if len(coords) == 2:
#                 x.append(float(coords[0].strip()))
#                 y.append(float(coords[1].strip()))
#     return x, y

# def plot_qpsk_symbols(x, y):
#     """Строит график QPSK символов."""
#     plt.figure(figsize=(10, 10))
#     # plt.scatter(x, y, color='blue', marker='o')
#     time = np.linspace(0, len(x))
#     plt.plot(x, time)
#     plt.title('QPSK Symbols Visualization')
#     plt.xlabel('In-Phase (I)')
#     plt.ylabel('Quadrature (Q)')
#     plt.grid(True)
#     # plt.axhline(0, color='black', linewidth=0.5, ls='--')
#     # plt.axvline(0, color='black', linewidth=0.5, ls='--')
#     # plt.xlim(min(x) - 10, max(x) + 10)
#     # plt.ylim(min(y) - 10, max(y) + 10)

#     # # Добавление аннотаций для символов QPSK
#     # for i in range(len(x)):
#     #     plt.annotate(f'({x[i]}, {y[i]})', (x[i], y[i]), textcoords="offset points", xytext=(0,10), ha='center')

#     plt.show()

# def main():
#     parser = argparse.ArgumentParser(description='Visualize QPSK symbols from a data file.')
#     parser.add_argument('file_path', type=str, help='Path to the QPSK data file')
#     parser.add_argument('--max_lines', type=int, help='Maximum number of lines to read from the file', default=None)
#     args = parser.parse_args()

#     # Чтение данных из файла
#     x, y = read_data(args.file_path, args.max_lines)

#     # Визуализация QPSK символов
#     plot_qpsk_symbols(x, y)

# if __name__ == '__main__':
#     main()

import argparse
import matplotlib.pyplot as plt

def read_complex_data(file_path, max_lines=None):
    """Читает комплексные данные из текстового файла и возвращает списки реальных и мнимых частей."""
    real_part = []
    imag_part = []
    with open(file_path, 'r') as file:
        for i, line in enumerate(file):
            if max_lines is not None and i >= max_lines:
                break  # Прекращаем чтение, если достигли максимального количества строк
            # Разделяем строку по запятой и преобразуем в числа
            coords = line.strip().split(',')
            if len(coords) == 2:
                real_part.append(float(coords[0].strip()))
                imag_part.append(float(coords[1].strip()))
    return real_part, imag_part

def plot_signal(real_part, imag_part):
    """Строит графики для реальной и мнимой частей сигнала."""
    time_indices = range(len(real_part))  # Индексы времени

    plt.figure(figsize=(12, 6))

    # График реальной части
    plt.subplot(2, 1, 1)
    plt.plot(time_indices, real_part, color='blue', label='Real Part')
    plt.title('Real Part of the Signal')
    plt.xlabel('Time Index')
    plt.ylabel('Amplitude')
    plt.grid(True)
    plt.axhline(0, color='black', linewidth=0.5, ls='--')
    plt.legend()

    # График мнимой части
    plt.subplot(2, 1, 2)
    plt.plot(time_indices, imag_part, color='red', label='Imaginary Part')
    plt.title('Imaginary Part of the Signal')
    plt.xlabel('Time Index')
    plt.ylabel('Amplitude')
    plt.grid(True)
    plt.axhline(0, color='black', linewidth=0.5, ls='--')
    plt.legend()

    plt.tight_layout()
    plt.show()

def main():
    parser = argparse.ArgumentParser(description='Visualize complex signal over time from a data file.')
    parser.add_argument('file_path', type=str, help='Path to the complex data file')
    parser.add_argument('--max_lines', type=int, help='Maximum number of lines to read from the file', default=None)
    args = parser.parse_args()

    # Чтение комплексных данных из файла
    real_part, imag_part = read_complex_data(args.file_path, args.max_lines)

    # Визуализация сигнала
    plot_signal(real_part, imag_part)

if __name__ == '__main__':
    main()
