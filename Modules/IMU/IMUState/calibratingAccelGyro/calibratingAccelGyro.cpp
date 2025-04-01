//=====[Libraries]=============================================================

#include "calibratingAccelGyro.h"
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
calibratingAccelGyro::calibratingAccelGyro (IMU * imu) {
    this->imu = imu;
    this->b_verbose = true;
}

/** 
* @brief
* 
* @param 
*/
calibratingAccelGyro::calibratingAccelGyro (IMU * imu, uint8_t mpuI2CAddr) {
    this->imu = imu;
    this->b_verbose = true;
    this->mpuI2CAddr = mpuI2CAddr;
}


/** 
* @brief
* 
* @param 
*/
calibratingAccelGyro::~calibratingAccelGyro ( ) {
    this->imu = NULL;
}

void calibratingAccelGyro::initialize(I2CInterfaceHandler * interface, 
NonBlockingDelay * refresh) {
   
 }

bool calibratingAccelGyro::calibrate (I2CInterfaceHandler * interface, 
NonBlockingDelay * refresh) {

    static int calibrationStep = 0;

        switch (calibrationStep) {
            case 0:
                if (this->set_acc_gyro_to_calibration(interface, refresh) == true) {
                    calibrationStep ++;
                }
                // setear biases en objeto imu
                break;
            case 1:
                this->collect_acc_gyro_data_to(interface);
                this->write_accel_offset(interface);
                this->write_gyro_offset(interface);
                calibrationStep ++;
                break;
            case 2:
                if (this->initMPU9250(interface, refresh)  == true) {
                    calibrationStep ++;
                }
                break;
            case 3:
                this->imu->set_accel_bias ( a_bias[0],  a_bias[1],  a_bias[2] );
                this->imu->set_gyro_bias (g_bias[0],  g_bias[1],  g_bias[2]);
                calibrationStep ++;
                break;
            default:
                this->imu->changeState  (new calibratingMag (this->imu, this->mpuI2CAddr) );
                calibrationStep = 0;
                return false;
    }
    return false;     
}

bool calibratingAccelGyro::obtainInertialData(I2CInterfaceHandler * interface, char * newInertialData) {
    return false;
}

void calibratingAccelGyro::obtainTemperature(I2CInterfaceHandler * interface, float * newTemperature) {
    return;
}

//=====[Implementations of private methods]==================================
bool calibratingAccelGyro::set_acc_gyro_to_calibration(I2CInterfaceHandler * interface, 
NonBlockingDelay * refresh) {
    static int calibrationStep = 0;

   if (refresh->read()) {
        switch (calibrationStep) {
            case 0:
                interface->write_byte(this->mpuI2CAddr, PWR_MGMT_1, 0x80);  // Write a one to bit 7 reset bit; toggle reset device
                calibrationStep ++;
                break;
            case 1:
                interface->write_byte(this->mpuI2CAddr, PWR_MGMT_1, 0x01);
                interface->write_byte(this->mpuI2CAddr, PWR_MGMT_2, 0x00);
                refresh->write(20);
                // refresh->write(200);
                calibrationStep ++;
                break;
            case 2:
                interface->write_byte(this->mpuI2CAddr, INT_ENABLE, 0x00);    // Disable all interrupts
                interface->write_byte(this->mpuI2CAddr, FIFO_EN, 0x00);       // Disable FIFO
                interface->write_byte(this->mpuI2CAddr, PWR_MGMT_1, 0x00);    // Turn on internal clock source
                interface->write_byte(this->mpuI2CAddr, I2C_MST_CTRL, 0x00);  // Disable I2C master
                interface->write_byte(this->mpuI2CAddr, USER_CTRL, 0x00);     // Disable FIFO and I2C master modes
                interface->write_byte(this->mpuI2CAddr, USER_CTRL, 0x0C);     // Reset FIFO and DMP
                refresh->write(15);
                calibrationStep ++;
                break;
            case 3:
                // Configure MPU6050 gyro and accelerometer for bias calculation
                interface->write_byte(this->mpuI2CAddr, MPU_CONFIG, 0x01);    // Set low-pass filter to 188 Hz
                interface->write_byte(this->mpuI2CAddr, SMPLRT_DIV, 0x00);    // Set sample rate to 1 kHz
                interface->write_byte(this->mpuI2CAddr, GYRO_CONFIG, 0x00);   // Set gyro full-scale to 250 degrees per second, maximum sensitivity
                interface->write_byte(this->mpuI2CAddr, ACCEL_CONFIG, 0x00);  // Set accelerometer full-scale to 2 g, maximum sensitivity
                // Configure FIFO to capture accelerometer and gyro data for bias calculation
                interface->write_byte(this->mpuI2CAddr, USER_CTRL, 0x40);  // Enable FIFO
                interface->write_byte(this->mpuI2CAddr, FIFO_EN, 0x78);    // Enable gyro and accelerometer sensors for FIFO  (max size 512 bytes in MPU-9150)
                refresh->write(40);
                calibrationStep ++;
                break;
            default:
                calibrationStep = 0;
                return true;
        }
    }
    return false; 
}



