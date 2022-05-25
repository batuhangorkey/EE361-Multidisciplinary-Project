# EE361 Multidisciplinary project
# PH Probe sampling
# with STM32 nucleo l432kc analog-to-digital converter
# Live Graphing and post-filtering done using matplotlib, numpy, scipy
import struct
import time

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import serial
from matplotlib.animation import FuncAnimation

PORT = 'COM4'
SAMPLE_SIZE = 1000
SAMPLE_RATE = 10e-3
T = int(SAMPLE_SIZE * SAMPLE_RATE)
try:
    ser = serial.Serial(port=PORT, baudrate=115200, timeout=1)
except Exception as e:
    raise serial.PortNotOpenError()


def sample():
    raw_data = np.zeros(SAMPLE_SIZE)
    t = np.linspace(0, T, SAMPLE_SIZE)
    if not ser.is_open:
        ser.open()
    ser.write(b"s\0")
    time.sleep(0.5)
    ser.write(SAMPLE_SIZE.to_bytes(4, 'little', signed=False))
    c = 0
    while c < SAMPLE_SIZE:
        raw = ser.read(4)
        [value] = struct.unpack('f', raw)
        raw_data[c] = value
        print(f"{value}\n")
        c += 1
    return t, raw_data


def draw_graph(t, raw_data):
    plt.plot(t, raw_data)
    plt.ylim(0.0, 1.0)
    plt.show()


def mode1():
    t, raw_data = sample()
    draw_graph(t, raw_data)
    data = {'time': t, 'voltage_percentage': raw_data}
    df = pd.DataFrame(data)
    df.to_csv('samples.csv')


def mode2():
    ser.write(b"d")


def animate(i, ys):
    raw = ser.read(4)
    val = int.from_bytes(raw, 'little', signed=True)
    if val == -1:
        ani.event_source.stop()
        plt.close()
        data = {'voltage_percentage': raw_data}
        df = pd.DataFrame(data)
        df.to_csv('samples.csv')
        return line,
    norm = (-5.7 * (3.3 * val / 65536)) + 21.9
    raw_data.append(val)
    ys.append(norm)
    ys = ys[-x_len:]
    line.set_ydata(ys)
    return line,


if __name__ == '__main__':
    sample_rate_r = 50
    raw_data = list()
    x_len = int(60 * 1000 / sample_rate_r)     # Number of points to display
    xs = np.linspace(0, 60, 1200, False)
    y_range = [1, 14]  # Range of possible Y values to display
    ys = [7] * x_len
    fig, ax = plt.subplots()
    ax.set_ylim(y_range)
    ax.set_xlabel('Time [sec]')
    ax.set_ylabel('PH')
    ax.grid(True)
    line, = ax.plot(xs, ys, linewidth=1)

    ser.write(b"r\0")
    time.sleep(0.5)
    ser.write(sample_rate_r.to_bytes(4, 'little', signed=False))

    ani = FuncAnimation(fig, animate, fargs=(ys, ),
                        interval=sample_rate_r, blit=True)
    
    # ani.save('sample.mp4', dpi='figure')
    plt.show()
    