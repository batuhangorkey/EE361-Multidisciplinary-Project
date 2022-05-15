#include "mbed.h"
#include <chrono>
#include <cstdio>
#include <cstdlib>

using namespace std::chrono;

#define BAUD_RATE           115200
#define SAMPLE_SIZE         1000
#define SAMPLE_INTERVAL     10ms

AnalogIn ain(PA_0);
DigitalOut led1(LED1);
Timer t;
Ticker ticker;
BufferedSerial serial_port(PA_9, PA_10, BAUD_RATE);

char read_buf[32] = {0};
float write_buf = 0;
int counter = 0;
int s_size = 0;

void sampling_led()
{
    led1 = !led1;
}

int main()
{
    printf("Online \n");
    t.reset();
    led1 = 0;
    serial_port.set_format(8, BufferedSerial::None, 1);
    while (true) {
        serial_port.read(&read_buf, sizeof(read_buf));
        if(read_buf[0] == 's')
        {
            serial_port.read(&read_buf, sizeof(read_buf));
            s_size = atoi(read_buf);
            printf("Sampling %d\n", s_size);
            ticker.attach(&sampling_led, 500ms);
            t.start();
            while(counter++ < s_size)
            {
                write_buf = ain.read();
                serial_port.write(&write_buf, sizeof(write_buf));
                ThisThread::sleep_for(SAMPLE_INTERVAL);
            }
            t.stop();
            printf("Total Sampled: %d\n", counter - 1);
            counter = 0;
            printf("The time taken was %llu milliseconds\n", duration_cast<milliseconds>(t.elapsed_time()).count());
            t.reset();
            led1 = 0;
            ticker.detach();
        }
        else if(read_buf[0] == 'd')
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
