#include <xc.h>
#include "LCD.h"
#include "../LSM303DLHC.h"

#pragma config FOSC=HSPLL
#pragma config WDTEN=OFF
#pragma config XINST=OFF


/*
Connections:
        Master RD5 <-> SDA
        Master RD6 <-> SCL
 */

void InitPins(void);
void ConfigInterrupts(void);
void ConfigPeriph(void);

void main(void) {
    OSCTUNEbits.PLLEN = 1;
    LCDInit();
    lprintf(0, "LSM303DLHC");
    __delay_ms(2000);
    InitPins();
    ConfigPeriph();
    ConfigInterrupts();
    InitLSM303DLHC();
    ConfigureMag();
    ConfigureAccel();
    while (1) {
//        MagData mag;
//        ReadCompass(&mag);
//        lprintf(0, "x:%d y:%d", mag.xMag, mag.yMag);
//        lprintf(1, "z:%d", mag.zMag);
//        __delay_ms(250);
        AccelData accel;
        ReadAccel(&accel);
        lprintf(0, "x:%d y:%d", accel.xAccel, accel.yAccel);
        lprintf(1, "z:%d", accel.zAccel);
        __delay_ms(250);
    }
}

void InitPins(void) {
    LATD = 0; //LED's are outputs
    TRISD = 0; //Turn off all LED's

    //Set TRIS bits for any required peripherals here.
    TRISB = 0b00000001; //Button0 is input;
}

void ConfigInterrupts(void) {

    RCONbits.IPEN = 0; //no priorities.  This is the default.

    //Configure your interrupts here

    //set up INT0 to interrupt on falling edge
    INTCON2bits.INTEDG0 = 0; //interrupt on falling edge
    INTCONbits.INT0IE = 1; //Enable the interrupt
    //note that we don't need to set the priority because we disabled priorities (and INT0 is ALWAYS high priority when priorities are enabled.)
    INTCONbits.INT0IF = 0; //Always clear the flag before enabling interrupts

    INTCONbits.GIE = 1; //Turn on interrupts
}

void ConfigPeriph(void) {

    //Configure peripherals here

    
}


void __interrupt(high_priority) HighIsr(void) {
    if (INTCONbits.INT0IF == 1) {
        //source is INT0
        
        INTCONbits.INT0IF = 0;
    }
}


