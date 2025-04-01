//=====[Libraries]=============================================================

#include "initializeMPU9250.h"
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
initializeMPU9250::initializeMPU9250 (IMU * imu) {
    this->imu = imu;
    this->b_verbose = true;

    MPU9250Setting setting = this->imu->getMPUSettings();

    this->acc_resolution = imu->get_acc_resolution(setting.accel_fs_sel);
    this->gyro_resolution = imu->get_gyro_resolution(setting.gyro_fs_sel);
    this->mag_resolution = imu->get_mag_resolution(setting.mag_output_bits);
    this->mpu_config = (uint8_t)setting.gyro_dlpf_cfg;
    this->sample_rate = (uint8_t)setting.fifo_sample_rate;
    this->gyro_fchoice = (uint8_t)setting.gyro_fchoice;
    this->accel_dlpf_cfg = setting.accel_dlpf_cfg;
    this->accel_fchoice = setting.accel_fchoice;
}

/** 
* @brief
* 
* @param 
*/
initializeMPU9250::~initializeMPU9250 ( ) {
    this->imu = NULL;
}

void initializeMPU9250::initialize(I2CInterfaceHandler * interface, 
NonBlockingDelay * refresh) {
    static bool firstCheckPass = false;

    if (firstCheckPass == false) { 
        uint8_t addr = this->imu->getMPUI2CAddress ();
        // addr should be valid for MPU
        if ((addr < MPU9250_DEFAULT_ADDRESS) || (addr > MPU9250_DEFAULT_ADDRESS + 7)) {
            char error_msg[80];
            sprintf(error_msg, "I2C address 0x%X is not valid for MPU. Please check your I2C address.\n", addr);
            uartUSB.write(error_msg, strlen(error_msg));
            return;
        }
        this->mpuI2CAddr = addr;

        if (! this->isConnectedMPU9250(interface)) {
            if (b_verbose) {
                const char* error_msg = "Could not connect to MPU9250\n";
                uartUSB.write(error_msg, strlen(error_msg));
            }
            return;
        }
        firstCheckPass = true; 
    }
    if (this->initMPU9250(interface, refresh)) {
        firstCheckPass = false; 

        char StringToSendUSB [50] = "Initializing Magnetometer";
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only}
        // Go to sleep
        this->imu->changeState  (new initializeAK8963 (this->imu, this->mpuI2CAddr));
        return;
    }
    return;
 }

bool initializeMPU9250::calibrate (I2CInterfaceHandler * interface, 
NonBlockingDelay * refresh) {
    return false;
}

bool initializeMPU9250::obtainInertialData(I2CInterfaceHandler * interface, char * newInertialData) {
    return false;
}

void initializeMPU9250::obtainTemperature(I2CInterfaceHandler * interface, float * newTemperature) {
    return;
}

//=====[Implementations of private methods]==================================

bool initializeMPU9250::isConnectedMPU9250(I2CInterfaceHandler * interface) {
    char c = interface->read_byte(this->mpuI2CAddr, WHO_AM_I_MPU9250);
    
    if (this->b_verbose) {
        // Mensaje de depuración
        char msg[50];
        sprintf(msg, "MPU9250 WHO AM I = 0x%02X\n", c);
        uartUSB.write(msg, strlen(msg));  // Enviar mensaje por UART
    }
    
    bool b = (c == MPU9250_WHOAMI_DEFAULT_VALUE);
    b |= (c == MPU9255_WHOAMI_DEFAULT_VALUE);
    b |= (c == MPU6500_WHOAMI_DEFAULT_VALUE);
    return b;
}


