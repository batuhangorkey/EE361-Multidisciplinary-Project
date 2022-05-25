import matplotlib.figure
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from scipy.signal import butter, freqz, lfilter

DELTA = 10
SYS_VOLTAGE = 3.3
RESOLUTION = 65536
CALIBRATION = 21.9
SLOPE = -5.65
sample_interval = 50e-3
fs = 1000
order = 2
cutoff = 2


def butter_lowpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype='low', analog=False, output='ba')
    return b, a


def butter_lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y

def draw_magnitude():
    fig1, ax1 = plt.subplots()
    ax1.set_title("Magnitude Spectrum")
    ax1.magnitude_spectrum(voltage, Fs=fs, scale='default', color='C1')


def draw_freq_response():
    w, h = freqz(b, a, worN=8000)
    fig_freq, ax_freq = plt.subplots()
    ax_freq.plot(0.5 * fs * w / np.pi, np.abs(h), 'b')
    ax_freq.plot(cutoff, 0.5 * np.sqrt(2), 'ko')
    ax_freq.axvline(cutoff, color='k')
    ax_freq.set_xlim(0, 0.5 * fs)


if __name__ == '__main__':
    sample_max = 6000
    samples = pd.read_csv('samples.csv')
    # t = samples['time'].to_numpy()
    voltage = samples['voltage_percentage'].to_numpy()
    voltage = voltage[0:sample_max]
    voltage = (SLOPE * (SYS_VOLTAGE * voltage / RESOLUTION)) + CALIBRATION
    t_min = np.arange(0, sample_max * sample_interval, sample_interval) / 60
    t_msec = np.arange(0, sample_max * sample_interval * 1000, sample_interval * 1000)
    b, a = butter_lowpass(cutoff, fs, order)
    # output length from valid mode
    # max(M, N) - min(M, N) + 1
    voltage_avg = np.convolve(voltage, np.ones(DELTA), mode='valid') / DELTA

    initial_volt_correct = np.mean(voltage[0:100])
    y = butter_lowpass_filter(voltage - initial_volt_correct, cutoff, fs, order) + initial_volt_correct

    data = {'time[milliseconds]': t_msec, 'ph': y}
    df = pd.DataFrame(data)
    df.to_csv('ph_data_filtered.csv')

    fig3: matplotlib.figure.Figure = plt.figure()
    (ax4, ax5) = fig3.subplots(1, 2)
    ax4: matplotlib.figure.Axes

    ax4.plot(t_min, voltage, 'b-', linewidth=0.5, label='data')
    ax5.plot(t_min, y, 'g-', linewidth=2, label='low-pass filter')
    # ax6.plot(t[DELTA - 1:], voltage_avg, label='averaging')
    ax4.set_xlabel('Time [min]')
    ax4.set_ylabel('Ph')
    ax5.set_xlabel('Time [min]')
    ax5.set_ylabel('Ph')
    ax4.set_ylim(3.5, 7.5)
    ax5.set_ylim(3.5, 7.5)
    ax4.grid()
    ax5.grid()
    # ax6.grid()
    fig3.legend()
    fig3.set_size_inches(1600 / fig3.dpi, 800 / fig3.dpi)
    fig3.savefig('exp_data.png')
    # plt.show()
