 //=====[Libraries]=============================================================
 #include "InertialSensor.h"
 #include "Debugger.h"

//=====[Declaration of private defines]========================================
#define REFRESHTIME  100

#define I2C_SDA PB_7 // eliminar delete
#define I2C_SCL PB_6 // eliminar delete

// See also MPU-9250 Register Map and Descriptions, Revision 4.0, RM-MPU-9250A-00, Rev. 1.4, 9/9/2013 for registers not listed in 
// above document; the MPU9250 and MPU9150 are virtually identical but the latter has a different register map
//
//Magnetometer Registers
#define AK8963_ADDRESS   0x0C<<1
#define AK8963_WHO_AM_I  0x00 // should return 0x48
#define AK8963_INFO      0x01
#define AK8963_ST1       0x02  // data ready status bit 0
#define AK8963_XOUT_L    0x03  // data
#define AK8963_XOUT_H    0x04
#define AK8963_YOUT_L    0x05
#define AK8963_YOUT_H    0x06
#define AK8963_ZOUT_L    0x07
#define AK8963_ZOUT_H    0x08
#define AK8963_ST2       0x09  // Data overflow bit 3 and data read error status bit 2
#define AK8963_CNTL      0x0A  // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
#define AK8963_ASTC      0x0C  // Self test control
#define AK8963_I2CDIS    0x0F  // I2C disable
#define AK8963_ASAX      0x10  // Fuse ROM x-axis sensitivity adjustment value
#define AK8963_ASAY      0x11  // Fuse ROM y-axis sensitivity adjustment value
#define AK8963_ASAZ      0x12  // Fuse ROM z-axis sensitivity adjustment value

#define SELF_TEST_X_GYRO 0x00                  
#define SELF_TEST_Y_GYRO 0x01                                                                          
#define SELF_TEST_Z_GYRO 0x02

/*#define X_FINE_GAIN      0x03 // [7:0] fine gain
#define Y_FINE_GAIN      0x04
#define Z_FINE_GAIN      0x05
#define XA_OFFSET_H      0x06 // User-defined trim values for accelerometer
#define XA_OFFSET_L_TC   0x07
#define YA_OFFSET_H      0x08
#define YA_OFFSET_L_TC   0x09
#define ZA_OFFSET_H      0x0A
#define ZA_OFFSET_L_TC   0x0B */

#define SELF_TEST_X_ACCEL 0x0D
#define SELF_TEST_Y_ACCEL 0x0E    
#define SELF_TEST_Z_ACCEL 0x0F

#define SELF_TEST_A      0x10

#define XG_OFFSET_H      0x13  // User-defined trim values for gyroscope
#define XG_OFFSET_L      0x14
#define YG_OFFSET_H      0x15
#define YG_OFFSET_L      0x16
#define ZG_OFFSET_H      0x17
#define ZG_OFFSET_L      0x18
#define SMPLRT_DIV       0x19
#define CONFIG           0x1A
#define GYRO_CONFIG      0x1B
#define ACCEL_CONFIG     0x1C
#define ACCEL_CONFIG2    0x1D
#define LP_ACCEL_ODR     0x1E   
#define WOM_THR          0x1F   

#define MOT_DUR          0x20  // Duration counter threshold for motion interrupt generation, 1 kHz rate, LSB = 1 ms
#define ZMOT_THR         0x21  // Zero-motion detection threshold bits [7:0]
#define ZRMOT_DUR        0x22  // Duration counter threshold for zero motion interrupt generation, 16 Hz rate, LSB = 64 ms