void calibratingAccelGyro::collect_acc_gyro_data_to (I2CInterfaceHandler * interface) {
    // At end of sample accumulation, turn off FIFO sensor read
    uint8_t data[12];                                    // data array to hold accelerometer and gyro x, y, z, data
    interface->write_byte(this->mpuI2CAddr, FIFO_EN, 0x00);             // Disable gyro and accelerometer sensors for FIFO
    interface->read_bytes(this->mpuI2CAddr, FIFO_COUNTH, 2, &data[0]);  // read FIFO sample count
    uint16_t fifo_count = ((uint16_t)data[0] << 8) | data[1];
    uint16_t packet_count = fifo_count / 12;  // How many sets of full gyro and accelerometer data for averaging

    for (uint16_t ii = 0; ii < packet_count; ii++) {
        int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};
        interface->read_bytes(this->mpuI2CAddr, FIFO_R_W, 12, &data[0]);              // read data for averaging
        accel_temp[0] = (int16_t)(((int16_t)data[0] << 8) | data[1]);  // Form signed 16-bit integer for each sample in FIFO
        accel_temp[1] = (int16_t)(((int16_t)data[2] << 8) | data[3]);
        accel_temp[2] = (int16_t)(((int16_t)data[4] << 8) | data[5]);
        gyro_temp[0] = (int16_t)(((int16_t)data[6] << 8) | data[7]);
        gyro_temp[1] = (int16_t)(((int16_t)data[8] << 8) | data[9]);
        gyro_temp[2] = (int16_t)(((int16_t)data[10] << 8) | data[11]);

        a_bias[0] += (float)accel_temp[0];  // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
        a_bias[1] += (float)accel_temp[1];
        a_bias[2] += (float)accel_temp[2];
        g_bias[0] += (float)gyro_temp[0];
        g_bias[1] += (float)gyro_temp[1];
        g_bias[2] += (float)gyro_temp[2];
    }
    a_bias[0] /= (float)packet_count;  // Normalize sums to get average count biases
    a_bias[1] /= (float)packet_count;
    a_bias[2] /= (float)packet_count;
    g_bias[0] /= (float)packet_count;
    g_bias[1] /= (float)packet_count;
    g_bias[2] /= (float)packet_count;

    if (a_bias[2] > 0L) {
        a_bias[2] -= (float)CALIB_ACCEL_SENSITIVITY;
    }  // Remove gravity from the z-axis accelerometer bias calculation
    else {
        a_bias[2] += (float)CALIB_ACCEL_SENSITIVITY;
    }
}



