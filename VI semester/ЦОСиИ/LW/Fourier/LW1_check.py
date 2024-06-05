import numpy as np
import matplotlib.pyplot as plt

def func(x):
    return np.sin(3*x) + np.cos(x)

# Number of samples
N = 16

# Generate time samples
x = np.linspace(0, 2*np.pi, N)

y = func(x)

fft_result = np.fft.fft(y)

amplitude_spectrum = np.abs(fft_result)

phase_spectrum = np.angle(fft_result)

plt.plot(x, y, 'b.-')
plt.title('y = sin(3x) + cos(x)')
plt.xlabel('Время')
plt.ylabel('Амплитуда')
plt.grid(True)
plt.show()

plt.stem(amplitude_spectrum)
plt.title('Амплитудный спектр')
plt.xlabel('Частота')
plt.ylabel('Амплитуда')
plt.grid(True)
plt.show()

plt.stem(phase_spectrum)
plt.title('Фазовый спектр')
plt.xlabel('Частота')
plt.ylabel('Фаза')
plt.grid(True)
plt.show()

ifft_result = np.fft.ifft(fft_result)

plt.plot(x, y, 'b', label='Исходный сигнал')
plt.plot(x, ifft_result, 'r--', label='После ОБПФ')
plt.title('После ОБПФ')
plt.xlabel('Время')
plt.ylabel('Амплитуда')
plt.legend()
plt.grid(True)
plt.show()
