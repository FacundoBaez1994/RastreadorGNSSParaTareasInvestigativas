//=====[Libraries]=============================================================

#include "acquiringData.h"
#include "IMU.h" //debido a declaracion adelantada

//=====[Declaration of private defines]========================================
#define MAXSAMPLES 100

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
acquiringData::acquiringData (IMU * imu) {
    this->imu = imu;
    this->b_verbose = true;
    this->mpuI2CAddr = 0x68;
    
    MPU9250Setting setting = this->imu->getMPUSettings();
    this->acc_resolution = this->imu->get_acc_resolution(setting.accel_fs_sel);
    this->gyro_resolution = this->imu->get_gyro_resolution(setting.gyro_fs_sel);
    this->mag_resolution = this->imu->get_mag_resolution(setting.mag_output_bits);

    this->imu->get_mag_scale(this->mag_scale[0], this->mag_scale[1], this->mag_scale[2]);
    this->imu->get_mag_bias(this->mag_bias[0], this->mag_bias[1], this->mag_bias[2]);
    this->imu->get_mag_bias_factory(this->mag_bias_factory[0], this->mag_bias_factory[1], 
    this->mag_bias_factory[2]);


    this->quat_filter =  new QuaternionFilter ();
    /////// DEBUGG
    char buffer[128];
        int len = snprintf(buffer, sizeof(buffer),
                       "\n\rmag_scale[0], mag_scale[1], mag_scale[2]: %.2f, %.2f, %.2f\n\r",
                       this->mag_scale[0], this->mag_scale[1], mag_scale[2]);
    uartUSB.write(buffer, len);

        len = snprintf(buffer, sizeof(buffer),
                       "\n\rmag_bias[0], mag_bias[1], mag_bias[2]: %.2f, %.2f, %.2f\n\r",
                       this->mag_bias[0], this->mag_bias[1], this->mag_bias[2]);
    uartUSB.write(buffer, len);

        len = snprintf(buffer, sizeof(buffer),
                       "\n\rmag_bias_factory[0], mag_bias_factory[1], mag_bias_factory[2]: %.2f, %.2f, %.2f\n\r",
                       this->mag_bias_factory[0], this->mag_bias_factory[1], this->mag_bias_factory[2]);
    uartUSB.write(buffer, len);

            len = snprintf(buffer, sizeof(buffer),"\n\racc_resolution  %.2f\n\r", this->acc_resolution);
    uartUSB.write(buffer, len);

                len = snprintf(buffer, sizeof(buffer), "\n\rgyro_resolution  %.2f\n\r", this->gyro_resolution);
    uartUSB.write(buffer, len);

                len = snprintf(buffer, sizeof(buffer), "\n\rmag_resolution  %.2f\n\r", this->mag_resolution );
    uartUSB.write(buffer, len);



}

/** 
* @brief
* 
* @param 
*/
acquiringData::acquiringData (IMU * imu, uint8_t mpuI2CAddr) {
    this->imu = imu;
    this->b_verbose = true;
    this->mpuI2CAddr = mpuI2CAddr;

    this->quat_filter =  new QuaternionFilter ();
    MPU9250Setting setting = this->imu->getMPUSettings();
    this->acc_resolution = this->imu->get_acc_resolution(setting.accel_fs_sel);
    this->gyro_resolution = this->imu->get_gyro_resolution(setting.gyro_fs_sel);
    this->mag_resolution = this->imu->get_mag_resolution(setting.mag_output_bits);

    this->imu->get_mag_scale(this->mag_scale[0], this->mag_scale[1], this->mag_scale[2]);
    this->imu->get_mag_bias(this->mag_bias[0], this->mag_bias[1], this->mag_bias[2]);
    this->imu->get_mag_bias_factory(this->mag_bias_factory[0], this->mag_bias_factory[1], 
    this->mag_bias_factory[2]);

}


/** 
* @brief
* 
* @param 
*/
acquiringData::~acquiringData ( ) {
    this->imu = nullptr;
    delete this->quat_filter;
    this->quat_filter = nullptr;
}

void acquiringData::initialize(I2CInterfaceHandler * interface, 
NonBlockingDelay * refresh) {
   
 }

bool acquiringData::calibrate (I2CInterfaceHandler * interface, 
NonBlockingDelay * refresh) {
    return false; 
}

