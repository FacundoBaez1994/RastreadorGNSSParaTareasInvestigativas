//=====[Libraries]=============================================================

#include "calibratingMag.h"
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
calibratingMag::calibratingMag (IMU * imu) {
    this->imu = imu;
    this->b_verbose = true;
}

/** 
* @brief
* 
* @param 
*/
calibratingMag::calibratingMag (IMU * imu, uint8_t mpuI2CAddr) {
    this->imu = imu;
    this->b_verbose = true;
    this->mpuI2CAddr = mpuI2CAddr;
}


/** 
* @brief
* 
* @param 
*/
calibratingMag::~calibratingMag ( ) {
    this->imu = NULL;
}

void calibratingMag::initialize(I2CInterfaceHandler * interface, 
NonBlockingDelay * refresh) {
  

 }

void calibratingMag::calibrate (I2CInterfaceHandler * interface, 
NonBlockingDelay * refresh) {
    MPU9250Setting setting = this->imu->getMPUSettings();
      // set MAG_OUTPUT_BITS to maximum to calibrate
    MAG_OUTPUT_BITS mag_output_bits_cache = setting.mag_output_bits;
    setting.mag_output_bits = MAG_OUTPUT_BITS::M16BITS;


    static int calibrationStep = 0;

    switch (calibrationStep) {
        case 0:
            if (this->initAK8963(interface, refresh) == true) {
                calibrationStep ++;
            }
            break;
        case 1:       
            if (this->collect_mag_data_to(interface, refresh) == true) {

                if (this->b_verbose) {
                    uartUSB.write("Mag Calibration done!\n\r", 22);

                    uartUSB.write("AK8963 mag biases (mG)\n\r", 24);
                    char buffer[10];
                    for (int i = 0; i < 3; i++) {
                        sprintf(buffer, "%.2f", mag_bias[i]);
                        uartUSB.write(buffer, strlen(buffer));
                        if (i < 2) uartUSB.write(", ", 2);  // Añadir coma entre los valores
                    }
                    uartUSB.write("\n\r", 2);

                    uartUSB.write("AK8963 mag scale (mG)\n\r", 24);
                    for (int i = 0; i < 3; i++) {
                        sprintf(buffer, "%.2f", mag_scale[i]);
                        uartUSB.write(buffer, strlen(buffer));
                        if (i < 2) uartUSB.write(", ", 2);  // Añadir coma entre los valores
                    }
                    uartUSB.write("\n\r", 2);
                }

                // restore MAG_OUTPUT_BITS
                setting.mag_output_bits = mag_output_bits_cache;
                calibrationStep ++;
                }
            break;
        case 2:
            if (this->initAK8963(interface, refresh) == true) {
                calibrationStep ++;
            }
            break;
        case 3:
            this->imu->set_mag_bias_factory (mag_bias_factory[0], mag_bias_factory[1], mag_bias_factory[2]);
            this->imu->set_mag_bias (mag_bias [0], mag_bias [1], mag_bias [2]);
            this->imu->set_mag_scale (mag_scale [0], mag_scale [1], mag_scale [2]);
            calibrationStep ++;
            break;
        default:
            this->imu->changeState  (new acquiringData (this->imu, this->mpuI2CAddr));
            //this->imu->changeState  (new calibratingMag (this->imu, this->mpuI2CAddr) );
            calibrationStep = 0;
            return;
    }
    return;
}

bool calibratingMag::obtainInertialData(I2CInterfaceHandler * interface, char * newInertialData) {
    return false;
}

void calibratingMag::obtainTemperature(I2CInterfaceHandler * interface, float * newTemperature) {
    return;
}



//=====[Implementations of private methods]==================================