#define FIFO_EN          0x23
#define I2C_MST_CTRL     0x24   
#define I2C_SLV0_ADDR    0x25
#define I2C_SLV0_REG     0x26
#define I2C_SLV0_CTRL    0x27
#define I2C_SLV1_ADDR    0x28
#define I2C_SLV1_REG     0x29
#define I2C_SLV1_CTRL    0x2A
#define I2C_SLV2_ADDR    0x2B
#define I2C_SLV2_REG     0x2C
#define I2C_SLV2_CTRL    0x2D
#define I2C_SLV3_ADDR    0x2E
#define I2C_SLV3_REG     0x2F
#define I2C_SLV3_CTRL    0x30
#define I2C_SLV4_ADDR    0x31
#define I2C_SLV4_REG     0x32
#define I2C_SLV4_DO      0x33
#define I2C_SLV4_CTRL    0x34
#define I2C_SLV4_DI      0x35
#define I2C_MST_STATUS   0x36
#define INT_PIN_CFG      0x37
#define INT_ENABLE       0x38
#define DMP_INT_STATUS   0x39  // Check DMP interrupt
#define INT_STATUS       0x3A
#define ACCEL_XOUT_H     0x3B
#define ACCEL_XOUT_L     0x3C
#define ACCEL_YOUT_H     0x3D
#define ACCEL_YOUT_L     0x3E
#define ACCEL_ZOUT_H     0x3F
#define ACCEL_ZOUT_L     0x40
#define TEMP_OUT_H       0x41
#define TEMP_OUT_L       0x42
#define GYRO_XOUT_H      0x43
#define GYRO_XOUT_L      0x44
#define GYRO_YOUT_H      0x45
#define GYRO_YOUT_L      0x46
#define GYRO_ZOUT_H      0x47
#define GYRO_ZOUT_L      0x48
#define EXT_SENS_DATA_00 0x49
#define EXT_SENS_DATA_01 0x4A
#define EXT_SENS_DATA_02 0x4B
#define EXT_SENS_DATA_03 0x4C
#define EXT_SENS_DATA_04 0x4D
#define EXT_SENS_DATA_05 0x4E
#define EXT_SENS_DATA_06 0x4F
#define EXT_SENS_DATA_07 0x50
#define EXT_SENS_DATA_08 0x51
#define EXT_SENS_DATA_09 0x52
#define EXT_SENS_DATA_10 0x53
#define EXT_SENS_DATA_11 0x54
#define EXT_SENS_DATA_12 0x55
#define EXT_SENS_DATA_13 0x56
#define EXT_SENS_DATA_14 0x57
#define EXT_SENS_DATA_15 0x58
#define EXT_SENS_DATA_16 0x59
#define EXT_SENS_DATA_17 0x5A
#define EXT_SENS_DATA_18 0x5B
#define EXT_SENS_DATA_19 0x5C
#define EXT_SENS_DATA_20 0x5D
#define EXT_SENS_DATA_21 0x5E
#define EXT_SENS_DATA_22 0x5F
#define EXT_SENS_DATA_23 0x60
#define MOT_DETECT_STATUS 0x61
#define I2C_SLV0_DO      0x63
#define I2C_SLV1_DO      0x64
#define I2C_SLV2_DO      0x65
#define I2C_SLV3_DO      0x66
#define I2C_MST_DELAY_CTRL 0x67
#define SIGNAL_PATH_RESET  0x68
#define MOT_DETECT_CTRL  0x69
#define USER_CTRL        0x6A  // Bit 7 enable DMP, bit 3 reset DMP
#define PWR_MGMT_1       0x6B // Device defaults to the SLEEP mode
#define PWR_MGMT_2       0x6C
#define DMP_BANK         0x6D  // Activates a specific bank in the DMP
#define DMP_RW_PNT       0x6E  // Set read/write pointer to a specific start address in specified DMP bank
#define DMP_REG          0x6F  // Register in DMP from which to read or to which to write
#define DMP_REG_1        0x70
#define DMP_REG_2        0x71 
#define FIFO_COUNTH      0x72
#define FIFO_COUNTL      0x73
#define FIFO_R_W         0x74
#define WHO_AM_I_MPU9250 0x75 // Should return 0x71
#define XA_OFFSET_H      0x77
#define XA_OFFSET_L      0x78
#define YA_OFFSET_H      0x7A
#define YA_OFFSET_L      0x7B
#define ZA_OFFSET_H      0x7D
#define ZA_OFFSET_L      0x7E

#define PI 3.14159265358979323846f

// Using the MSENSR-9250 breakout board, ADO is set to 0 
// Seven-bit device address is 110100 for ADO = 0 and 110101 for ADO = 1
//mbed uses the eight-bit device address, so shift seven-bit addresses left by one!
#define ADO 0
#if ADO
#define MPU9250_ADDRESS 0x69<<1  // Device address when ADO = 1
#else
#define MPU9250_ADDRESS 0x68<<1  // Device address when ADO = 0
#endif  

#define Kp 2.0f * 5.0f // these are the free parameters in the Mahony filter and fusion scheme, Kp for proportional feedback, Ki for integral
#define Ki 0.0f



//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================s

//=====[Declaration and initialization of public global variables]=============
// float PI = 3.14159265358979323846f;
const float G = 9.80665f; // 

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]===================================
InertialSensor::InertialSensor ( ) {
    this->I2CHandler =  new I2CInterfaceHandler ();

    this->refreshTime = new NonBlockingDelay (REFRESHTIME);
  
    this->Mmode = 0x06;  


    this->GyroMeasError = PI * (60.0f / 180.0f);     // gyroscope measurement error in rads/s (start at 60 deg/s), then reduce after ~10 s to 3
    this->beta = sqrt(3.0f / 4.0f) * GyroMeasError;  // compute beta
    this->GyroMeasDrift = PI * (1.0f / 180.0f);      // gyroscope measurement drift in rad/s/s (start at 0.0 deg/s/s)
    this->zeta = sqrt(3.0f / 4.0f) * GyroMeasDrift;

    this->Ascale = AFS_2G;     // AFS_2G, AFS_4G, AFS_8G, AFS_16G
    this->Gscale = GFS_250DPS; // GFS_250DPS, GFS_500DPS, GFS_1000DPS, GFS_2000DPS
    this->Mscale = MFS_16BITS; // MFS_14BITS or MFS_16BITS, 14-bit or 16-bit magnetometer resolution
    this->getAres();
    this->getGres();
    this->getMres();
}

InertialSensor::~InertialSensor ( ) {
    delete this->I2CHandler; 
    this->I2CHandler  = NULL;
    delete  this->refreshTime;
    this->refreshTime = NULL;
}