void calibratingAccelGyro::write_accel_offset(I2CInterfaceHandler * interface) {
    // Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
    // factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
    // non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
    // compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
    // the accelerometer biases calculated above must be divided by 8.

    uint8_t read_data[2] = {0};
    int16_t acc_bias_reg[3] = {0, 0, 0};                      // A place to hold the factory accelerometer trim biases
    interface->read_bytes(this->mpuI2CAddr, XA_OFFSET_H, 2, &read_data[0]);  // Read factory accelerometer trim values
    acc_bias_reg[0] = ((int16_t)read_data[0] << 8) | read_data[1];
    interface->read_bytes(this->mpuI2CAddr, YA_OFFSET_H, 2, &read_data[0]);
    acc_bias_reg[1] = ((int16_t)read_data[0] << 8) | read_data[1];
    interface->read_bytes(this->mpuI2CAddr, ZA_OFFSET_H, 2, &read_data[0]);
    acc_bias_reg[2] = ((int16_t)read_data[0] << 8) | read_data[1];

    int16_t mask_bit[3] = {1, 1, 1};  // Define array to hold mask bit for each accelerometer bias axis
    for (int i = 0; i < 3; i++) {
        if (acc_bias_reg[i] % 2) {
            mask_bit[i] = 0;
        }
        acc_bias_reg[i] -= (int16_t) this->a_bias[i] >> 3;  // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g
        if (mask_bit[i]) {
            acc_bias_reg[i] = acc_bias_reg[i] & ~mask_bit[i];  // Preserve temperature compensation bit
        } else {
            acc_bias_reg[i] = acc_bias_reg[i] | 0x0001;  // Preserve temperature compensation bit
        }
    }

    uint8_t write_data[6] = {0};
    write_data[0] = (acc_bias_reg[0] >> 8) & 0xFF;
    write_data[1] = (acc_bias_reg[0]) & 0xFF;
    write_data[2] = (acc_bias_reg[1] >> 8) & 0xFF;
    write_data[3] = (acc_bias_reg[1]) & 0xFF;
    write_data[4] = (acc_bias_reg[2] >> 8) & 0xFF;
    write_data[5] = (acc_bias_reg[2]) & 0xFF;

    // Push accelerometer biases to hardware registers
    interface->write_byte(this->mpuI2CAddr, XA_OFFSET_H, write_data[0]);
    interface->write_byte(this->mpuI2CAddr, XA_OFFSET_L, write_data[1]);
    interface->write_byte(this->mpuI2CAddr, YA_OFFSET_H, write_data[2]);
    interface->write_byte(this->mpuI2CAddr, YA_OFFSET_L, write_data[3]);
    interface->write_byte(this->mpuI2CAddr, ZA_OFFSET_H, write_data[4]);
    interface->write_byte(this->mpuI2CAddr, ZA_OFFSET_L, write_data[5]);
}


void calibratingAccelGyro::write_gyro_offset(I2CInterfaceHandler * interface) {
    // Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
    uint8_t gyro_offset_data[6] {0};
    gyro_offset_data[0] = (-(int16_t) this->g_bias[0] / 4 >> 8) & 0xFF;  // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
    gyro_offset_data[1] = (-(int16_t) this->g_bias[0] / 4) & 0xFF;       // Biases are additive, so change sign on calculated average gyro biases
    gyro_offset_data[2] = (-(int16_t) this->g_bias[1] / 4 >> 8) & 0xFF;
    gyro_offset_data[3] = (-(int16_t) this->g_bias[1] / 4) & 0xFF;
    gyro_offset_data[4] = (-(int16_t) this->g_bias[2] / 4 >> 8) & 0xFF;
    gyro_offset_data[5] = (-(int16_t) this->g_bias[2] / 4) & 0xFF;

    // Push gyro biases to hardware registers
    interface->write_byte(this->mpuI2CAddr, XG_OFFSET_H, gyro_offset_data[0]);
    interface->write_byte(this->mpuI2CAddr, XG_OFFSET_L, gyro_offset_data[1]);
    interface->write_byte(this->mpuI2CAddr, YG_OFFSET_H, gyro_offset_data[2]);
    interface->write_byte(this->mpuI2CAddr, YG_OFFSET_L, gyro_offset_data[3]);
    interface->write_byte(this->mpuI2CAddr, ZG_OFFSET_H, gyro_offset_data[4]);
    interface->write_byte(this->mpuI2CAddr, ZG_OFFSET_L, gyro_offset_data[5]);
}



