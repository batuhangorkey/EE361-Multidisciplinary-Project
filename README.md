# EE361 - Digital Signal Processing - Multidisciplinary Project
### About
PH Probe sampling and transmitting data using STM32 Nucleo-L432KC analog-to-digital converter and CP2102 USB UART module
Graphing and Low-Pass Filtering done on Python using libraries: serial, matplotlib, numpy, scipy
#### Nucleo L432KC
Controller awaits 'r' character from serial port. In a new line it reads sampling interval integer in milliseconds format. After receiving, controller starts converting voltage on A0 pin to a integer value which is transmitted to the serial port of PC through the UART module with the specified sampling interval until the interrupt button is pressed. Interrupt button sends -1 value to the serial port to let Python code know that sampling is stopped.
#### Python
During sampling, a live plot is drawn of the last minute. If -1 value is read from serial port, it stops drawing and saves all read sample data to a new .csv file. Two order 1 kHz low pass filter with cutoff frequency 2 Hz is then applied to this data and a new figure is drawn showing differences between raw and filtered data.