bool acquiringData::obtainInertialData(I2CInterfaceHandler * interface, char * newInertialData) {
    static bool resetQuatFilter;

    if (! (interface->read_byte(this->mpuI2CAddr, INT_STATUS) & 0x01)) {
        return false; 
    } 

    if (resetQuatFilter == true) {
        this->quat_filter->resetQuaternionFilter();
        resetQuatFilter = false;
    }

    this->update_accel_gyro (interface);
    this->update_mag (interface);

    // Madgwick function needs to be fed North, East, and Down direction like
    // (AN, AE, AD, GN, GE, GD, MN, ME, MD)
    // Accel and Gyro direction is Right-Hand, X-Forward, Z-Up
    // Magneto direction is Right-Hand, Y-Forward, Z-Down
    // So to adopt to the general Aircraft coordinate system (Right-Hand, X-Forward, Z-Down),
    // we need to feed (ax, -ay, -az, gx, -gy, -gz, my, -mx, mz)
    // but we pass (-ax, ay, az, gx, -gy, -gz, my, -mx, mz)
    // because gravity is by convention positive down, we need to ivnert the accel data

    // get quaternion based on aircraft coordinate (Right-Hand, X-Forward, Z-Down)
    // acc[mg], gyro[deg/s], mag [mG]
    // gyro will be convert from [deg/s] to [rad/s] inside of this function
    // quat_filter.update(-a[0], a[1], a[2], g[0] * DEG_TO_RAD, -g[1] * DEG_TO_RAD, -g[2] * DEG_TO_RAD, m[1], -m[0], m[2], q);

    float an = -this->a[0];
    float ae = +this->a[1];
    float ad = +this->a[2];
    float gn = +this->g[0] * DEG_TO_RAD;
    float ge = -this->g[1] * DEG_TO_RAD;
    float gd = -this->g[2] * DEG_TO_RAD;
    float mn = +this->m[1];
    float me = -this->m[0];
    float md = +this->m[2];

    float oldq [4];
    oldq [0] = this->q[0];
    oldq [1] = this->q[1];
    oldq [2] = this->q[2];
    oldq [3] = this->q[3];

    for (size_t i = 0; i < this->n_filter_iter; ++i) {
        this->quat_filter->update(an, ae, ad, gn, ge, gd, mn, me, md, this->q);
    }

    if (this->update_rpy(this->q[0], this->q[1], this->q[2], this->q[3]) == false) {
        this->q[0] = oldq [0];
        this->q [1] = oldq [1];
        this->q[2] = oldq [2];
        this->q[3] = oldq [3];
    }

    this->print_roll_pitch_yaw ();
    this->promAcumulatedMeasurement();
    if (this->samplesCounter == 3100) {
        char buffer[128];
        int len = snprintf(buffer, sizeof(buffer),
                        "\n\rax: %.2f, ay: %.2f, az: %.2f Yaw %.2f, Pitch  %.2f, Roll: %.2f \n\r",
                            this->aAccumlated [0], this->aAccumlated [1], this->aAccumlated [2],
                        this->anglesAccumlated [2], this->anglesAccumlated [1], this->anglesAccumlated [0]);
        uartUSB.write(buffer, len);

        len = snprintf(buffer, sizeof(buffer),
                        "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
                            this->aAccumlated [0], this->aAccumlated [1], this->aAccumlated [2],
                        this->anglesAccumlated [2], this->anglesAccumlated [1], this->anglesAccumlated [0]);
        strcpy (newInertialData, buffer);
        this->clearAcumulatedMeasurement();
        resetQuatFilter = true;
        return true;
    }
    return false;
 };


void acquiringData::obtainTemperature(I2CInterfaceHandler * interface, float * newTemperature) {
    this->temperature_count = this->read_temperature_data(interface);               // Read the adc values
    this->temperature = ((float)this->temperature_count) / 333.87 + 21.0;  // Temperature in degrees Centigrade
    *newTemperature = this->temperature;
 }

//=====[Implementations of private methods]==================================

void acquiringData::update_accel_gyro (I2CInterfaceHandler * interface) {
    int16_t raw_acc_gyro_data[7];        // used to read all 14 bytes at once from the MPU9250 accel/gyro
    this->read_accel_gyro(raw_acc_gyro_data, interface);  // INT cleared on any read

    // Now we'll calculate the accleration value into actual g's
    this->a[0] = (float)raw_acc_gyro_data[0] * this->acc_resolution;  // get actual g value, this depends on scale being set
    this->a[1] = (float)raw_acc_gyro_data[1] * this->acc_resolution;
    this->a[2] = (float)raw_acc_gyro_data[2] * this->acc_resolution;

    this->temperature_count = raw_acc_gyro_data[3];                  // Read the adc values
    this->temperature = ((float)temperature_count) / 333.87 + 21.0;  // Temperature in degrees Centigrade

    // Calculate the gyro value into actual degrees per second
    this->g[0] = (float)raw_acc_gyro_data[4] * this->gyro_resolution;  // get actual gyro value, this depends on scale being set
    this->g[1] = (float)raw_acc_gyro_data[5] * this->gyro_resolution;
    this->g[2] = (float)raw_acc_gyro_data[6] * this->gyro_resolution;
}

