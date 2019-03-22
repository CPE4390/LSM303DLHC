
#ifndef LSM303DLHC_H
#define	LSM303DLHC_H

#ifdef	__cplusplus
extern "C" {
#endif
#define ACCEL_ADDRESS   0x19
#define MAG_ADDRESS     0x1e
        
    typedef struct {
        int xMag;
        int zMag;
        int yMag;
    } MagData;
    
    typedef struct {
        int xAccel;
        int yAccel;
        int zAccel;
    } AccelData;
    
    void InitLSM303DLHC(void);
    unsigned char I2CReadRegister(unsigned char i2cAddress, unsigned char reg);
    void I2CWriteRegister(unsigned char i2cAddress, unsigned char reg, unsigned int data);
    char ReadAccelData(unsigned char reg, unsigned char *data, int len);
    char ReadMagData(unsigned char reg, unsigned char *data, int len);
    void ConfigureAccel(void);
    void ConfigureMag(void);
    char ReadCompass(MagData *value);
    char ReadAccel(AccelData *value);
    
#ifdef	__cplusplus
}
#endif

#endif

