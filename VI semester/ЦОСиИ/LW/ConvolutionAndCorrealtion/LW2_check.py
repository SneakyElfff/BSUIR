# Var2
# y = sin(3x), z = cos(x), N = 16

import numpy as np
import matplotlib.pyplot as plt


def convolve_signals(signal1, signal2):
    return np.convolve(signal1, signal2, mode='full')


def convolve_signals_fft(signal1, signal2):
    len_X = len(signal1)
    len_Y = len(signal2)
    len_res = len_X + len_Y - 1  # без такой длины отобразится только переходный фрагмент

    Z = np.fft.ifft(np.fft.fft(signal1, len_res) * np.fft.fft(signal2, len_res))

    return np.real(Z)


def correlate_signals(signal1, signal2):
    return np.correlate(signal1, signal2, mode='full')


def correlate_signals_fft(signal1, signal2):
    signal2 = np.flip(signal2)

    len_X = len(signal1)
    len_Y = len(signal2)
    len_res = len_X + len_Y - 1

    Z = np.fft.ifft(np.fft.fft(signal1, len_res) * np.fft.fft(signal2, len_res))

    return np.real(Z)

def main():
    N = 1000
    samples = 16

    x = np.linspace(0, 2*np.pi, N)
    y = np.sin(3 * x)
    z = np.cos(x)

    conv_result = convolve_signals(z, y)
    cor_result = correlate_signals(z, y)

    x_fft = np.linspace(0, 2*np.pi, samples)
    y_fft = np.sin(3 * x_fft)
    z_fft = np.cos(x_fft)

    conv_result_fft = convolve_signals_fft(z_fft, y_fft)
    cor_result_fft = correlate_signals_fft(z_fft, y_fft)

    plt.plot(x, y)
    plt.title('Signal y = sin(3x)')
    plt.xlabel('x')
    plt.ylabel('y')
    plt.show()

    plt.plot(x, z)
    plt.title('Signal z = cos(x)')
    plt.xlabel('x')
    plt.ylabel('z')
    plt.show()

    plt.plot(conv_result)
    plt.title('Convolution of y and z')
    plt.xlabel('x')
    plt.ylabel('Convolution')
    plt.show()

    plt.plot(cor_result)
    plt.title('Correlation of y and z')
    plt.xlabel('x')
    plt.ylabel('Correlation')
    plt.show()

    plt.plot(conv_result_fft)
    plt.title('Convolution of y and z (FFT)')
    plt.xlabel('x')
    plt.ylabel('Convolution')
    plt.show()

    plt.plot(cor_result_fft)
    plt.title('Correlation of y and z (FFT)')
    plt.xlabel('x')
    plt.ylabel('Correlation')
    plt.show()


if __name__ == "__main__":
    main()
