# LSM303DLHC
A 3-axis accelerometer and 3-axis compass with I2C interface.  
The acclerometer and compass each have their own I2C address.  
The accelerometer supports free-fall and click interrupts.  
Supports 6D position and motion interrupt.   

## Important notes
* DRDY pin is for mag sensor only
* Mag sensor and temperature values are big endian but accelerometer values are little endian
* Look at AN4825 for more information on usage.  This application note is for the LSM303AGR which is very similar (but not identical)

## Suggested improvements/additions
* Implement click or free-fall interrupts
* Implement 6D and/or 4D position interrupt
* Implement high pass filter.
* Implement compass heading
* Output compass values in appropriate units based on gain 