# fast discrete biorthogonal CDF 9/7 wavelet

import pywt
import numpy as np

samples = 16
x = np.linspace(0, 2 * np.pi, samples)
y = np.sin(3*x) + np.cos(x)

print("Исходный сигнал: ", y)
print("\n")

coeffs = pywt.wavedec(y, 'bior4.4', level=3)

for i in range(len(coeffs)):
    if i == 0:
        print(f"L: {coeffs[i]}")
    else:
        print(f"H{len(coeffs)-i}: {coeffs[i]}")

reconstructed_y = pywt.waverec(coeffs, 'bior4.4')
print("\nВосстановленный сигнал: ", reconstructed_y)

error = np.linalg.norm(y - reconstructed_y)
print(f"\nПогрешность: {error:.6f}")