bool InertialSensor::initializeSensors () {
    static int initializationStep = 0;
      char buffer [60];
    switch (initializationStep) {
        case 0:
            if (this->checkCommunicationWithModule () == true) {
                initializationStep++;
            }
            break;
        case 1:
            if (this->resetMPU9250 () == true) {
                initializationStep++;
            }
            break;
        case 2:
            if (this->calibrateMPU9250 () == true) {
                initializationStep++;
            }
            break;
        case 3:
            if (this->initMPU9250 () == true) {
                initializationStep++;
            }
            break;
        case 4:
            if (this->initAK8963 () == true) {
                initializationStep++;
            }
            break;
        default:
            initializationStep = 0;
            return true;
    }
    return false;
}


bool InertialSensor::checkCommunicationWithModule() {
    uint8_t whoami = this->I2CHandler->readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);  // Read WHO_AM_I register for MPU-9250
    char buffer [60];

    sprintf(buffer, "I AM 0x%x\n\r", whoami);  // Format message with whoami value
    uartUSB.write(buffer, strlen(buffer));     // Send formatted message over UART
    if (whoami == 0x71) { // WHO_AM_I should always be 0x68  
        sprintf(buffer, "MPU9250 is online...\n\r");
        uartUSB.write(buffer, strlen(buffer));
        return true;
    } else {
        sprintf(buffer, "Could not connect to MPU9250: \n\r");
        uartUSB.write(buffer, strlen(buffer));
        sprintf(buffer, "%#x \n", whoami);
        uartUSB.write(buffer, strlen(buffer));
        return false;
    }
}

bool InertialSensor::resetMPU9250() {
  // reset device
  static bool registerWritten = false;
  char buffer [60];



  if (registerWritten == false) {
        this->I2CHandler->writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
        registerWritten = true;
        sprintf(buffer, "reseting MPU9250\n\r");
        uartUSB.write(buffer, strlen(buffer));
  }
  if (this->refreshTime->read() && registerWritten == true)  { // wait a Refresh time 100ms
    registerWritten = false;
    return true;
  }
  return false;
}