void acquiringData::read_accel_gyro (int16_t* destination, I2CInterfaceHandler * interface) {
    uint8_t raw_data[14];                                                 // x/y/z accel register data stored here
    interface->read_bytes(this->mpuI2CAddr, ACCEL_XOUT_H, 14, &raw_data[0]);             // Read the 14 raw data registers into data array
    destination[0] = ((int16_t)raw_data[0] << 8) | (int16_t)raw_data[1];  // Turn the MSB and LSB into a signed 16-bit value
    destination[1] = ((int16_t)raw_data[2] << 8) | (int16_t)raw_data[3];
    destination[2] = ((int16_t)raw_data[4] << 8) | (int16_t)raw_data[5];
    destination[3] = ((int16_t)raw_data[6] << 8) | (int16_t)raw_data[7];
    destination[4] = ((int16_t)raw_data[8] << 8) | (int16_t)raw_data[9];
    destination[5] = ((int16_t)raw_data[10] << 8) | (int16_t)raw_data[11];
    destination[6] = ((int16_t)raw_data[12] << 8) | (int16_t)raw_data[13];
}

void acquiringData::update_mag(I2CInterfaceHandler * interface) {
    int16_t mag_count[3] = {0, 0, 0};  // Stores the 16-bit signed magnetometer sensor output

    // Read the x/y/z adc values
    if (this->read_mag(mag_count, interface)) {
        // Calculate the magnetometer values in milliGauss
        // Include factory calibration per data sheet and user environmental corrections
        // mag_bias is calcurated in 16BITS
        float bias_to_current_bits = this->mag_resolution / this->imu->get_mag_resolution(MAG_OUTPUT_BITS::M16BITS);
        this->m[0] = (float)(mag_count[0] * this->mag_resolution * this->mag_bias_factory[0] 
        - this->mag_bias[0] * bias_to_current_bits) * this->mag_scale[0];  // get actual magnetometer value, this depends on scale being set
        this->m[1] = (float)(mag_count[1] * this->mag_resolution * this->mag_bias_factory[1] 
        - this->mag_bias[1] * bias_to_current_bits) * this->mag_scale[1];
        this->m[2] = (float)(mag_count[2] * this->mag_resolution * this->mag_bias_factory[2] 
        - this->mag_bias[2] * bias_to_current_bits) * this->mag_scale[2];
    }
}

