#include "mbed.h"

BusOut display(D6, D7, D9, D10, D11, D5, D4, D8);
char table[12] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x0};

Serial pc( USBTX, USBRX );

AnalogIn Ain(A0);

DigitalOut redLED(LED1);
DigitalOut greenLED(LED2);
DigitalOut dot(D8); //Decimal point

DigitalIn  Switch(SW3);
AnalogOut Aout(DAC0_OUT);

float ADCdata;

Timer t;
float current_time;
float interval = 1/*1(s)*/;


int wave = 0; //state = 1 mean sampling sine wave in the "upper half"
              // state = 0 mean sampling sine wave in the "lower half"
int counter = 0;  

int n = 0;
int n0 = 0;
int n1 = 0;
int n2 = 0;

int main(){

    redLED = 1; //redLED is dark
    greenLED = 0; //greenLED is bright

    t.start();

    current_time = t.read();

    while(t.read() - current_time <= interval){

        ADCdata = Ain;

        if((ADCdata >= 0.33) && (wave == 0)){
            counter++;
            wave = 1;
        }else if((ADCdata <= 0.33) && (wave == 1)){
            counter++;
            wave = 0;
        }
    }

    n = counter / 2;
    n2 = n / 100;
    n1 = (n % 100) / 10;
    n0 = n % 10;

    int sample = 512;
    float ADC_data[512];
    for (int i = 0; i < sample; i++){
        ADC_data[i] = Ain;
        wait(1./sample);
    }

    for (int i = 0; i < sample; i++){
        pc.printf("%1.3f\r\n", ADC_data[i]);
        //wait(0.1);
    }


    while(1){
        
        if(Switch == 0){ //When switch is pushed
            redLED = 0; //redLED is bright
            greenLED = 1; //greenLED is dark

            display = table[n2];
            wait(1);

            display = table[n1];
            wait(1);

            display = table[n0];
            dot = 1; //Decimal point is bright
            wait(1);

            display = table[11]; //all dark

        }else{
            redLED = 1; //redLED is dark
            greenLED = 0; //greenLED is bright
        }
        
        double pi = 3.141592653589;
        double w = 2*pi*n; //w = 2*pi*f
        
        for(float j = 0; j < 2; j += 0.05){
            Aout =  0.5+0.5*sin(w*j);
            wait(0.001);
        }
        
    }

}