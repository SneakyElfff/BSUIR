import numpy as np


def fft_dif(a):
    N = len(a)

    if N == 2:
        y = np.zeros(2, dtype=np.complex_)
        y[0] = a[0] + a[1]
        y[1] = a[0] - a[1]
        return y

    else:
        b = np.zeros(N // 2, dtype=complex)
        c = np.zeros(N // 2, dtype=complex)

        for j in range(0, N // 2):
            w = np.exp(-2j * np.pi * j / N)
            b[j] = a[j] + a[j + N // 2]
            c[j] = (a[j] - a[j + N // 2]) * w

        y_b = fft_dif(b)
        y_c = fft_dif(c)

        # Combining results
        y = np.zeros(N, dtype=complex)
        for j in range(N // 2):
            y[2 * j] = y_b[j]
            y[2 * j + 1] = y_c[j]

        # Returns the calculated fast fourier transform coefficients.
        return y


def ifft_dif(y):
    N = len(y)

    if N == 2:
        a = np.zeros(2, dtype=np.complex_)
        a[0] = (y[0] + y[1]) / 2
        a[1] = (y[0] - y[1]) / 2
        return a

    else:
        b = np.zeros(N // 2, dtype=complex)
        c = np.zeros(N // 2, dtype=complex)

        for j in range(0, N // 2):
            w = np.exp(2j * np.pi * j / N)
            b[j] = (y[j] + y[j + N // 2])
            c[j] = (y[j] - y[j + N // 2]) * w

        a_b = ifft_dif(b)
        a_c = ifft_dif(c)

        # Combining results
        a = np.zeros(N, dtype=complex)
        for j in range(N // 2):
            a[2 * j] = a_b[j]
            a[2 * j + 1] = a_c[j]

        # Returns the calculated inverse fast fourier transform coefficients.
        return a
