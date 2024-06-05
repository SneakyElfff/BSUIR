# Var 8
# y = sin(3x) + cos(x)
# КИХ-фильтр (FIR-filter). ВЧ оконный фильтр. Окно Хэмминга

import numpy as np
import matplotlib.pyplot as plt
from numpy import fft


def designHighPassWindowFIR(noise_sequence, M, N, Fc):
    # входной сигнал в частотной области
    noise_sequence_fft = fft.fft(noise_sequence)

    # создать массив для весовых коэффициентов фильтра
    coeffs = []

    # вычислить коэффициенты фильтра
    for i in range(N):
        window = 0.54 - 0.46 * np.cos(2 * np.pi * i / M)
        if (i - (M / 2)) == 0:
            # coeffs.append(2 * np.pi * Fc)  # НЧ-фильтр
            coeffs.append(1 - 2 * Fc)  # ВЧ-фильтр
        else:
            # coeffs.append(np.sin(2 * np.pi * Fc * (i - (M / 2))) / (i - (M / 2)))  # НЧ-фильтр
            coeffs.append(-np.sin(2 * np.pi * Fc * (i - (M / 2))) / (i - (M / 2)))  # ВЧ-фильтр
        coeffs[i] *= window  # окно Хэмминга

    # нормирование коэффициентов фильтра для получения единичного коэффициента усиления на нулевой частоте
    coeffs_res = [element / np.sum(coeffs) for element in coeffs]

    # применение фильтра к исходному сигналу
    coagulation = np.array(noise_sequence_fft).copy()
    for i in range(M, np.size(noise_sequence_fft) // 2 + 1):
        coagulation[i] = 0
        for j in range(M):
            coagulation[i] = coagulation[i] + noise_sequence_fft[i - j] * coeffs_res[j]
            coagulation[np.size(noise_sequence_fft) - i] = coagulation[i]

    y_filtered = fft.ifft(coagulation)

    return y_filtered


def main():
    samples = 64
    x = np.linspace(0, 2 * np.pi, samples)
    y = np.sin(3 * x) + np.cos(x)

    # шум
    noise = np.sin(2 * np.pi * 4 * x)
    noise_sequence = np.array(y) + noise

    # параметры фильтра
    Fc = 25  # частота среза
    order = 20  # порядок фильтра

    y_filtered = designHighPassWindowFIR(noise_sequence, order, samples, Fc)

    gridsize = (15, 4)
    fig = plt.figure(figsize=(15, 10))
    fig.suptitle('LW3', fontfamily='serif', fontstyle='italic', fontsize=30)

    plt1 = plt.subplot2grid(gridsize, (1, 0), colspan=2, rowspan=2)
    plt1.set_xlabel('Время')
    plt1.set_ylabel('Амплитуда')
    plt1.set_title('Исходный сигнал', fontsize=20)
    plt1.grid(True)
    plt1.plot(x, y, label='y = sin(3x) + cos(x)')

    plt2 = plt.subplot2grid(gridsize, (1, 2), colspan=2, rowspan=2)
    plt2.set_xlabel('Частота')
    plt2.set_ylabel('Амплитуда')
    plt2.set_title('АЧХ исходного сигнала', fontsize=20)
    plt2.grid(True)
    plt2.plot(x, np.abs(fft.fft(y)), label='y = sin(3x) + cos(x)')

    plt3 = plt.subplot2grid(gridsize, (5, 0), colspan=2, rowspan=2)
    plt3.set_xlabel('Время')
    plt3.set_ylabel('Амплитуда')
    plt3.set_title('Шум', fontsize=20)
    plt3.grid(True)
    plt3.plot(x, noise)

    plt4 = plt.subplot2grid(gridsize, (5, 2), colspan=2, rowspan=2)
    plt4.set_xlabel('Частота')
    plt4.set_ylabel('Амплитуда')
    plt4.set_title('АЧХ шума', fontsize=20)
    plt4.grid(True)
    plt4.plot(x, np.abs(fft.fft(noise)))

    plt5 = plt.subplot2grid(gridsize, (9, 0), colspan=2, rowspan=2)
    plt5.set_xlabel('Время')
    plt5.set_ylabel('Амплитуда')
    plt5.set_title('Сигнал с помехами', fontsize=20)
    plt5.grid(True)
    plt5.plot(x, noise_sequence)

    plt6 = plt.subplot2grid(gridsize, (9, 2), colspan=2, rowspan=2)
    plt6.set_xlabel('Частота')
    plt6.set_ylabel('Амплитуда')
    plt6.set_title('АЧХ сигнала с помехами', fontsize=20)
    plt6.grid(True)
    plt6.plot(x, np.abs(fft.fft(noise_sequence)))

    plt7 = plt.subplot2grid(gridsize, (13, 0), colspan=2, rowspan=2)
    plt7.set_xlabel('Время')
    plt7.set_ylabel('Амплитуда')
    plt7.set_title('Отфильтрованный сигнал', fontsize=20)
    plt7.grid(True)
    plt7.plot(x, y_filtered)

    plt8 = plt.subplot2grid(gridsize, (13, 2), colspan=2, rowspan=2)
    plt8.set_xlabel('Частота')
    plt8.set_ylabel('Амплитуда')
    plt8.set_title('АЧХ отфильтрованного сигнала', fontsize=20)
    plt8.grid(True)
    plt8.plot(x, np.abs(fft.fft(y_filtered)))

    plt.show()


if __name__ == "__main__":
    main()
