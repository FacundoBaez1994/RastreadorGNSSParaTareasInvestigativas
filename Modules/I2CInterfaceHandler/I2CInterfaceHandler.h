//=====[#include guards - begin]===============================================
#ifndef I2C_INTERFACE_HANDLER_H
#define I2C_INTERFACE_HANDLER_H
 
//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"

//=====[Declaration of public data types]======================================


//=====[Declaration of public classes]=========================================

class I2CInterfaceHandler {
    //=====[Declaration of public methods]=========================================
public:
    I2CInterfaceHandler ();
    virtual ~I2CInterfaceHandler ();
    void write_byte(uint8_t address, uint8_t subAddress, uint8_t data);
    uint8_t read_byte(uint8_t address, uint8_t subAddress);
    void read_bytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t* dest);
    void print_i2c_error();
       

 private:
    //=====[Declaration of private methods]=========================================

    //=====[Declaration of private atributtes]=========================================
    I2C * i2cInterface;
    uint8_t i2c_err_;

};

//  I2C_INTERFACE_HANDLER_H
#endif