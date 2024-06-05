# Var 9/7
# y = sin(3x) + cos(x), N = 16
# Быстрое вейвлет-преобразование (схема Маллата), биортогональный 9/7 вейвлет (CFD)

import numpy as np


def wt97(signal):
    # НЧ и ВЧ фильтры (анализа)
    analysis_lowpass = np.array([0.026748757411, -0.016864118443, -0.078223266529,
                                 0.266864118443, 0.602949018236, 0.266864118443,
                                 -0.078223266529, -0.016864118443, 0.026748757411])
    analysis_highpass = np.array([0, 0.091271763114, -0.057543526229,
                                  -0.591271763114, 1.11508705, -0.591271763114,
                                  -0.057543526229, 0.091271763114, 0])

    # применить НЧ-фильтр
    lowpass_output = np.convolve(analysis_lowpass, signal, mode='same')

    # применить ВЧ-фильтр
    highpass_output = np.convolve(analysis_highpass, signal, mode='same')

    # децимировать и урезать
    N = len(signal)
    lowpass_output = lowpass_output[1:N:2]
    highpass_output = highpass_output[1:N:2]

    return lowpass_output, highpass_output


def iwt97(lowpass, highpass):
    # интерполировать
    N = len(lowpass) * 2
    upsampled_lowpass = np.zeros(N)
    upsampled_lowpass[::2] = lowpass
    upsampled_highpass = np.zeros(N)
    upsampled_highpass[::2] = highpass

    # НЧ и ВЧ фильтры (синтеза)
    synthesis_lowpass = np.array([0, -0.091271763114, -0.057543526229, 0.591271763114,
                                 1.11508705, 0.591271763114, -0.057543526229,
                                 -0.091271763114, 0])
    synthesis_highpass = np.array([0.026748757411, 0.016864118443, -0.078223266529,
                                  -0.266864118443, 0.602949018236, -0.266864118443,
                                  -0.078223266529, 0.016864118443, 0.026748757411])

    # применить НЧ-фильтр
    lowpass_output = np.convolve(upsampled_lowpass, synthesis_lowpass, mode='same')

    # применить ВЧ-фильтр
    highpass_output = np.convolve(upsampled_highpass, synthesis_highpass, mode='same')

    # урезать до ожидаемой длины
    lowpass_output = lowpass_output[:N]
    highpass_output = highpass_output[:N]

    signal = lowpass_output + highpass_output

    return signal


def main():
    samples = 16
    x = np.linspace(0, 2*np.pi, samples)
    y = np.sin(3*x) + np.cos(x)

    print("Исходный сигнал: ", y)

    lowpass1, highpass1 = wt97(y)
    lowpass2, highpass2 = wt97(lowpass1)
    lowpass3, highpass3 = wt97(lowpass2)

    print("\nL:", lowpass3)
    print("H1:", highpass1)
    print("H2:", highpass2)
    print("H3:", highpass3)

    reconstructed_lowpass2 = iwt97(lowpass3, highpass3)
    reconstructed_lowpass1 = iwt97(reconstructed_lowpass2, highpass2)
    reconstructed_y = iwt97(reconstructed_lowpass1, highpass1)

    print("\nВосстановленный сигнал:")
    print(reconstructed_y)


if __name__ == "__main__":
    main()
