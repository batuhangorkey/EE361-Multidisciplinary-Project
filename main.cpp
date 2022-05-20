#include "mbed.h"
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

using namespace std::chrono;

#define BAUD_RATE           115200
#define SAMPLE_SIZE         1000
#define SAMPLE_INTERVAL     10ms

AnalogIn ain(PA_0);
DigitalOut led1(LED1);
InterruptIn but1(D6);
Timer t;
Ticker ticker;
BufferedSerial serial_port(PA_9, PA_10, BAUD_RATE);

char read_buf[32] = {0};
float write_buf = 0;
int counter = 0;
int s_size = 0;
bool stop = false;

void stop_sample()
{
    stop = true;
}

void sampling_led()
{
    led1 = !led1;
}

int main()
{
    printf("Online \n");
    printf("Ref voltage: %d\n", (int) ain.get_reference_voltage());
    but1.rise(&stop_sample);
    t.reset();
    led1 = 0;
    serial_port.set_format(8, BufferedSerial::None, 1);
    while (true) {
        serial_port.read(&read_buf, sizeof(read_buf));
        if(read_buf[0] == 's')
        {
            memset(read_buf, 0, sizeof(read_buf));
            serial_port.read(&read_buf, sizeof(read_buf));
            s_size = atoi(read_buf);
            printf("Sampling %d Points...\n", s_size);
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
            printf("The time taken was %llu milliseconds (%llu seconds)\n", duration_cast<milliseconds>(t.elapsed_time()).count(),
                                                                            duration_cast<seconds>(t.elapsed_time()).count());
            t.reset();
            led1 = 0;
            ticker.detach();
        }
        else if(read_buf[0] == 'd')
        {
            while(counter++ < 20)
            {
                uint16_t raw = ain.read_u16();
                printf("Value: %d\n", raw);
                ThisThread::sleep_for(500ms);
            }
            counter = 0;
        }
        else if(read_buf[0] == 'r')
        {
            stop = false;
            while(counter++ < 30000)
            {
                int raw = -1;
                if (stop)
                {
                    printf("Value: %d\n", raw);
                    serial_port.write(&raw, sizeof(raw));
                    printf("Stopped\n");
                    break;
                }
                raw = ain.read_u16();
                serial_port.write(&raw, sizeof(raw));
                printf("Value: %d\n", raw);
                ThisThread::sleep_for(500ms);
            }
            counter = 0;
        }
        ThisThread::sleep_for(1s);
    }
}
