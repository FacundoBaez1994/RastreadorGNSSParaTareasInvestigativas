//=====[Libraries]=============================================================

#include "I2CInterfaceHandler.h"
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define I2C_SDA_DEFAULT PA_10
#define I2C_SCL_DEFAULT PA_9 
#define I2C_FREQUENCY_DEFAULT 400000
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
    this->i2cInterface =  new I2C (I2C_SDA_DEFAULT,  I2C_SCL_DEFAULT);
    this->i2cInterface->frequency(I2C_FREQUENCY_DEFAULT);  // use fast (400 kHz) I2C  
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
void I2CInterfaceHandler::write_byte(uint8_t address, uint8_t subAddress, uint8_t data) {
    char data_out[2] = {static_cast<char>(subAddress), static_cast<char>(data)};
    i2c_err_ = i2cInterface->write(address << 1, data_out, 2);  // Address shifted left for 8-bit format
    if (i2c_err_) print_i2c_error();
}

uint8_t I2CInterfaceHandler::read_byte(uint8_t address, uint8_t subAddress) {
    char subAddr = static_cast<char>(subAddress);
    char data = 0;

    i2c_err_ = i2cInterface->write(address << 1, &subAddr, 1, true);  // Write subAddress and keep connection open
    if (i2c_err_) {
        print_i2c_error();
        return 0;
    }

    i2c_err_ = i2cInterface->read(address << 1, &data, 1);  // Read one byte
    if (i2c_err_) {
        print_i2c_error();
        return 0;
    }

    return static_cast<uint8_t>(data);
}

void I2CInterfaceHandler::read_bytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t* dest) {
    char subAddr = static_cast<char>(subAddress);

    i2c_err_ = i2cInterface->write(address << 1, &subAddr, 1, true);  // Write subAddress and keep connection open
    if (i2c_err_) {
        print_i2c_error();
        return;
    }

    i2c_err_ = i2cInterface->read(address << 1, reinterpret_cast<char*>(dest), count);  // Read `count` bytes
    if (i2c_err_) {
        print_i2c_error();
    }
}

void I2CInterfaceHandler::print_i2c_error() {
    if (i2c_err_) {
        char buffer[32];
        int len = sprintf(buffer, "I2C ERROR CODE: %d\n", i2c_err_);
        uartUSB.write(buffer, len);  // Send error message through UART
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
