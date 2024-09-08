//=====[#include guards - begin]===============================================
#ifndef I2C_INTERFACE_HANDLER_H
#define I2C_INTERFACE_HANDLER_H
 
//==================[Libraries]===============================================

#include "mbed.h"
#include "math.h"
#include "Non_Blocking_Delay.h"

//=====[Declaration of public data types]======================================


//=====[Declaration of public classes]=========================================

class I2CInterfaceHandler {
    //=====[Declaration of public methods]=========================================
public:
    I2CInterfaceHandler ();
    virtual ~I2CInterfaceHandler ();

    // mover a I2C interface handler
    void writeByte(uint8_t address, uint8_t subAddress, uint8_t data);
    char readByte(uint8_t address, uint8_t subAddress);
    void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest);
       

 private:
    //=====[Declaration of private methods]=========================================

    //=====[Declaration of private atributtes]=========================================
    I2C * i2cInterface;

};

//  I2C_INTERFACE_HANDLER_H
#endif