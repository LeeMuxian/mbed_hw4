#include"mbed.h"
#include "bbcar.h"
#include "mbed_rpc.h"

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BufferedSerial pc(USBTX,USBRX); //tx,rx
BufferedSerial uart(D1,D0); //tx,rx
BBCar car(pin5, pin6, servo_ticker);

DigitalInOut ping(D10);
Timer t;

void calib(Arguments *in, Reply *out);
RPCFunction Calib(&calib, "calib");

int main() {
    char buf[256], outbuf[256];
    FILE *devin = fdopen(&uart, "r");
    FILE *devout = fdopen(&uart, "w");

    while(1) {
        memset(buf, 0, 256);
        for (int i = 0; ; i++) {
            char recv = fgetc(devin);
            if (recv == '\n') {
                printf("\r\n");
                break;
            }
            buf[i] = fputc(recv, devout);
        }
        //Call the static call method on the RPC class
        RPC::call(buf, outbuf);
        printf("%s\r\n", outbuf);
    }
}

void calib(Arguments *in, Reply *out) {
    double degY = in->getArg<double>();

    if (degY > 355 || degY < 5) {       // if degree is over 180, it means the car is at the right of Apriltag
        float val;
        
        ping.output();
        ping = 0; wait_us(200);
        ping = 1; wait_us(5);
        ping = 0; wait_us(5);

        ping.input();
        while(ping.read() == 0);
        t.start();
        while(ping.read() == 1);
        val = t.read();
        printf("Ping = %lf\r\n", val*17700.4f);
        t.stop();
        t.reset();

        ThisThread::sleep_for(1s);

        return;
    }

    if (degY > 180) {
        car.turn(50, -0.8);
        ThisThread::sleep_for(50ms);
        car.stop();
    } else if (degY > 0) {
        car.turn(50, 0.8);
        ThisThread::sleep_for(100ms);
        car.stop();
    }
}