#include "mbed.h"
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std::chrono;

#define BAUD_RATE           115200
#define SAMPLE_INTERVAL     10ms

AnalogIn ain(PA_0);
DigitalOut led1(D9);
InterruptIn but1(D6);
Timer t;
Ticker ticker;
BufferedSerial serial_port(PA_9, PA_10, BAUD_RATE);

char read_buf[32] = {0};
float write_buf = 0;
uint16_t counter = 0;
int32_t raw = -1;
bool stop = false;
milliseconds sample_interval = SAMPLE_INTERVAL;

void on_but1_rise()
{
    stop = true;
}

void on_tick()
{
    led1 = !led1;
}

int main()
{
    printf("Online \n");
    but1.rise(&on_but1_rise);
    t.reset();
    led1 = 0;
    serial_port.set_format(8, BufferedSerial::None, 1);
    while (true) {
        memset(read_buf, 0, 32);
        serial_port.read(read_buf, sizeof(read_buf));
        if(read_buf[0] == 'r')
        {   
            printf("Waiting\n");
            uint32_t buf = 0;
            sample_interval = 10ms;
            serial_port.read(&buf, 4);
            printf("%d\n", buf);
            sample_interval = milliseconds(buf);
            sample_interval = sample_interval < 10ms ? 10ms : sample_interval;
            printf("Sample rate: %lld Hz\n", 1000 / sample_interval.count());
            ticker.attach(&on_tick, 500ms);
            stop = false;
            t.start();
            while(true)
            {
                if (stop)
                {
                    raw = -1;
                    serial_port.write(&raw, sizeof(raw));
                    ticker.detach();
                    printf("Value: %d\n", raw);
                    printf("Stopped\n");
                    break;
                }
                raw = ain.read_u16();
                serial_port.write(&raw, sizeof(raw));
                counter++;
                ThisThread::sleep_for(sample_interval);
            }
            t.stop();
            printf("Total Sampled: %d\n", counter - 1);
            counter = 0;
            printf("The time taken was %llu milliseconds (%llu seconds)\n", duration_cast<milliseconds>(t.elapsed_time()).count(),
                                                                            duration_cast<seconds>(t.elapsed_time()).count());
            t.reset();
            led1 = 0;
        }
        ThisThread::sleep_for(1s);
    }
}
