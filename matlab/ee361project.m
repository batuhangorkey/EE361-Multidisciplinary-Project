close all
clearvars
clc 
size = 2000;
s_interval = 10e-3;
fs = s_interval^-1;

mc = serialport("COM5", 115200);
write(mc, 's', 'uint8');
data = read(mc, size + 1, 'single');
t = 0 : s_interval : size * s_interval;
plot(t, data);
xlabel('Time (s)');
ylabel('Amplitude');
xlim([0, size * s_interval]);
ylim([0, 1.2]);
title('Sampled Signal');
writematrix(data, 'samples.csv');