/** 
* @brief callback function adds 1 to a counter
* @note ISR cannot form part of the class
*/
bool calibratingAccelGyro::initMPU9250(I2CInterfaceHandler * interface, NonBlockingDelay * refresh) {
    static int calibrationStep = 0;
    uint8_t c;
    uint8_t mpu_config;
    uint8_t sample_rate;
    MPU9250Setting setting = this->imu->getMPUSettings();
    float acc_resolution =  imu->get_acc_resolution(setting.accel_fs_sel);
    float gyro_resolution = imu->get_gyro_resolution(setting.gyro_fs_sel);
    float mag_resolution =  imu->get_mag_resolution(setting.mag_output_bits);


    if (refresh->read()) {
        switch (calibrationStep) {
            case 0:
                interface->write_byte(this->mpuI2CAddr, PWR_MGMT_1, 0x80);  // Write a one to bit 7 reset bit; toggle reset device
                refresh->write(10);
                calibrationStep ++;
                break;
            case 1:
                // wake up device
                interface->write_byte(this->mpuI2CAddr, PWR_MGMT_1, 0x00);  // Clear sleep mode bit (6), enable all sensors
                // Wait for all registers to reset
                calibrationStep ++;
                break;
            case 2:
                interface->write_byte(this->mpuI2CAddr, PWR_MGMT_1, 0x01);  // Auto select clock source to be PLL gyroscope reference if ready else
                refresh->write(20);
                calibrationStep ++;
                break;
            case 3:
            // Configure Gyro and Thermometer
            // Disable FSYNC and set thermometer and gyro bandwidth to 41 and 42 Hz, respectively;
            // minimum delay time for this setting is 5.9 ms, which means sensor fusion update rates cannot
            // be higher than 1 / 0.0059 = 170 Hz
            // GYRO_DLPF_CFG = bits 2:0 = 011; this limits the sample rate to 1000 Hz for both
            // With the MPU9250, it is possible to get gyro sample rates of 32 kHz (!), 8 kHz, or 1 kHz
            mpu_config = (uint8_t)setting.gyro_dlpf_cfg;
            interface->write_byte(this->mpuI2CAddr, MPU_CONFIG, mpu_config);

            // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
            sample_rate = (uint8_t)setting.fifo_sample_rate;
            interface->write_byte(this->mpuI2CAddr, SMPLRT_DIV, sample_rate);  // Use a 200 Hz rate; a rate consistent with the filter update rate
                                                                // determined inset in CONFIG above

            // Set gyroscope full scale range
            // Range selects FS_SEL and GFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
            c = interface->read_byte(this->mpuI2CAddr, GYRO_CONFIG);  // get current GYRO_CONFIG register value
            c = c & ~0xE0;                                     // Clear self-test bits [7:5]
            c = c & ~0x03;                                     // Clear Fchoice bits [1:0]
            c = c & ~0x18;                                     // Clear GYRO_FS_SEL bits [4:3]
            c = c | (uint8_t(setting.gyro_fs_sel) << 3);       // Set full scale range for the gyro
            c = c | (uint8_t(~setting.gyro_fchoice) & 0x03);   // Set Fchoice for the gyro
            interface->write_byte(this->mpuI2CAddr, GYRO_CONFIG, c);          // Write new GYRO_CONFIG value to register

            // Set accelerometer full-scale range configuration
            c = interface->read_byte(this->mpuI2CAddr, ACCEL_CONFIG);     // get current ACCEL_CONFIG register value
            c = c & ~0xE0;                                 // Clear self-test bits [7:5]
            c = c & ~0x18;                                 // Clear ACCEL_FS_SEL bits [4:3]
            c = c | (uint8_t(setting.accel_fs_sel) << 3);  // Set full scale range for the accelerometer
            interface->write_byte(this->mpuI2CAddr, ACCEL_CONFIG, c);     // Write new ACCEL_CONFIG register value

            // Set accelerometer sample rate configuration
            // It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
            // accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
            c = interface->read_byte(this->mpuI2CAddr, ACCEL_CONFIG2);        // get current ACCEL_CONFIG2 register value
            c = c & ~0x0F;                                     // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])
            c = c | (~(setting.accel_fchoice << 3) & 0x08);    // Set accel_fchoice_b to 1
            c = c | (uint8_t(setting.accel_dlpf_cfg) & 0x07);  // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz
            interface->write_byte(this->mpuI2CAddr, ACCEL_CONFIG2, c);        // Write new ACCEL_CONFIG2 register value

            // The accelerometer, gyro, and thermometer are set to 1 kHz sample rates,
            // but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting

            // Configure Interrupts and Bypass Enable
            // Set interrupt pin active high, push-pull, hold interrupt pin level HIGH until interrupt cleared,
            // clear on read of INT_STATUS, and enable I2C_BYPASS_EN so additional chips
            // can join the I2C bus and all can be controlled by the Arduino as master
            interface->write_byte(this->mpuI2CAddr, INT_PIN_CFG, 0x22);
            interface->write_byte(this->mpuI2CAddr, INT_ENABLE, 0x01);  // Enable data ready (bit 0) interrupt
            refresh->write(11);
            //  refresh->write(1100);
            calibrationStep ++;
            break;
            case 4:
                refresh->write(11);
                 //  refresh->write(1100);
                calibrationStep ++;
            break;
            case 5:
                uartUSB.write("Mag calibration will start in 5sec\n\r", 
                strlen ("Mag calibration will start in 5sec\n\r")); 
                uartUSB.write("Please Wave device in a figure eight until done\n\r", 
                strlen ("Please Wave device in a figure eight until done\n\r"));
                calibrationStep ++;
                refresh->write(50);
                //  refresh->write(5000);
            break;
            default:
                refresh->write(10);
                //  refresh->write(100);
                calibrationStep = 0;
                return true;
        }
    }
    return false; 
}