/** 
* @brief callback function adds 1 to a counter
* @note ISR cannot form part of the class
*/
bool initializeMPU9250::initMPU9250(I2CInterfaceHandler * interface, NonBlockingDelay * refresh) {
    static bool resetToggled = false;
    static int initializationStep = 0;   
    uint8_t c;

    if (resetToggled == false) {
        // reset device
        interface->write_byte(this->mpuI2CAddr, PWR_MGMT_1, 0x80);  // Write a one to bit 7 reset bit; toggle reset device
        resetToggled = true;
        refresh->write(100);
        // refresh->write(100); antes
    }

     if (refresh->read()) {
        switch (initializationStep) {
            case 0:
                interface->write_byte(this->mpuI2CAddr, PWR_MGMT_1, 0x00);  // Clear sleep mode bit (6), enable all sensors
                initializationStep ++;
                break;
            case 1:
                interface->write_byte(this->mpuI2CAddr, PWR_MGMT_1, 0x01);  // Auto select clock source to be PLL gyroscope reference if ready else
                initializationStep ++;
                //refresh->write(20); anterior
                refresh->write(200);
                break;
            case 2:
                // Configure Gyro and Thermometer
                // Disable FSYNC and set thermometer and gyro bandwidth to 41 and 42 Hz, respectively;
                // minimum delay time for this setting is 5.9 ms, which means sensor fusion update rates cannot
                // be higher than 1 / 0.0059 = 170 Hz
                // GYRO_DLPF_CFG = bits 2:0 = 011; this limits the sample rate to 1000 Hz for both
                // With the MPU9250, it is possible to get gyro sample rates of 32 kHz (!), 8 kHz, or 1 kHz
                
                //uint8_t mpu_config = (uint8_t)setting.gyro_dlpf_cfg; ELIMINAR
                interface->write_byte(this->mpuI2CAddr, MPU_CONFIG, this->mpu_config);

                // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
                // uint8_t sample_rate = (uint8_t)setting.fifo_sample_rate; eliminiar
                interface->write_byte(this->mpuI2CAddr, SMPLRT_DIV, this->sample_rate);  // Use a 200 Hz rate; a rate consistent with the filter update rate
                                                                    // determined inset in CONFIG above

                // Set gyroscope full scale range
                // Range selects FS_SEL and GFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
                c = interface->read_byte(this->mpuI2CAddr, GYRO_CONFIG);  // get current GYRO_CONFIG register value
                c = c & ~0xE0;                                     // Clear self-test bits [7:5]
                c = c & ~0x03;                                     // Clear Fchoice bits [1:0]
                c = c & ~0x18;                                     // Clear GYRO_FS_SEL bits [4:3]
                c = c | (uint8_t(this->gyro_resolution) << 3);       // Set full scale range for the gyro
                c = c | (uint8_t(~this->gyro_fchoice) & 0x03);   // Set Fchoice for the gyro
                interface->write_byte(this->mpuI2CAddr, GYRO_CONFIG, c);          // Write new GYRO_CONFIG value to register

                // Set accelerometer full-scale range configuration
                c = interface->read_byte(this->mpuI2CAddr, ACCEL_CONFIG);     // get current ACCEL_CONFIG register value
                c = c & ~0xE0;                                 // Clear self-test bits [7:5]
                c = c & ~0x18;                                 // Clear ACCEL_FS_SEL bits [4:3]
                c = c | (uint8_t(this->acc_resolution) << 3);  // Set full scale range for the accelerometer
                interface->write_byte(this->mpuI2CAddr, ACCEL_CONFIG, c);     // Write new ACCEL_CONFIG register value

                // Set accelerometer sample rate configuration
                // It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
                // accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
                c = interface->read_byte(this->mpuI2CAddr, ACCEL_CONFIG2);        // get current ACCEL_CONFIG2 register value
                c = c & ~0x0F;                                     // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])
                c = c | (~(this->accel_fchoice << 3) & 0x08);    // Set accel_fchoice_b to 1
                c = c | (uint8_t(this->accel_dlpf_cfg) & 0x07);  // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz
                interface->write_byte(this->mpuI2CAddr, ACCEL_CONFIG2, c);        // Write new ACCEL_CONFIG2 register value

                // The accelerometer, gyro, and thermometer are set to 1 kHz sample rates,
                // but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting

                // Configure Interrupts and Bypass Enable
                // Set interrupt pin active high, push-pull, hold interrupt pin level HIGH until interrupt cleared,
                // clear on read of INT_STATUS, and enable I2C_BYPASS_EN so additional chips
                // can join the I2C bus and all can be controlled by the Arduino as master
                interface->write_byte(this->mpuI2CAddr, INT_PIN_CFG, 0x22);
                interface->write_byte(this->mpuI2CAddr, INT_ENABLE, 0x01);  // Enable data ready (bit 0) interrupt
                initializationStep ++;
                break;

            default:
                initializationStep = 0;
                return true;
        }
    }
    return false; 

}


