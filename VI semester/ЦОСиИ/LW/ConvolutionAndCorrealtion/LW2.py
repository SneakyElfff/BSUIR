# Var2
# y = sin(3x), z = cos(x), N = 16

import numpy as np
import matplotlib.pyplot as plt
import FFTAndIFFT as fourier


# Z(m) = 1/N * SUM(x(h)*y(m-h)), m = [0, N-1]
def convolve_signals(X, Y, N):
    result = np.zeros(N)

    for m in range(N):
        for h in range(N):
            if m - h < 0:
                break
            result[m] += X[h] * Y[m - h]
        result[m] /= N

    return result


def convolve_with_fft(X, Y):
    len_X = len(X)
    len_Y = len(Y)
    len_res = len_X + len_Y - 1  # без такой длины отобразится только переходный фрагмент

    # дополнить нулями до ближайшей степени двойки (>= len_res) для оптимизации БПФ
    n = 2 ** int(np.ceil(np.log2(len_res)))
    X_pad = np.pad(X, (0, n - len_X))
    Y_pad = np.pad(Y, (0, n - len_Y))

    C_x = fourier.fft_dif(X_pad)
    C_y = fourier.fft_dif(Y_pad)

    C_z = C_x * C_y

    Z = fourier.ifft_dif(C_z)

    # для удобного сравнения с результатом без БПФ
    return np.real(Z[: len_res // 2])

    # фрагмент целиком как при использовании библиотечной функции
    # return Z


# Z(m) = 1/N * SUM(x(h)*y(m+h)), m = [0, N-1]
def correlate_signals(X, Y, N):
    result = np.zeros(N)

    for m in range(N):
        for h in range(N):
            if m + h > N - 1:
                break
            result[m] += X[h] * Y[m + h]
        result[m] /= N

    return result


def correlate_with_fft(X, Y):
    Y = np.flip(Y)

    len_X = len(X)
    len_Y = len(Y)
    len_res = len_X + len_Y - 1

    # дополнить нулями до ближайшей степени двойки для оптимизации БПФ
    n = 2 ** int(np.ceil(np.log2(len_res)))
    X_pad = np.pad(X, (0, n - len_X))
    Y_pad = np.pad(Y, (0, n - len_Y))

    C_x = fourier.fft_dif(X_pad)
    C_y = fourier.fft_dif(Y_pad)

    C_z = C_x * C_y

    Z = fourier.ifft_dif(C_z)

    return np.real(Z[: len_res // 2])

    # return Z


def main():
    N = 1000
    samples = 16

    x = np.linspace(0, 2 * np.pi, N)
    y = np.sin(3 * x)
    z = np.cos(x)

    conv_result = convolve_signals(z, y, N)
    cor_result = correlate_signals(z, y, N)

    x_fft = np.linspace(0, 2 * np.pi, samples)
    y_fft = np.sin(3 * x_fft)
    z_fft = np.cos(x_fft)

    conv_FFT_result = convolve_with_fft(z_fft, y_fft)
    cor_FFT_result = correlate_with_fft(z_fft, y_fft)

    plt.plot(x, y, label='y = sin(3x)')
    plt.title('Signal y = sin(3x)')
    plt.xlabel('x')
    plt.ylabel('Amplitude')
    plt.legend()
    plt.show()

    plt.plot(x, z, label='z = cos(x)')
    plt.title('Signal z = cos(x)')
    plt.xlabel('x')
    plt.ylabel('Amplitude')
    plt.legend()
    plt.show()

    plt.plot(x, conv_result, label='Convolution')
    plt.title('Convolution of y and z')
    plt.xlabel('x')
    plt.ylabel('Amplitude')
    plt.legend()
    plt.show()

    plt.plot(x, cor_result, label='Correlation')
    plt.title('Correlation of y and z')
    plt.xlabel('x')
    plt.ylabel('Amplitude')
    plt.legend()
    plt.show()

    plt.plot(conv_FFT_result, label='Convolution')
    plt.title('Convolution of y and z (FFT)')
    plt.xlabel('x')
    plt.ylabel('Amplitude')
    plt.legend()
    plt.show()

    plt.plot(cor_FFT_result, label='Correlation')
    plt.title('Correlation of y and z (FFT)')
    plt.xlabel('x')
    plt.ylabel('Amplitude')
    plt.legend()
    plt.show()


if __name__ == "__main__":
    main()