/** 
* @brief
* @note
*/
bool calibratingMag::initAK8963(I2CInterfaceHandler * interface, NonBlockingDelay * refresh) {
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

bool calibratingMag::collect_mag_data_to(I2CInterfaceHandler * interface, NonBlockingDelay * refresh) {
    static bool firstEntryToMethod = true;
    static int state = 0;
    static uint16_t sample_count = 0;
    static int16_t mag_max[3] = {-32767, -32767, -32767};
    static int16_t mag_min[3] = {32767, 32767, 32767};
    static int16_t mag_temp[3] = {0, 0, 0};

    if (firstEntryToMethod) {
        firstEntryToMethod = false;
        if (this->b_verbose)
            uartUSB.write("Mag Calibration: Wave device in a figure eight until done!\n\r", 
                          strlen("Mag Calibration: Wave device in a figure eight until done!\n\r"));
        refresh->write(200);
        //refresh->write(4000);
        state = 0;
    }

    if (refresh->read()) {
        switch (state) {
            case 0:
                refresh->write(1);
                state = 1;
                break;

            case 1:
                sample_count = (MAG_MODE == 0x02) ? 128 : 1500;
                refresh->write((MAG_MODE == 0x02) ? 135 : 12);
                state = 2;
                break;

            case 2:
                if (sample_count > 0) {
                    this->read_mag(mag_temp, interface);
                    for (int jj = 0; jj < 3; jj++) {
                        if (mag_temp[jj] > mag_max[jj]) mag_max[jj] = mag_temp[jj];
                        if (mag_temp[jj] < mag_min[jj]) mag_min[jj] = mag_temp[jj];
                    }
                    sample_count--;
                    refresh->write((MAG_MODE == 0x02) ? 135 : 12);
                } else {
                    state = 3;
                }
                break;

            case 3:
                int32_t bias[3] = {
                    (mag_max[0] + mag_min[0]) / 2,
                    (mag_max[1] + mag_min[1]) / 2,
                    (mag_max[2] + mag_min[2]) / 2
                };
                float bias_resolution = this->imu->get_mag_resolution(MAG_OUTPUT_BITS::M16BITS);
                this->mag_bias[0] = (float)bias[0] * bias_resolution * mag_bias_factory[0];
                this->mag_bias[1] = (float)bias[1] * bias_resolution * mag_bias_factory[1];
                this->mag_bias[2] = (float)bias[2] * bias_resolution * mag_bias_factory[2];

                float scale[3] = {
                    (float)(mag_max[0] - mag_min[0]) * mag_bias_factory[0] / 2,
                    (float)(mag_max[1] - mag_min[1]) * mag_bias_factory[1] / 2,
                    (float)(mag_max[2] - mag_min[2]) * mag_bias_factory[2] / 2
                };
                float avg_rad = (scale[0] + scale[1] + scale[2]) / 3.0;
                this->mag_scale[0] = avg_rad / scale[0];
                this->mag_scale[1] = avg_rad / scale[1];
                this->mag_scale[2] = avg_rad / scale[2];
                state = 4;
                return true;
        }
    }
    return false;
}

bool calibratingMag::read_mag(int16_t* destination, I2CInterfaceHandler * interface) {
    const uint8_t st1 = interface->read_byte(AK8963_ADDRESS, AK8963_ST1);
    if (st1 & 0x01) {                                                    // wait for magnetometer data ready bit to be set
        uint8_t raw_data[7];                                             // x/y/z gyro register data, ST2 register stored here, must read ST2 at end of data acquisition
        interface->read_bytes(AK8963_ADDRESS, AK8963_XOUT_L, 7, &raw_data[0]);      // Read the six raw data and ST2 registers sequentially into data array
        if (MAG_MODE == 0x02 || MAG_MODE == 0x04 || MAG_MODE == 0x06) {  // continuous or external trigger read mode
            if ((st1 & 0x02) != 0)                                       // check if data is not skipped
                return false;                                            // this should be after data reading to clear DRDY register
        }

        uint8_t c = raw_data[6];                                         // End data read by reading ST2 register
        if (!(c & 0x08)) {                                               // Check if magnetic sensor overflow set, if not then report data
            destination[0] = ((int16_t)raw_data[1] << 8) | raw_data[0];  // Turn the MSB and LSB into a signed 16-bit value
            destination[1] = ((int16_t)raw_data[3] << 8) | raw_data[2];  // Data stored as little Endian
            destination[2] = ((int16_t)raw_data[5] << 8) | raw_data[4];
            return true;
        }
    }
    return false;
}