bool acquiringData::read_mag(int16_t* destination, I2CInterfaceHandler * interface) {
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

int16_t acquiringData::read_temperature_data (I2CInterfaceHandler * interface) {
    uint8_t raw_data[2];                                    // x/y/z gyro register data stored here
    interface->read_bytes(this->mpuI2CAddr, TEMP_OUT_H, 2, &raw_data[0]);  // Read the two raw data registers sequentially into data array
    return ((int16_t)raw_data[0] << 8) | raw_data[1];       // Turn the MSB and LSB into a 16-bit value
}

bool acquiringData::update_rpy(float qw, float qx, float qy, float qz) {
      // Define output variables from updated quaternion---these are Tait-Bryan angles, commonly used in aircraft orientation.
    // In this coordinate system, the positive z-axis is down toward Earth.
    // Yaw is the angle between Sensor x-axis and Earth magnetic North (or true North if corrected for local declination, looking down on the sensor positive yaw is counterclockwise.
    // Pitch is angle between sensor x-axis and Earth ground plane, toward the Earth is positive, up toward the sky is negative.
    // Roll is angle between sensor y-axis and Earth ground plane, y-axis up is positive roll.
    // These arise from the definition of the homogeneous rotation matrix constructed from quaternions.
    // Tait-Bryan angles as well as Euler angles are non-commutative; that is, the get the correct orientation the rotations must be
    // applied in the correct order which for this configuration is yaw, pitch, and then roll.
    // For more see http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles which has additional links.
    float a12, a22, a31, a32, a33;  // rotation matrix coefficients for Euler angles and gravity components
    float yaw, roll, pitch;
    static bool firstentry = true;
  //  static float oldYaw, oldRoll, oldPitch;
    
    a12 = 2.0f * (qx * qy + qw * qz);
    a22 = qw * qw + qx * qx - qy * qy - qz * qz;
    a31 = 2.0f * (qw * qx + qy * qz);
    a32 = 2.0f * (qx * qz - qw * qy);
    a33 = qw * qw - qx * qx - qy * qy + qz * qz;
    roll = atan2f(a31, a33);  //Roll
    pitch = -asinf(a32);       //Pitch
    yaw = atan2f(a12, a22);  // yaw
    roll *= 180.0f / PI;
    pitch *= 180.0f / PI;
    yaw *= 180.0f / PI;

    //yaw += -16.91;
    //rpy[2] += magnetic_declination;

    if (yaw >= +180.f)
        yaw -= 360.f;
    else if (yaw < -180.f)
        yaw += 360.f;

    lin_acc[0] = a[0] + a31;
    lin_acc[1] = a[1] + a32;
    lin_acc[2] = a[2] - a33;

    if ( firstentry == false){
        if ( abs (rpy[2] - yaw) < 0.04 &&  abs (rpy[1] - pitch) < 0.04  &&  abs (rpy[0] - roll) < 0.04) {
            return false;
        }
    }
    


    firstentry = false;
    rpy[2] = yaw;
    rpy[1] = pitch;
    rpy[0] = roll;
    return true;
}

void acquiringData::promAcumulatedMeasurement() {
    this->samplesCounter++;

    if (this->samplesCounter > 100) {
        int samplesUsed = this->samplesCounter - 99; // Asegura conteo correcto

        // Promediar aceleración
        for (int i = 0; i < 3; i++) {
            this->aAccumlated[i] += (this->a[i] - this->aAccumlated[i]) / samplesUsed;
            this->gAccumlated[i] += (this->g[i] - this->gAccumlated[i]) / samplesUsed;
            this->mAccumlated[i] += (this->m[i] - this->mAccumlated[i]) / samplesUsed;
        }

        // Promediar ángulos correctamente usando seno y coseno
        static float rollSinAccum = 0, rollCosAccum = 0;
        static float pitchSinAccum = 0, pitchCosAccum = 0;
        static float yawSinAccum = 0, yawCosAccum = 0;

        float rollSin = sin(this->rpy[0] * PI / 180.0);
        float rollCos = cos(this->rpy[0] * PI / 180.0);
        float pitchSin = sin(this->rpy[1] * PI / 180.0);
        float pitchCos = cos(this->rpy[1] * PI / 180.0);
        float yawSin = sin(this->rpy[2] * PI / 180.0);
        float yawCos = cos(this->rpy[2] * PI / 180.0);

        rollSinAccum  += (rollSin  - rollSinAccum)  / samplesUsed;
        rollCosAccum  += (rollCos  - rollCosAccum)  / samplesUsed;
        pitchSinAccum += (pitchSin - pitchSinAccum) / samplesUsed;
        pitchCosAccum += (pitchCos - pitchCosAccum) / samplesUsed;
        yawSinAccum   += (yawSin   - yawSinAccum)   / samplesUsed;
        yawCosAccum   += (yawCos   - yawCosAccum)   / samplesUsed;

        this->anglesAccumlated[0] = atan2(rollSinAccum, rollCosAccum) * 180.0 / PI;
        this->anglesAccumlated[1] = atan2(pitchSinAccum, pitchCosAccum) * 180.0 / PI;
        this->anglesAccumlated[2] = atan2(yawSinAccum, yawCosAccum) * 180.0 / PI;

        // Promediar temperatura
        this->temperatureAccumlated += (this->temperature - this->temperatureAccumlated) / samplesUsed;
    }
}


void acquiringData::clearAcumulatedMeasurement() {
    samplesCounter = 0;
    aAccumlated [0] = 0;
    aAccumlated [1] = 0;
    aAccumlated [2] = 0;
    gAccumlated [0] = 0;
    gAccumlated [1] = 0;
    gAccumlated [2] = 0;
    mAccumlated [0] = 0;
    mAccumlated [1] = 0;
    mAccumlated [2] = 0;
    anglesAccumlated [0] = 0;
    anglesAccumlated [1] = 0;
    anglesAccumlated [2] = 0;
    temperatureAccumlated  = 0;
}

void acquiringData::print_roll_pitch_yaw() {
    char buffer[128];
    int len = snprintf(buffer, sizeof(buffer),
                       "Yaw, Pitch, Roll: %.2f, %.2f, %.2f\n\r",
                       this->rpy[2], this->rpy[1], this->rpy[0]);
    uartUSB.write(buffer, len);
}