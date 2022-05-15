#include "mbed.h"
#include <chrono>
#include <cstdio>

using namespace std::chrono;

#define BAUD_RATE           115200
#define SAMPLE_SIZE         1000
#define SAMPLE_INTERVAL     10ms

AnalogIn ain(PA_0);
PwmOut led1(LED1);
Timer t;
BufferedSerial serial_port(PA_9, PA_10, BAUD_RATE);

char read_buf = 0;
float write_buf = 0;
int counter = 0;

int main()
{
    printf("Online \n");
    t.reset();
    led1.period(1.0f);
    led1.write(0.5f);
    led1.suspend();
    serial_port.set_format(8, BufferedSerial::None, 1);
    while (true) {
        serial_port.read(&read_buf, sizeof(read_buf));
        if(read_buf == 's')
        {
            printf("Sampling \n");
            led1.resume();
            t.start();
            while(counter++ < SAMPLE_SIZE)
            {
                write_buf = ain.read();
                serial_port.write(&write_buf, sizeof(write_buf));
                ThisThread::sleep_for(SAMPLE_INTERVAL);
            }
            t.stop();
            counter = 0;
            led1.suspend();
            t.reset();
            printf("Total Sampled: %d\n", counter - 1);
            printf("The time taken was %llu milliseconds\n", duration_cast<milliseconds>(t.elapsed_time()).count());
        }
        if(read_buf == 'd')
        {
            while(counter++ < 20)
            {
                int raw = ain.read_u16();
                printf("Value: %d\n", raw);
                ThisThread::sleep_for(500ms);
            }
            counter = 0;
        }
        ThisThread::sleep_for(1s);
    }
}
