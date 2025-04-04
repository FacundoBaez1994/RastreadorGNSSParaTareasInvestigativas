//=====[Libraries]=============================================================
#include "IMU.h"
#include "Debugger.h" // due to global usbUart
#include "MPU9250RegisterMap.h"



//=====[Declaration of private defines]========================================
#define I2C_SDA_DEFAULT PA_10
#define I2C_SCL_DEFAULT PA_9 

#define I2C_FREQUENCY_DEFAULT 400000
#define REFRESH_TIME_DEFAULT 100

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]===================================
/** 
* @brief Contructor method creates a new trackerGPS instance ready to be used
*/
IMU::IMU () {
    this->currentState =  new initializeMPU9250 (this);
    this->interface =  new I2CInterfaceHandler ();
    this->refreshTime = new NonBlockingDelay (REFRESH_TIME_DEFAULT);
    this->mpuI2CAddress = MPU9250_DEFAULT_ADDRESS;

    this->setting = MPU9250Setting();
}


IMU::~IMU() {
    delete this->currentState;
    this->currentState = nullptr; 
    delete this->interface; 
    this->interface = nullptr;
    delete this->refreshTime;
    this->refreshTime = nullptr;
}


/** 
* @brief Main rutine of the tracker device
*   
*
*/
bool IMU::obtainInertialMeasures (char * dataObtain, float * temperatureObtain) {
    static char newData [100];
    float temperature; 
    
    if (this->currentState->obtainInertialData(this->interface, newData) == true){
        this->currentState->obtainTemperature(this->interface, &temperature);

        *temperatureObtain =  temperature;
        strcpy (dataObtain, newData);

        return true;  
    }
    return false;
}


/** 
* @brief Main rutine of the tracker device
*   
*
*/
bool IMU::calibrate () {
    this->currentState->initialize(this->interface, this->refreshTime);
    if (this->currentState->calibrate(this->interface, this->refreshTime)) {
        return true;
    }
    return false;
}


void IMU::resetIMU() {
    // Reiniciar los atributos de la clase a sus valores por defecto
    this->mag_bias_factory[0] = 0.0f;
    this->mag_bias_factory[1] = 0.0f;
    this->mag_bias_factory[2] = 0.0f;

    this->a_bias[0] = 0.0f;
    this->a_bias[1] = 0.0f;
    this->a_bias[2] = 0.0f;

    this->g_bias[0] = 0.0f;
    this->g_bias[1] = 0.0f;
    this->g_bias[2] = 0.0f;

    this->mag_bias[0] = 0.0f;
    this->mag_bias[1] = 0.0f;
    this->mag_bias[2] = 0.0f;

    this->mag_scale[0] = 1.0f;
    this->mag_scale[1] = 1.0f;
    this->mag_scale[2] = 1.0f;

    // Volver a inicializar y calibrar
    this->changeState(new initializeMPU9250(this));
}


void IMU::changeState  (IMUState * newImuState) {
    delete this->currentState;
    this->currentState = newImuState;
}

uint8_t IMU::getMPUI2CAddress ( ) {
    return this->mpuI2CAddress;
}



////// Settings
MPU9250Setting IMU::getMPUSettings ( ) {
    return this->setting;
}


float IMU::get_acc_resolution(const ACCEL_FS_SEL accel_af_sel) const {
    switch (accel_af_sel) {
        // Possible accelerometer scales (and their register bit settings) are:
        // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
        case ACCEL_FS_SEL::A2G:
            return 2.0 / 32768.0;
        case ACCEL_FS_SEL::A4G:
            return 4.0 / 32768.0;
        case ACCEL_FS_SEL::A8G:
            return 8.0 / 32768.0;
        case ACCEL_FS_SEL::A16G:
            return 16.0 / 32768.0;
        default:
            return 0.;
    }
}

float IMU::get_gyro_resolution(const GYRO_FS_SEL gyro_fs_sel) const {
    switch (gyro_fs_sel) {
        // Possible gyro scales (and their register bit settings) are:
        // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11).
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
        case GYRO_FS_SEL::G250DPS:
            return 250.0 / 32768.0;
        case GYRO_FS_SEL::G500DPS:
            return 500.0 / 32768.0;
        case GYRO_FS_SEL::G1000DPS:
            return 1000.0 / 32768.0;
        case GYRO_FS_SEL::G2000DPS:
            return 2000.0 / 32768.0;
        default:
            return 0.;
    }
}

float IMU::get_mag_resolution(const MAG_OUTPUT_BITS mag_output_bits) const {
    switch (mag_output_bits) {
        // Possible magnetometer scales (and their register bit settings) are:
        // 14 bit resolution (0) and 16 bit resolution (1)
        // Proper scale to return milliGauss
        case MAG_OUTPUT_BITS::M14BITS:
            return 10. * 4912. / 8190.0;
        case MAG_OUTPUT_BITS::M16BITS:
            return 10. * 4912. / 32760.0;
        default:
            return 0.;
    }
}

//////////////////////////////////////////////////
void IMU::set_mag_bias_factory (float xMagBias, float yMagBias, float zMagBias) {
    this->mag_bias_factory[0] = xMagBias;
    this->mag_bias_factory[1] = yMagBias;
    this->mag_bias_factory[2] = zMagBias;
}

void IMU::get_mag_bias_factory (float &xMagBias, float &yMagBias, float &zMagBias) {
    xMagBias = this->mag_bias_factory[0];
    yMagBias= this->mag_bias_factory[1];
    zMagBias = this->mag_bias_factory[2];
}

void IMU::set_accel_bias (float xa_bias, float ya_bias, float za_bias) {
    this->a_bias[0] = xa_bias;
    this->a_bias[1] = ya_bias;
    this->a_bias[2] = za_bias;
}

void IMU::get_accel_bias (float &xa_bias, float &ya_bias, float &za_bias) {
    xa_bias = this->a_bias[0];
    ya_bias = this->a_bias[1];
    za_bias = this->a_bias[2];
}

void IMU::set_gyro_bias (float xg_bias, float yg_bias, float zg_bias) {
    this->g_bias[0] = xg_bias;
    this->g_bias[1] = yg_bias;
    this->g_bias[2] = zg_bias;
}

void IMU::get_gyro_bias (float &xg_bias, float &yg_bias, float &zg_bias) {
    xg_bias = this->g_bias[0];
    yg_bias = this->g_bias[1];
    zg_bias = this->g_bias[2];
}


void IMU::set_mag_bias (float xmag_bias, float ymag_bias, float zmag_bias) {
    this->mag_bias[0] = xmag_bias;
    this->mag_bias[1] = ymag_bias;
    this->mag_bias[2] = zmag_bias;
}

void IMU::get_mag_bias (float &xmag_bias, float &ymag_bias, float &zmag_bias) {
    xmag_bias = this->mag_bias[0];
    ymag_bias = this->mag_bias[0];
    zmag_bias = this->mag_bias[0];
}

void IMU::set_mag_scale (float xmag_scale, float ymag_scale, float zmag_scale){
    this->mag_scale[0] = xmag_scale;
    this->mag_scale[1] = ymag_scale;
    this->mag_scale[2] = zmag_scale;
}

void IMU::get_mag_scale (float &xmag_scale, float &ymag_scale, float &zmag_scale) {
    xmag_scale = this->mag_scale[0];
    ymag_scale = this->mag_scale[1];
    zmag_scale = this->mag_scale[2];
}




//=====[Implementations of private methods]==================================