// Function which accumulates gyro and accelerometer data after device initialization. It calculates the average
// of the at-rest readings and then loads the resulting offsets into accelerometer and gyro bias registers.
bool InertialSensor::calibrateMPU9250( ) {  
    char buffer [60];
    uint8_t data[12]; // data array to hold accelerometer and gyro x, y, z, data
    uint16_t ii, packet_count, fifo_count;
    int32_t gyro_bias[3] = {0, 0, 0}, accel_bias[3] = {0, 0, 0};

    static bool firstWaitPassed = false;
    static bool registerReset = false;

    static bool secondWaitPassed = false;
    static bool clockSet = false;

    static bool thirdWaitPassed = false;
    static bool biasConfigured = false;
    
    static bool fourthWaitPassed = false;
    static bool accelgyrobiasConfigured = false;
    
    // reset device, reset all registers, clear gyro and accelerometer bias registers
    if (registerReset == false) {
        this->I2CHandler->writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
        registerReset = true;
    }

    if (firstWaitPassed  == false) {
        if (this->refreshTime->read()) {
            firstWaitPassed = true;
        } else { return false;}
    }

    // get stable time source
    // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
    if (clockSet == false) {
        this->I2CHandler->writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x01);  
        this->I2CHandler->writeByte(MPU9250_ADDRESS, PWR_MGMT_2, 0x00); 
        clockSet = true;
    }
    if (secondWaitPassed  == false) {
        if (this->refreshTime->read()) {
            secondWaitPassed = true;
        } else { return false;}
    }
    
        // Configure device for bias calculation
    if (biasConfigured == false) {
        this->I2CHandler->writeByte(MPU9250_ADDRESS, INT_ENABLE, 0x00);   // Disable all interrupts
        this->I2CHandler->writeByte(MPU9250_ADDRESS, FIFO_EN, 0x00);      // Disable FIFO
        this->I2CHandler->writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x00);   // Turn on internal clock source
        this->I2CHandler->writeByte(MPU9250_ADDRESS, I2C_MST_CTRL, 0x00); // Disable I2C master
        this->I2CHandler->writeByte(MPU9250_ADDRESS, USER_CTRL, 0x00);    // Disable FIFO and I2C master modes
        this->I2CHandler->writeByte(MPU9250_ADDRESS, USER_CTRL, 0x0C);    // Reset FIFO and DMP
        biasConfigured = true;
    }
    if (thirdWaitPassed  == false) {
        if (this->refreshTime->read()) {
            thirdWaitPassed = true;
        } else { return false;}
    }
    
    if (accelgyrobiasConfigured == false) {
    // Configure MPU9250 gyro and accelerometer for bias calculation
    I2CHandler->writeByte(MPU9250_ADDRESS, CONFIG, 0x01);      // Set low-pass filter to 188 Hz
    I2CHandler->writeByte(MPU9250_ADDRESS, SMPLRT_DIV, 0x00);  // Set sample rate to 1 kHz
    I2CHandler->writeByte(MPU9250_ADDRESS, GYRO_CONFIG, 0x00);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
    I2CHandler->writeByte(MPU9250_ADDRESS, ACCEL_CONFIG, 0x00); // Set accelerometer full-scale to 2 g, maximum sensitivity
    

    // Configure FIFO to capture accelerometer and gyro data for bias calculation
    I2CHandler->writeByte(MPU9250_ADDRESS, USER_CTRL, 0x40);   // Enable FIFO  
    I2CHandler->writeByte(MPU9250_ADDRESS, FIFO_EN, 0x78);     // Enable gyro and accelerometer sensors for FIFO (max size 512 bytes in MPU-9250)
    }
    /*
    if (fourthWaitPassed  == false) {
        if (this->refreshTime->read()) {
            fourthWaitPassed  = true;
        } else { return false;}
    }*/
    wait_us(40000); // BLOQUEANTE BLOQUEO

    uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
    uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g


    // At end of sample accumulation, turn off FIFO sensor read
    I2CHandler->writeByte(MPU9250_ADDRESS, FIFO_EN, 0x00);        // Disable gyro and accelerometer sensors for FIFO
    I2CHandler->readBytes(MPU9250_ADDRESS, FIFO_COUNTH, 2, &data[0]); // read FIFO sample count
    fifo_count = ((uint16_t)data[0] << 8) | data[1];
    packet_count = fifo_count/12;// How many sets of full gyro and accelerometer data for averaging

    for (ii = 0; ii < packet_count; ii++) {
        int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};
        I2CHandler->readBytes(MPU9250_ADDRESS, FIFO_R_W, 12, &data[0]); // read data for averaging
        accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  ) ;  // Form signed 16-bit integer for each sample in FIFO
        accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  ) ;
        accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  ) ;    
        gyro_temp[0]  = (int16_t) (((int16_t)data[6] << 8) | data[7]  ) ;
        gyro_temp[1]  = (int16_t) (((int16_t)data[8] << 8) | data[9]  ) ;
        gyro_temp[2]  = (int16_t) (((int16_t)data[10] << 8) | data[11]) ;
        
        accel_bias[0] += (int32_t) accel_temp[0]; // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
        accel_bias[1] += (int32_t) accel_temp[1];
        accel_bias[2] += (int32_t) accel_temp[2];
        gyro_bias[0]  += (int32_t) gyro_temp[0];
        gyro_bias[1]  += (int32_t) gyro_temp[1];
        gyro_bias[2]  += (int32_t) gyro_temp[2];
                
    }
        accel_bias[0] /= (int32_t) packet_count; // Normalize sums to get average count biases
        accel_bias[1] /= (int32_t) packet_count;
        accel_bias[2] /= (int32_t) packet_count;
        gyro_bias[0]  /= (int32_t) packet_count;
        gyro_bias[1]  /= (int32_t) packet_count;
        gyro_bias[2]  /= (int32_t) packet_count;
        
    if(accel_bias[2] > 0L) {accel_bias[2] -= (int32_t) accelsensitivity;}  // Remove gravity from the z-axis accelerometer bias calculation
    else {accel_bias[2] += (int32_t) accelsensitivity;}
    
    // Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
    data[0] = (-gyro_bias[0]/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
    data[1] = (-gyro_bias[0]/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
    data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
    data[3] = (-gyro_bias[1]/4)       & 0xFF;
    data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
    data[5] = (-gyro_bias[2]/4)       & 0xFF;

    this->gyroBias[0] = (float) gyro_bias[0]/(float) gyrosensitivity; // construct gyro bias in deg/s for later manual subtraction
    this->gyroBias[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
    this->gyroBias[2] = (float) gyro_bias[2]/(float) gyrosensitivity;

    // Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
    // factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
    // non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
    // compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
    // the accelerometer biases calculated above must be divided by 8.

    int32_t accel_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
    I2CHandler->readBytes(MPU9250_ADDRESS, XA_OFFSET_H, 2, &data[0]); // Read factory accelerometer trim values
    accel_bias_reg[0] = (int16_t) ((int16_t)data[0] << 8) | data[1];
    I2CHandler->readBytes(MPU9250_ADDRESS, YA_OFFSET_H, 2, &data[0]);
    accel_bias_reg[1] = (int16_t) ((int16_t)data[0] << 8) | data[1];
    I2CHandler->readBytes(MPU9250_ADDRESS, ZA_OFFSET_H, 2, &data[0]);
    accel_bias_reg[2] = (int16_t) ((int16_t)data[0] << 8) | data[1];
    
    uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
    uint8_t mask_bit[3] = {0, 0, 0}; // Define array to hold mask bit for each accelerometer bias axis
    
    for(ii = 0; ii < 3; ii++) {
        if(accel_bias_reg[ii] & mask) mask_bit[ii] = 0x01; // If temperature compensation bit is set, record that fact in mask_bit
    }

    // Construct total accelerometer bias, including calculated average accelerometer bias from above
    accel_bias_reg[0] -= (accel_bias[0]/8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
    accel_bias_reg[1] -= (accel_bias[1]/8);
    accel_bias_reg[2] -= (accel_bias[2]/8);
    
    data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
    data[1] = (accel_bias_reg[0])      & 0xFF;
    data[1] = data[1] | mask_bit[0]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
    data[3] = (accel_bias_reg[1])      & 0xFF;
    data[3] = data[3] | mask_bit[1]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
    data[5] = (accel_bias_reg[2])      & 0xFF;
    data[5] = data[5] | mask_bit[2]; // preserve temperature compensation bit when writing back to accelerometer bias registers


    // Output scaled accelerometer biases for manual subtraction in the main program
    this->accelBias[0] = (float)accel_bias[0]/(float)accelsensitivity; 
    this->accelBias[1] = (float)accel_bias[1]/(float)accelsensitivity;
    this->accelBias[2] = (float)accel_bias[2]/(float)accelsensitivity;
    
    sprintf(buffer, "MPU9250 initialized for active data mode!\n\r"); // Initialize device for active mode read of accelerometer, gyroscope, and temperature
    uartUSB.write(buffer, strlen(buffer));


    sprintf(buffer, "x gyro bias = %f\n\r", this->gyroBias[0]);
    uartUSB.write(buffer, strlen(buffer));
    sprintf(buffer, "y gyro bias = %f\n\r", this->gyroBias[1]);
    uartUSB.write(buffer, strlen(buffer));

    sprintf(buffer, "z gyro bias = %f\n\r", this->gyroBias[2]);
    uartUSB.write(buffer, strlen(buffer));
    sprintf(buffer, "x accel bias = %f\n\r", this->accelBias[0]);
    uartUSB.write(buffer, strlen(buffer));
    sprintf(buffer, "y accel bias = %f\n\r", this->accelBias[1]);
    uartUSB.write(buffer, strlen(buffer));
    sprintf(buffer, "z accel bias = %f\n\r", this->accelBias[2]);
    uartUSB.write(buffer, strlen(buffer));

    firstWaitPassed = false;
    registerReset = false;
    secondWaitPassed = false;
    clockSet = false;
    thirdWaitPassed = false;
    biasConfigured = false;
    fourthWaitPassed = false;
    accelgyrobiasConfigured = false;

    return true;
}



bool InertialSensor::initAK8963() {
    static int initializationStep = 0;

    // First extract the factory calibration for each magnetometer axis
    uint8_t rawData[3];  // x/y/z gyro calibration data stored here
    char buffer [80];

    if (this->refreshTime->read()) {
        switch (initializationStep) {
            case 0:
                this->I2CHandler->writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer
                initializationStep ++;
                break;
            case 1:
                this->I2CHandler->writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x0F); // Enter Fuse ROM access mode
                initializationStep ++;
                break;
            case 2:
                this->I2CHandler->readBytes(AK8963_ADDRESS, AK8963_ASAX, 3, &rawData[0]);  // Read the x-, y-, and z-axis calibration values
                this->magCalibration[0] =  (float)(rawData[0] - 128)/256.0f + 1.0f;   // Return x-axis sensitivity adjustment values, etc.
                this->magCalibration[1] =  (float)(rawData[1] - 128)/256.0f + 1.0f;  
                this->magCalibration[2] =  (float)(rawData[2] - 128)/256.0f + 1.0f; 
                this->I2CHandler->writeByte(AK8963_ADDRESS, AK8963_CNTL, 0x00); // Power down magnetometer  
                initializationStep ++;
                break;
            case 3:
                // Configure the magnetometer for continuous read and highest resolution
                // set Mscale bit 4 to 1 (0) to enable 16 (14) bit resolution in CNTL register,
                // and enable continuous mode data acquisition Mmode (bits [3:0]), 0010 for 8 Hz and 0110 for 100 Hz sample rates
                this->I2CHandler->writeByte(AK8963_ADDRESS, AK8963_CNTL, this->Mscale << 4 | this->Mmode); // Set magnetometer data resolution and sample ODR
                sprintf(buffer, "AK8963 initialized for active data mode....\n\r"); // Initialize device for active mode read of magnetometer
                uartUSB.write(buffer, strlen(buffer));

                if(this->Mscale == 0) uartUSB.write("Magnetometer resolution = 14  bits\n\r", 36);
                if(this->Mscale == 1) uartUSB.write("Magnetometer resolution = 16  bits\n\r", 36);
                if(this->Mmode == 2) uartUSB.write("Magnetometer ODR = 8 Hz\n\r", 25);
                if(this->Mmode == 6) uartUSB.write("Magnetometer ODR = 100 Hz\n\r", 28);
                initializationStep ++;
                break;
            default:
                initializationStep = 0;
                return true;
        }
    }  
    return false;      
}

bool InertialSensor::initMPU9250() {  
     // Initialize MPU9250 device
    // wake up device
    static bool watingPassed = false;
    static bool sensorsEnable = false;

    if (sensorsEnable  == false) {
        this->I2CHandler->writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors 
        sensorsEnable  = true;
    }

    //  Delay 100 ms for PLL to get established on x-axis gyro; should check for PLL ready interrupt  
    if (watingPassed  == false) {
        if (this->refreshTime->read()) {
            watingPassed = true;
        } else { return false;}
    }

    // get stable time source
    this->I2CHandler->writeByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x01);  // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001

    // Configure Gyro and Accelerometer
    // Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively; 
    // DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both
    // Maximum delay is 4.9 ms which is just over a 200 Hz maximum rate
    this->I2CHandler->writeByte(MPU9250_ADDRESS, CONFIG, 0x03);  
 
    // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
    this->I2CHandler->writeByte(MPU9250_ADDRESS, SMPLRT_DIV, 0x04);  // Use a 200 Hz rate; the same rate set in CONFIG above
    
    // Set gyroscope full scale range
    // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
    uint8_t c = this->I2CHandler->readByte(MPU9250_ADDRESS, GYRO_CONFIG); // get current GYRO_CONFIG register value
    // c = c & ~0xE0; // Clear self-test bits [7:5] 
    c = c & ~0x02; // Clear Fchoice bits [1:0] 
    c = c & ~0x18; // Clear AFS bits [4:3]
    c = c | this->Gscale << 3; // Set full scale range for the gyro
    // c =| 0x00; // Set Fchoice for the gyro to 11 by writing its inverse to bits 1:0 of GYRO_CONFIG
    this->I2CHandler->writeByte(MPU9250_ADDRESS, GYRO_CONFIG, c ); // Write new GYRO_CONFIG value to register
    
    // Set accelerometer full-scale range configuration
    c = this->I2CHandler->readByte(MPU9250_ADDRESS, ACCEL_CONFIG); // get current ACCEL_CONFIG register value
    // c = c & ~0xE0; // Clear self-test bits [7:5] 
    c = c & ~0x18;  // Clear AFS bits [4:3]
    c = c | this->Ascale << 3; // Set full scale range for the accelerometer 
    this->I2CHandler->writeByte(MPU9250_ADDRESS, ACCEL_CONFIG, c); // Write new ACCEL_CONFIG register value

    // Set accelerometer sample rate configuration
    // It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
    // accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
    c = this->I2CHandler->readByte(MPU9250_ADDRESS, ACCEL_CONFIG2); // get current ACCEL_CONFIG2 register value
    c = c & ~0x0F; // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])  
    c = c | 0x03;  // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz
    this->I2CHandler->writeByte(MPU9250_ADDRESS, ACCEL_CONFIG2, c); // Write new ACCEL_CONFIG2 register value

    // The accelerometer, gyro, and thermometer are set to 1 kHz sample rates, 
    // but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting

    // Configure Interrupts and Bypass Enable
    // Set interrupt pin active high, push-pull, and clear on read of INT_STATUS, enable I2C_BYPASS_EN so additional chips 
    // can join the I2C bus and all can be controlled by the STM32 as master
    this->I2CHandler->writeByte(MPU9250_ADDRESS, INT_PIN_CFG, 0x22);    
    this->I2CHandler->writeByte(MPU9250_ADDRESS, INT_ENABLE, 0x01);  // Enable data ready (bit 0) interrupt
    return true;
}


void InertialSensor::readAccelData() {
    char buffer [60];
    int16_t accelCount[3] = {0, 0, 0};  // Stores the 16-bit signed accelerometer sensor output
    uint8_t rawData[6];  // x/y/z accel register data stored here

    this->I2CHandler->readBytes(MPU9250_ADDRESS, ACCEL_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers into data array
    accelCount[0] = (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
    accelCount[1] = (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;  
    accelCount[2] = (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ; 

    this->ax = (float) accelCount[0] * this->aRes - this->accelBias[0];  // get actual g value, this depends on scale being set
    this->ay = (float) accelCount[1] * this->aRes - this->accelBias[1];   
    this->az = (float) accelCount[2] * this->aRes - this->accelBias[2]; 

    /////
    sprintf(buffer, "ax = %f (m/s2)\n\r",  this->ax * G);
    uartUSB.write(buffer, strlen(buffer));
    sprintf(buffer, "ay = %f (m/s2)\n\r", this->ay * G);
    uartUSB.write(buffer, strlen(buffer));
    sprintf(buffer, "az = %f (m/s2)\n\r", this->az * G);
    uartUSB.write(buffer, strlen(buffer));
}

// unit °/seg
void InertialSensor::readGyroData( ) {
    char buffer [60];
    int16_t gyroCount[3] = {0, 0, 0}; 
    uint8_t rawData[6];  // x/y/z gyro register data stored here

    this->I2CHandler->readBytes(MPU9250_ADDRESS, GYRO_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
    gyroCount[0] = (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]);  // Turn the MSB and LSB into a signed 16-bit value
    gyroCount[1] = (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]);  
    gyroCount[2] = (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]); 

    this->gx = (float) gyroCount[0]*this->gRes - this->gyroBias[0];  // get actual gyro value, this depends on scale being set
    this->gy = (float) gyroCount[1]*this->gRes - this->gyroBias[1];  
    this->gz = (float) gyroCount[2]*this->gRes - this->gyroBias[2];   

    /////
    sprintf(buffer, "gx = %f\n\r",  this->gx);
    uartUSB.write(buffer, strlen(buffer));
    sprintf(buffer, "gy = %f\n\r", this->gy);
    uartUSB.write(buffer, strlen(buffer));
    sprintf(buffer, "gz = %f\n\r", this->gz);
    uartUSB.write(buffer, strlen(buffer));
}

void InertialSensor::readMagData() {
    char buffer [60];
    int16_t magCount[3] = {0, 0, 0};
    uint8_t rawData[7];  // x/y/z gyro register data, ST2 register stored here, must read ST2 at end of data acquisition
    
    if(this->I2CHandler->readByte(AK8963_ADDRESS, AK8963_ST1) & 0x01) { // wait for magnetometer data ready bit to be set
    this->I2CHandler->readBytes(AK8963_ADDRESS, AK8963_XOUT_L, 7, &rawData[0]);  // Read the six raw data and ST2 registers sequentially into data array
    uint8_t c = rawData[6]; // End data read by reading ST2 register
    if(!(c & 0x08)) { // Check if magnetic sensor overflow set, if not then report data
    magCount[0] = (int16_t)(((int16_t)rawData[1] << 8) | rawData[0]);  // Turn the MSB and LSB into a signed 16-bit value
    magCount[1] = (int16_t)(((int16_t)rawData[3] << 8) | rawData[2]) ;  // Data stored as little Endian
    magCount[2] = (int16_t)(((int16_t)rawData[5] << 8) | rawData[4]) ; 

    this->mx = (float) magCount[0] * this->mRes*this->magCalibration[0] - this->magbias[0];  // get actual magnetometer value, this depends on scale being set
    this->my = (float) magCount[1] * this->mRes*this->magCalibration[1] - this->magbias[1];  
    this->mz = (float) magCount[2] * this->mRes*this->magCalibration[2] - this->magbias[2]; 

        /////
    sprintf(buffer, "mx = %f\n\r",  this->mx);
    uartUSB.write(buffer, strlen(buffer));
    sprintf(buffer, "my = %f\n\r", this->my);
    uartUSB.write(buffer, strlen(buffer));
    sprintf(buffer, "mz = %f\n\r", this->mz);
    uartUSB.write(buffer, strlen(buffer));
   }
  }
}

void InertialSensor::readTempData() {
    char buffer [60];
    int16_t tempCount;
    uint8_t rawData[2];  // x/y/z gyro register data stored here

    this->I2CHandler->readBytes(MPU9250_ADDRESS, TEMP_OUT_H, 2, &rawData[0]);  // Read the two raw data registers sequentially into data array 
    tempCount =(int16_t)(((int16_t)rawData[0]) << 8 | rawData[1]) ;  // Turn the MSB and LSB into a 16-bit value
    this->temperature = ((float) tempCount) / 333.87f + 21.0f; // Temperature in degrees Centig
    
    sprintf(buffer, "Temperature = %f Celsius \n\r",  this->temperature);
    uartUSB.write(buffer, strlen(buffer));
}


// Implementation of Sebastian Madgwick's "...efficient orientation filter for... inertial/magnetic sensor arrays"
// (see http://www.x-io.co.uk/category/open-source/ for examples and more details)
// which fuses acceleration, rotation rate, and magnetic moments to produce a quaternion-based estimate of absolute
// device orientation -- which can be converted to yaw, pitch, and roll. Useful for stabilizing quadcopters, etc.
// The performance of the orientation filter is at least as good as conventional Kalman-based filtering algorithms
// but is much less computationally intensive---it can be performed on a 3.3 V Pro Mini operating at 8 MHz!
void InertialSensor::MadgwickQuaternionUpdate(float  deltat) {
    float q1 = this->q[0], q2 = this->q[1], q3 = this->q[2], q4 = this->q[3];   // short name local variable for readability
    float ax = this->ax, ay= this->ay, az= this->az;
    float gx = this->gx *PI/180.0f, gy = this->gy *PI/180.0f, gz = this->gz *PI/180.0f;
    float mx = this->mx, my = this->my, mz = this->mz;
    float beta = this->beta;
    
   
    float norm;
    float hx, hy, _2bx, _2bz;
    float s1, s2, s3, s4;
    float qDot1, qDot2, qDot3, qDot4;

    // Auxiliary variables to avoid repeated arithmetic
    float _2q1mx;
    float _2q1my;
    float _2q1mz;
    float _2q2mx;
    float _4bx;
    float _4bz;
    float _2q1 = 2.0f * q1;
    float _2q2 = 2.0f * q2;
    float _2q3 = 2.0f * q3;
    float _2q4 = 2.0f * q4;
    float _2q1q3 = 2.0f * q1 * q3;
    float _2q3q4 = 2.0f * q3 * q4;
    float q1q1 = q1 * q1;
    float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q1q4 = q1 * q4;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q2q4 = q2 * q4;
    float q3q3 = q3 * q3;
    float q3q4 = q3 * q4;
    float q4q4 = q4 * q4;

    // Normalise accelerometer measurement
    norm = sqrt(ax * ax + ay * ay + az * az);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f/norm;
    ax *= norm;
    ay *= norm;
    az *= norm;

    // Normalise magnetometer measurement
    norm = sqrt(mx * mx + my * my + mz * mz);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f/norm;
    mx *= norm;
    my *= norm;
    mz *= norm;

    // Reference direction of Earth's magnetic field
    _2q1mx = 2.0f * q1 * mx;
    _2q1my = 2.0f * q1 * my;
    _2q1mz = 2.0f * q1 * mz;
    _2q2mx = 2.0f * q2 * mx;
    hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
    hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;
    _2bx = sqrt(hx * hx + hy * hy);
    _2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
    _4bx = 2.0f * _2bx;
    _4bz = 2.0f * _2bz;

    // Gradient decent algorithm corrective step
    s1 = -_2q3 * (2.0f * q2q4 - _2q1q3 - ax) + _2q2 * (2.0f * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s2 = _2q4 * (2.0f * q2q4 - _2q1q3 - ax) + _2q1 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q2 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + _2bz * q4 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s3 = -_2q1 * (2.0f * q2q4 - _2q1q3 - ax) + _2q4 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q3 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s4 = _2q2 * (2.0f * q2q4 - _2q1q3 - ax) + _2q3 * (2.0f * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    norm = sqrt(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);    // normalise step magnitude
    norm = 1.0f/norm;
    s1 *= norm;
    s2 *= norm;
    s3 *= norm;
    s4 *= norm;

    // Compute rate of change of quaternion
    qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - beta * s1;
    qDot2 = 0.5f * (q1 * gx + q3 * gz - q4 * gy) - beta * s2;
    qDot3 = 0.5f * (q1 * gy - q2 * gz + q4 * gx) - beta * s3;
    qDot4 = 0.5f * (q1 * gz + q2 * gy - q3 * gx) - beta * s4;

    // Integrate to yield quaternion
    q1 += qDot1 * deltat;
    q2 += qDot2 * deltat;
    q3 += qDot3 * deltat;
    q4 += qDot4 * deltat;
    norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
    norm = 1.0f/norm;
    this->q[0] = q1 * norm;
    this->q[1] = q2 * norm;
    this->q[2] = q3 * norm;
    this->q[3] = q4 * norm;
    char buffer[60];
    snprintf(buffer, sizeof(buffer), "q0 = %f, q1 = %f, q2 = %f, q3 = %f\n\r", 
            this->q[0], this->q[1], this->q[2], this->q[3]);
    uartUSB.write(buffer, strlen(buffer));

}

void InertialSensor::obtainYawPitchRoll( ) {
    float pitch, yaw, roll;

    yaw   = atan2(2.0f * (this->q[1] * this->q[2] + this->q[0] * this->q[3]), this->q[0] * 
    this->q[0] + this->q[1] * this->q[1] - this->q[2] * this->q[2] - this->q[3] * this->q[3]);   
    pitch = -asin(2.0f * (this->q[1] * this->q[3] - this->q[0] * this->q[2]));
    roll  = atan2(2.0f * (this->q[0] * this->q[1] + this->q[2] * this->q[3]),
     this->q[0] * this->q[0] - this->q[1] * this->q[1] - this->q[2] * this->q[2] + this->q[3] * this->q[3]);
    pitch *= 180.0f / PI;
    yaw   *= 180.0f / PI; 
    yaw   -= 13.8f; // Declination at Danville, California is 13 degrees 48 minutes and 47 seconds on 2014-04-04
    roll  *= 180.0f / PI;

    char buffer[60];
    snprintf(buffer, sizeof(buffer), "pitch: %f, roll: = %f, yaw = %f\n\r", 
            pitch, roll, yaw);
    uartUSB.write(buffer, strlen(buffer));
}
  
 bool InertialSensor::acquireData (float deltat) {
     if(this->I2CHandler->readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01) {
        this->readAccelData();
        this->readGyroData();
        this->readMagData();
        this->readTempData();
        this->MadgwickQuaternionUpdate(deltat);
        this->obtainYawPitchRoll();
        return true;
    }
    return false;
}

//=====[Implementations of private functions]==================================

void InertialSensor::getMres() {
  switch (this->Mscale)
  {
    // Possible magnetometer scales (and their register bit settings) are:
    // 14 bit resolution (0) and 16 bit resolution (1)
    case MFS_14BITS:
          this->mRes = 10.0*4912.0/8190.0; // Proper scale to return milliGauss
          break;
    case MFS_16BITS:
          this->mRes = 10.0*4912.0/32760.0; // Proper scale to return milliGauss
          break;
  }
}


void InertialSensor::getGres() {
  switch (this->Gscale)
  {
    // Possible gyro scales (and their register bit settings) are:
    // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11). 
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    case GFS_250DPS:
          this->gRes = 250.0/32768.0;
          break;
    case GFS_500DPS:
          this->gRes = 500.0/32768.0;
          break;
    case GFS_1000DPS:
          this->gRes = 1000.0/32768.0;
          break;
    case GFS_2000DPS:
          this->gRes = 2000.0/32768.0;
          break;
  }
}


void InertialSensor::getAres() {
  switch (this->Ascale)
  {
    // Possible accelerometer scales (and their register bit settings) are:
    // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11). 
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    case AFS_2G:
          this->aRes = 2.0/32768.0;
          break;
    case AFS_4G:
          this->aRes = 4.0/32768.0;
          break;
    case AFS_8G:
          this->aRes = 8.0/32768.0;
          break;
    case AFS_16G:
          this->aRes = 16.0/32768.0;
          break;
  }
}

