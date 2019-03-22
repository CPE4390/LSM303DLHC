
#include <xc.h>
#include "LSM303DLHC.h"

#define _XTAL_FREQ  32000000L

void InitLSM303DLHC(void) {
    TRISD = 0b01100000; //MMSP2 uses RD5 as SDA, RD6 as SCL, both set as inputs
    SSP2ADD = 0x19; //400kHz
    SSP2CON1bits.SSPM = 0b1000; //I2C Master mode
    SSP2CON1bits.SSPEN = 1; //Enable MSSP
}

void ConfigureAccel(void) {
    I2CWriteRegister(ACCEL_ADDRESS, 0x20, 0b01000111); //50 Hz
    I2CWriteRegister(ACCEL_ADDRESS, 0x22, 0b00010000); //DRDY on Int1
}

void ConfigureMag(void) {
    
}

char ReadCompass(MagData *value) {
    unsigned char *ptr = (unsigned char *)value;
    I2CWriteRegister(MAG_ADDRESS, 0x02, 0b00000001); //Single conversion
    while (!(I2CReadRegister(MAG_ADDRESS, 0x09) & 1));  //Wait for DRDY
    char result = ReadMagData(0x03, ptr, 6);
    //data from Mag sensor is big endian.  Need to swap bytes to make it little endian
    unsigned char temp;
    temp = ptr[0];
    ptr[0] = ptr[1];
    ptr[1] = temp;
    temp = ptr[2];
    ptr[2] = ptr[3];
    ptr[3] = temp;
    temp = ptr[4];
    ptr[4] = ptr[5];
    ptr[5] = temp;
    return result;
}

char ReadAccel(AccelData *value) {
    char result = ReadAccelData(0x29, (unsigned char *)value, 6);
    return result;
}

unsigned char I2CReadRegister(unsigned char i2cAddress, unsigned char reg) {
    unsigned char data;
    SSP2CON2bits.SEN = 1; //Start condition
    while (SSP2CON2bits.SEN == 1); //Wait for start to finish
    SSP2BUF = i2cAddress << 1; //address with R/W clear for write
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2BUF = reg; //Send register
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2CON2bits.RSEN = 1; //Restart condition
    while (SSP2CON2bits.RSEN == 1); //Wait for restart to finish
    SSP2BUF = (i2cAddress << 1) + 1; //address with R/W set for read
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2CON2bits.RCEN = 1; // enable master for 1 byte reception
    while (!SSP2STATbits.BF); // wait until byte received
    data = SSP2BUF;
    SSP2CON2bits.ACKDT = 1;
    SSP2CON2bits.ACKEN = 1; //Send ACK/NACK
    while (SSP2CON2bits.ACKEN != 0);
    SSP2CON2bits.PEN = 1; //Stop condition
    while (SSP2CON2bits.PEN == 1); //Wait for stop to finish
    return data;
}

void I2CWriteRegister(unsigned char i2cAddress, unsigned char reg, unsigned int data) {
    SSP2CON2bits.SEN = 1; //Start condition
    while (SSP2CON2bits.SEN == 1); //Wait for start to finish
    SSP2BUF = i2cAddress << 1; //address with R/W clear for write
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2BUF = reg; //Send register
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2BUF = data; //Send byte
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2CON2bits.PEN = 1; //Stop condition
    while (SSP2CON2bits.PEN == 1); //Wait for stop to finish
}
    
char ReadAccelData(unsigned char reg, unsigned char *data, int len) {
    int i;
    SSP2CON2bits.SEN = 1; //Start condition
    while (SSP2CON2bits.SEN == 1); //Wait for start to finish
    SSP2BUF = ACCEL_ADDRESS << 1; //address with R/W clear for write
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    if (SSP2CON2bits.ACKSTAT != 0) {  
        return 0;  //didn't get ACK from slave 
    }
    SSP2BUF = reg | 0b10000000; //Send register with bit 8 set for auto increment
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2CON2bits.RSEN = 1; //Restart condition
    while (SSP2CON2bits.RSEN == 1); //Wait for restart to finish
    SSP2BUF = (ACCEL_ADDRESS << 1) + 1; //address with R/W set for read
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    for (i = 0; i < len; ++i) {
        SSP2CON2bits.RCEN = 1; // enable master for 1 byte reception
        while (!SSP2STATbits.BF); // wait until byte received
        data[i] = SSP2BUF;
        if (i == len - 1) {
            SSP2CON2bits.ACKDT = 1;
        } else {
            SSP2CON2bits.ACKDT = 0;
        }
        SSP2CON2bits.ACKEN = 1; //Send ACK/NACK
        while (SSP2CON2bits.ACKEN != 0);
    }
    SSP2CON2bits.PEN = 1; //Stop condition
    while (SSP2CON2bits.PEN == 1); //Wait for stop to finish
    return len;
}
    
char ReadMagData(unsigned char reg, unsigned char *data, int len) {
    int i;
    SSP2CON2bits.SEN = 1; //Start condition
    while (SSP2CON2bits.SEN == 1); //Wait for start to finish
    SSP2BUF = MAG_ADDRESS << 1; //address with R/W clear for write
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    if (SSP2CON2bits.ACKSTAT != 0) {  
        return 0;  //didn't get ACK from slave 
    }
    SSP2BUF = reg; //Send register
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    SSP2CON2bits.RSEN = 1; //Restart condition
    while (SSP2CON2bits.RSEN == 1); //Wait for restart to finish
    SSP2BUF = (MAG_ADDRESS << 1) + 1; //address with R/W set for read
    while (SSP2STATbits.BF || SSP2STATbits.R_W); // wait until write cycle is complete
    for (i = 0; i < len; ++i) {
        SSP2CON2bits.RCEN = 1; // enable master for 1 byte reception
        while (!SSP2STATbits.BF); // wait until byte received
        data[i] = SSP2BUF;
        if (i == len - 1) {
            SSP2CON2bits.ACKDT = 1;
        } else {
            SSP2CON2bits.ACKDT = 0;
        }
        SSP2CON2bits.ACKEN = 1; //Send ACK/NACK
        while (SSP2CON2bits.ACKEN != 0);
    }
    SSP2CON2bits.PEN = 1; //Stop condition
    while (SSP2CON2bits.PEN == 1); //Wait for stop to finish
    return len;
}
