import numpy as np

p1_97 = -1.586134342
ip1_97 = -p1_97

u1_97 = -0.05298011854
iu1_97 = -u1_97

p2_97 = 0.8829110762
ip2_97 = -p2_97

u2_97 = 0.4435068522
iu2_97 = -u2_97

scale97 = 1.149604398
iscale97 = 1.0 / scale97


# биортогональное 9/7 вейвлет-преобразование
def fwt97(signal):
    n = len(signal)

    # блок предсказания 1
    for i in range(1, n - 2, 2):
        signal[i] += p1_97 * (signal[i - 1] + signal[i + 1])
    signal[n - 1] += 2 * p1_97 * signal[n - 2]

    # блок обновления 1
    for i in range(2, n, 2):
        signal[i] += u1_97 * (signal[i - 1] + signal[i + 1])
    signal[0] += 2 * u1_97 * signal[1]

    # блок предсказания 2
    for i in range(1, n - 2, 2):
        signal[i] += p2_97 * (signal[i - 1] + signal[i + 1])
    signal[n - 1] += 2 * p2_97 * signal[n - 2]

    # блок обновления 2
    for i in range(2, n, 2):
        signal[i] += u2_97 * (signal[i - 1] + signal[i + 1])
    signal[0] += 2 * u2_97 * signal[1]

    # scale
    for i in range(n):
        if i % 2 != 0:
            signal[i] *= iscale97
        else:
            signal[i] /= iscale97

    # pack
    tb = np.zeros(n)
    for i in range(n):
        if i % 2 == 0:
            tb[i // 2] = signal[i]
        else:
            tb[n // 2 + i // 2] = signal[i]
    signal[:] = tb


# обратное биортогональное 9/7 вейвлет-преобразование
def iwt97(signal):
    n = len(signal)

    # unpack
    tb = np.zeros(n)
    for i in range(n // 2):
        tb[i * 2] = signal[i]
        tb[i * 2 + 1] = signal[i + n // 2]
    signal[:] = tb

    # undo scale
    for i in range(n):
        if i % 2 != 0:
            signal[i] *= scale97
        else:
            signal[i] /= scale97

    # обратить обновление 2
    for i in range(2, n, 2):
        signal[i] += iu2_97 * (signal[i - 1] + signal[i + 1])
    signal[0] += 2 * iu2_97 * signal[1]

    # обратить предсказание 2
    for i in range(1, n - 2, 2):
        signal[i] += ip2_97 * (signal[i - 1] + signal[i + 1])
    signal[n - 1] += 2 * ip2_97 * signal[n - 2]

    # обратить обновление 1
    for i in range(2, n, 2):
        signal[i] += iu1_97 * (signal[i - 1] + signal[i + 1])
    signal[0] += 2 * iu1_97 * signal[1]

    # обратить предсказание 1
    for i in range(1, n - 2, 2):
        signal[i] += ip1_97 * (signal[i - 1] + signal[i + 1])
    signal[n - 1] += 2 * ip1_97 * signal[n - 2]


samples = 16
x = np.linspace(0, 2 * np.pi, samples)
y = np.sin(3*x) + np.cos(x)
original_y = y.copy()

print("Исходный сигнал: ", y)
fwt97(y)
print("\nПреобразованный сигнал: ", y)

iwt97(y)
print("\nВосстановленный сигнал: ", y)

assert np.allclose(original_y, y, atol=1e-10), "Сигналы не совпали."
