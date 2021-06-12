#include "mbed.h"
#include "bbcar.h"
#include "mbed_rpc.h"
#include "math.h"

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BufferedSerial pc(USBTX,USBRX); //tx,rx
BufferedSerial uart(D1,D0); //tx,rx
BBCar car(pin5, pin6, servo_ticker);

DigitalInOut ping(D10);
Timer t;

void calib(Arguments *in, Reply *out);
RPCFunction Calib(&calib, "calib");

//bool first = false, measure = false;
//double first_deg, first_dis, sin_dis;

double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
double speed_table0[] = {-9.646, -9.784, -9.025, -8.445, -4.882, 0.000, 5.777, 10.364, 9.885, 9.895, 9.965};
double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
double speed_table1[] = {-8.530, -8.132, -8.690, -8.929, -4.824, 0.000, 4.829, 8.132, 8.371, 9.849, 9.769};

int main() {
    car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);

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
    double deg = in->getArg<double>();
    printf("%f\r\n", deg);

    if (deg >= 175 && deg <= 185) {     // if degree is over 180, it means the car is at the left of Apriltag
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

    if (deg > 185) {       // at the left
        car.turn(100, -0.01);
        ThisThread::sleep_for(1300ms);
        car.stop();
        ThisThread::sleep_for(500ms);
        
        car.goStraightCalib(8);
        ThisThread::sleep_for(1000ms);
        car.stop();
        ThisThread::sleep_for(500ms);

        car.turn(100, 0.01);
        ThisThread::sleep_for(1300ms);
        car.stop();
    }
    else {                      // at the right
        car.turn(100, 0.01);
        ThisThread::sleep_for(1300ms);
        car.stop();
        ThisThread::sleep_for(500ms);
        
        car.goStraightCalib(8);
        ThisThread::sleep_for(1000ms);
        car.stop();
        ThisThread::sleep_for(500ms);

        car.turn(100, -0.01);
        ThisThread::sleep_for(1300ms);
        car.stop();
    }
}
/*void calib(Arguments *in, Reply *out) {
    if (measure == false) {
        double degY = in->getArg<double>();
        double disZ = in->getArg<double>();

        if (first == false) {
            first_deg = degY;
            first_dis = disZ;
            sin_dis = disZ * sin(degY);
            first = true;
        }

        // first and fourth argument : length of table                               
        car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);

        if (first_deg < 180) {
            car.turn(100, -0.1);
            ThisThread::sleep_for(800ms);
            car.stop();
            ThisThread::sleep_for(1500ms);

            car.goStraightCalib(9);
            ThisThread::sleep_for((sin_dis / 9) * 1000);
            car.stop();
            ThisThread::sleep_for(1500ms);

            car.turn(100, 0.1);
            ThisThread::sleep_for(1100ms);
            car.stop();
        } else if (first_deg > 180) {
            car.turn(100, 0.1);
            ThisThread::sleep_for(800ms);
            car.stop();
            ThisThread::sleep_for(1500ms);

            car.goStraightCalib(9);
            ThisThread::sleep_for((sin_dis / 9) * 1000);
            car.stop();
            ThisThread::sleep_for(1500ms);

            car.turn(100, -0.1);
            ThisThread::sleep_for(1100ms);
            car.stop();
        }

        measure = true;
    }

    //if (degY > 350 || degY < 10) {       // if degree is over 180, it means the car is at the right of Apriltag

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
    else if (measure == true) {
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
}*/