# Var 8
# y = sin(3x) + cos(x), N = 16
# КИХ-фильтр (FIR-filter). ВЧ оконный фильтр. Окно Хэмминга

import numpy as np
import matplotlib.pyplot as plt
import fft


def design_filter(num_taps, cutoff_freq, nyquist_rate):
    # Вычисляем нормализованные частоты
    cutoff_freq_normalized = cutoff_freq / nyquist_rate

    # Создаем массив для коэффициентов фильтра
    b = np.zeros(num_taps)

    # Вычисляем коэффициенты фильтра
    for i in range(num_taps):
        if i == (num_taps - 1) // 2:
            b[i] = 2 * cutoff_freq_normalized
        else:
            b[i] = np.sin(2 * np.pi * cutoff_freq_normalized * (i - (num_taps - 1) // 2)) / (np.pi * (i - (num_taps - 1) // 2))

    return b


def hamming_window(num_taps):
    # Создаем массив для окна Хэмминга
    window = np.zeros(num_taps)

    # Вычисляем значения окна
    for i in range(num_taps):
        window[i] = 0.54 - 0.46 * np.cos(2 * np.pi * i / (num_taps - 1))

    return window

# Создание исходного сигнала
x = np.linspace(0, 2 * np.pi, 32)
y = np.sin(3*x) + np.cos(x)

# Параметры фильтра
nyquist_rate = 0.5  # Частота Найквиста
cutoff_freq = 0.2   # Отсечка частоты для ВЧ фильтра
num_taps = 3       # Количество коэффициентов фильтра

# Применение окна Хэмминга к исходному сигналу
hamming_window_values = hamming_window(len(y))
hamming_y = y * hamming_window_values

freq_domain = fft.fft(hamming_y)
freq_hamming_y = np.linspace(0, 1 / (x[1] - x[0]), num=len(freq_domain))

# Проектирование фильтра
b = design_filter(num_taps, cutoff_freq, nyquist_rate)

# Применение фильтра к исходному сигналу
filtered_y = np.convolve(hamming_y, b, mode='same')

# FIRF frequency domain
freq_domain_FIRF = fft.fft(filtered_y)
freq_filtered_y = np.linspace(0, 1 / (x[1] - x[0]), num=len(freq_domain_FIRF))

# Визуализация исходного сигнала
plt.figure()
plt.plot(x, y, label='Исходный сигнал')
plt.xlabel('Время')
plt.ylabel('Амплитуда')
plt.legend()
plt.title('Исходный сигнал')
plt.grid(True)
plt.show()

# Визуализация исходного сигнала (+ окно Хэмминга)
plt.figure()
plt.plot(x, hamming_y, label='Окно Хэмминга')
plt.xlabel('Время')
plt.ylabel('Амплитуда')
plt.legend()
plt.title('Исходный сигнал (окно Хэмминга)')
plt.grid(True)
plt.show()

# Визуализация исходного сигнала (+ окно Хэмминга) в частотном диапазоне
plt.figure()
plt.stem(freq_hamming_y, freq_domain, label='Окно Хэмминга')
plt.xlabel('Частота')
plt.ylabel('Амплитуда')
plt.legend()
plt.title('Частотный диапазон')
plt.grid(True)
plt.show()

# Визуализация отфильтрованного сигнала
plt.figure()
plt.plot(x, filtered_y, label='Отфильтрованный сигнал')
plt.xlabel('Время')
plt.ylabel('Амплитуда')
plt.legend()
plt.title('Отфильтрованный сигнал')
plt.grid(True)
plt.show()

# Визуализация отфильтрованного сигнала
plt.figure()
plt.stem(freq_filtered_y, freq_domain_FIRF, label='Отфильтрованный сигнал')
plt.xlabel('Частота')
plt.ylabel('Амплитуда')
plt.legend()
plt.title('Частотный диапазон')
plt.grid(True)
plt.show()
