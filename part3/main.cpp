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

bool check = false, first = false;
double first_deg;

double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
double speed_table0[] = {-9.646, -9.784, -9.025, -8.445, -4.882, 0.000, 5.777, 10.364, 9.885, 9.895, 9.965};
double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
double speed_table1[] = {-8.530, -8.132, -8.690, -8.929, -4.824, 0.000, 4.829, 8.132, 8.371, 9.849, 9.769};

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

    if (first == false) {
        first_deg = degY;
        first = true;
    }

    // first and fourth argument : length of table                               
    car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);

    if (degY > 350 || degY < 10) {       // if degree is over 180, it means the car is at the right of Apriltag
        if (degY < 5 && check == false) {
            car.goStraight(100);
            ThisThread::sleep_for(300ms);
            car.stop();
            ThisThread::sleep_for(50ms);
            car.turn(60, 0.6);
            ThisThread::sleep_for(800ms);
            car.stop();
            check = true;
            printf("33333\r\n");
        } else if (degY > 355 && check == false) {
            car.goStraight(100);
            ThisThread::sleep_for(300ms);
            car.stop();
            ThisThread::sleep_for(50ms);
            car.turn(60, 0.6);
            ThisThread::sleep_for(800ms);
            car.stop();
            check = true;
            printf("33333\r\n");
        }

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

    car.goStraight(100);
    ThisThread::sleep_for(50ms);
    car.stop();
    printf("1111\r\n");
}