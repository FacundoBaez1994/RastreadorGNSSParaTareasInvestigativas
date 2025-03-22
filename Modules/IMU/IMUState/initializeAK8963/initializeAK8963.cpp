//=====[Libraries]=============================================================

#include "initializeAK8963.h"
#include "IMU.h" //debido a declaracion adelantada
//#include "FormattingMessage.h"

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
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


/** 
* @brief
* 
* @param 
*/
initializeAK8963::initializeAK8963 (IMU * imu) {
    this->imu = imu;
    this->b_verbose = true;
}

initializeAK8963::initializeAK8963 (IMU * imu, uint8_t mpuI2CAddr) {
    this->imu = imu;
    this->b_verbose = true;
    this->mpuI2CAddr = mpuI2CAddr;
}

/** 
* @brief
* 
* @param 
*/
initializeAK8963::~initializeAK8963 ( ) {
    this->imu = NULL;
}

void initializeAK8963::initialize(I2CInterfaceHandler * interface, 
NonBlockingDelay * refresh) {
    static bool firstCheckPass = false;

    if (firstCheckPass == false) { 
        if (! this->isConnectedAK8963(interface)) {
            if (b_verbose) {
                const char* error_msg = "Could not connect to AK8963\n";
                uartUSB.write(error_msg, strlen(error_msg));
            }
            return;
        }
        firstCheckPass = true; 
    }
    if (this->initAK8963(interface, refresh)) {
        firstCheckPass = false; 

        this->imu->set_mag_bias_factory(this->mag_bias_factory [0], 
        this->mag_bias_factory [1], this->mag_bias_factory [2]);
        char StringToSendUSB [50] = "Magnetometer ok";
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only}
        // Go to sleep
        //this->tracker->changeState  (new GoingToSleep (this->tracker));
        uartUSB.write("Accel Gyro calibration will start in 5sec\n\r", 
        strlen ("Accel Gyro calibration will start in 5sec\n\r")); 
        uartUSB.write("Please leave the device still on the flat plane\n\r", 
        strlen ("Please leave the device still on the flat plane\n\r"));
        wait_us(5000000);
        this->imu->changeState  (new calibratingAccelGyro (this->imu, this->mpuI2CAddr) );
        return;
    }
    return;
 }

void initializeAK8963::calibrate (I2CInterfaceHandler * interface, 
NonBlockingDelay * refresh) {

}

bool initializeAK8963::obtainInertialData(I2CInterfaceHandler * interface, char * newInertialData) {
    return false;
}

void initializeAK8963::obtainTemperature(I2CInterfaceHandler * interface, float * newTemperature) {
    return;
}


//=====[Implementations of private methods]==================================
bool initializeAK8963::isConnectedAK8963(I2CInterfaceHandler * interface) {
    char c = interface->read_byte(AK8963_ADDRESS, AK8963_WHO_AM_I);
    
    if (this->b_verbose) {
        // Mensaje de depuración
        char msg[50];
        sprintf(msg, "AK8963 WHO AM I = 0x%02X\n", c);
        uartUSB.write(msg, strlen(msg));  // Enviar mensaje por UART
    }
    
    return (c == AK8963_WHOAMI_DEFAULT_VALUE);
}


/** 
* @brief
* @note
*/
bool initializeAK8963::initAK8963(I2CInterfaceHandler * interface, NonBlockingDelay * refresh) {
    static int initializationStep = 0;
    uint8_t raw_data[3];                            // x/y/z gyro calibration data stored here
    MPU9250Setting setting = this->imu->getMPUSettings();

     if (refresh->read()) {
        switch (initializationStep) {
            case 0:
                interface->write_byte(AK8963_ADDRESS, AK8963_CNTL, 0x00);  // Power down magnetometer
                initializationStep ++;
                break;
            case 1:
                interface->write_byte(AK8963_ADDRESS, AK8963_CNTL, 0x0F);  // Enter Fuse ROM access mode
                initializationStep ++;
                refresh->write(10);
                break;
            case 2:
                interface->read_bytes(AK8963_ADDRESS, AK8963_ASAX, 3, &raw_data[0]);      // Read the x-, y-, and z-axis calibration values
                this->mag_bias_factory[0] = (float)(raw_data[0] - 128) / 256. + 1.;  // Return x-axis sensitivity adjustment values, etc.
                this->mag_bias_factory[1] = (float)(raw_data[1] - 128) / 256. + 1.;
                this->mag_bias_factory[2] = (float)(raw_data[2] - 128) / 256. + 1.;
                interface->write_byte(AK8963_ADDRESS, AK8963_CNTL, 0x00);  // Power down magnetometer
                initializationStep ++;
                break;
            case 3:
                interface->write_byte(AK8963_ADDRESS, AK8963_CNTL, (uint8_t)setting.mag_output_bits << 4 | MAG_MODE);  // Set magnetometer data resolution and sample 
                initializationStep ++;
                break;
            case 4:
                if (b_verbose) {
                uartUSB.write("Mag Factory Calibration Values: \n\r", 30);
                uartUSB.write("X-Axis sensitivity offset value ", 31);
                char buffer[10];
                sprintf(buffer, "%.2f", mag_bias_factory[0]);
                uartUSB.write(buffer, strlen(buffer));
                uartUSB.write("\n\r", 2);
                
                uartUSB.write("Y-Axis sensitivity offset value ", 31);
                sprintf(buffer, "%.2f", mag_bias_factory[1]);
                uartUSB.write(buffer, strlen(buffer));
                uartUSB.write("\n\r", 2);
                
                uartUSB.write("Z-Axis sensitivity offset value ", 31);
                sprintf(buffer, "%.2f", mag_bias_factory[2]);
                uartUSB.write(buffer, strlen(buffer));
                uartUSB.write("\n\r", 2);
            }
                initializationStep ++;
                break;
            default:
                initializationStep = 0;
                return true;
        }
    }
    return false; 

}


