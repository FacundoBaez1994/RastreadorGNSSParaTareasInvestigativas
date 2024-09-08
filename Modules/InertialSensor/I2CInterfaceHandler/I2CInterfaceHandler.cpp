//=====[Libraries]=============================================================

#include "I2CInterfaceHandler.h"

//=====[Declaration of private defines]========================================
#define I2C_SDA PB_7
#define I2C_SCL PB_6 

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============



//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of private methods]===================================
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================


I2CInterfaceHandler::I2CInterfaceHandler ( ) {
    this->i2cInterface =  new I2C (I2C_SDA, I2C_SCL);
    this->i2cInterface->frequency(400000);  // use fast (400 kHz) I2C  
}

/** 
* @brief 
* @returns 
*/
I2CInterfaceHandler::~I2CInterfaceHandler ( ) {
    delete this->i2cInterface; 
    this->i2cInterface = NULL;

}


/** 
* @brief 
* @returns 
*/
void I2CInterfaceHandler::writeByte(uint8_t address, uint8_t subAddress
, uint8_t data) {
    char data_write[2];
   data_write[0] = subAddress;
   data_write[1] = data;
   this->i2cInterface->write(address, data_write, 2, 0);
}

char I2CInterfaceHandler::readByte(uint8_t address, uint8_t subAddress) {
    char data[1]; // `data` will store the register data     
    char data_write[1];
    data_write[0] = subAddress;
    this->i2cInterface->write(address, data_write, 1, 1); // no stop
    this->i2cInterface->read(address, data, 1, 0); 
    return data[0]; 
}

void I2CInterfaceHandler::readBytes(uint8_t address, uint8_t subAddress,
 uint8_t count, uint8_t * dest) {     
    char data[14];
    char data_write[1];
    data_write[0] = subAddress;
    this->i2cInterface->write(address, data_write, 1, 1); // no stop
    this->i2cInterface->read(address, data, count, 0); 
    for(int ii = 0; ii < count; ii++) {
     dest[ii] = data[ii];
    }
} 



//=====[Implementations of private functions]==================================
/** 
* @brief callback function adds 1 to a counter
* @note ISR cannot form part of the class
*/


/** 
* @brief returns counter
* @return tickCounter a counter value
*/
