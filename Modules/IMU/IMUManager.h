//=====[#include guards - begin]===============================================
#ifndef _IMU_MANAGER_H
#define _IMU_MANAGER_H

#include "mbed.h"
#include "Adafruit_I2CDevice.h"
#include "Adafruit_BNO08x.h"
#include "arm_book_lib.h"

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================
/**
 * @enum deviceMotionStatus_t
 * @brief Enumerates the motion status of the device.
 */
typedef enum {
    DEVICE_ON_MOTION,    //!< The device is moving.
    DEVICE_STATIONARY,   //!< The device is stationary.
} deviceMotionStatus_t;

/**
 * @struct euler_t
 * @brief Stores orientation angles in degrees.
 */
struct euler_t {
  float yaw;    //!< Rotation around the vertical axis (Z).
  float pitch;  //!< Rotation around the lateral axis (Y).
  float roll;   //!< Rotation around the longitudinal axis (X).
};

/**
 * @struct acceleration_t
 * @brief Stores acceleration values on each axis in m/s².
 */
struct acceleration_t {
  float ax; //!< Acceleration in the X-axis.
  float ay; //!< Acceleration in the Y-axis.
  float az; //!< Acceleration in the Z-axis.
};

/**
 * @struct IMUData_t
 * @brief Holds sensor data including orientation, acceleration, and metadata.
 */
struct IMUData_t {
  euler_t angles;                 //!< Euler angles (yaw, pitch, roll).
  acceleration_t acceleration;   //!< Linear acceleration values.
  uint8_t status;                //!< Sensor status byte.
  char * timestamp;              //!< Timestamp of the measurement (optional).
  int timeBetweenSamples;        //!< Time elapsed between samples in ms.
};

//=====[Declaration of public classes]=========================
/**
 * @class IMUManager
 * @brief Manages initialization, innertial data retrieval and status monitoring of the BNO08x IMU sensor.
 * @details Provides high-level abstraction for working with the BNO08x sensor over I2C.
 * Supports initialization, sensor fusion (Euler angles), linear acceleration measurement,
 * motion status classification (moving/stationary), and running average filtering.
 */
class IMUManager {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructs an IMUManager instance.
    */
    IMUManager();
    
    /**
     * @brief Destroys the IMUManager instance and frees resources.
     */
    ~IMUManager();

    /**
     * @brief Enables a specific sensor report with a given update interval.
     * @param reportType The type of sensor report (e.g. SH2_ROTATION_VECTOR).
     * @param report_interval The interval in microseconds between sensor updates.
     * @return true if the report was successfully enabled.
     */
    bool setReports(sh2_SensorId_t reportType, long report_interval);

    /**
     * @brief Retrieves filtered acceleration and orientation data.
     * @param inertialMeasures Pointer to the structure where data will be stored.
     * @return true if valid data has been collected.
     */
    bool obtainInertialMeasures (IMUData_t* inertialMeasures);

    /**
     * @brief Determines if the device is currently moving or stationary.
     * @param currentMotionStatus Pointer to a variable that will be set to the current motion state.
     */
    void checkStability (deviceMotionStatus_t* currentMotionStatus);

    /**
     * @brief Initializes the IMU and prepares it for data acquisition.
     * @return true if initialization succeeded.
     */
    bool initialize (void);

private:
    /**
     * @brief Returns the square of a floating-point number.
     * @param x Input value.
     * @return x squared.
     */
    inline float sq(float x);

    /**
     * @brief Converts a quaternion to Euler angles.
     * @param qr Real component.
     * @param qi i component.
     * @param qj j component.
     * @param qk k component.
     * @param ypr Pointer to Euler angle struct to store result.
     * @param degrees True to return angles in degrees; false for radians.
     */
    void quaternionToEuler(float qr, float qi, float qj, float qk, euler_t* ypr, bool degrees);

    /**
     * @brief Converts a SH2 rotation vector to Euler angles.
     * @param rotational_vector Pointer to SH2 rotation vector.
     * @param ypr Pointer to Euler angle struct to store result.
     * @param degrees True to return angles in degrees.
     */
    void quaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, euler_t* ypr, bool degrees);

    /**
     * @brief Converts a SH2 gyro-integrated rotation vector to Euler angles.
     * @param rotational_vector Pointer to SH2 gyro-integrated vector.
     * @param ypr Pointer to Euler angle struct to store result.
     * @param degrees True to return angles in degrees.
     */
    void quaternionToEulerGI(sh2_GyroIntegratedRV_t* rotational_vector, euler_t* ypr, bool degrees);

    /**
     * @brief Computes the running average of Euler angle readings.
     */
    void promEulerMeasurement(void);

    /**
     * @brief Computes the running average of acceleration readings.
     */
    void promAccelMeasurement(void);

    /**
     * @brief Clears the internal accumulators used for averaging.
     */
    void clearAcumulatedMeasurement(void);

    //=====[Private Attributes]================================================

    deviceMotionStatus_t currentMotionStatus = DEVICE_ON_MOTION; //!< Current motion classification.
    int motionCounter = 0;     //!< Internal counter for motion detection.
    int stillnessCounter = 0;  //!< Internal counter for stationary detection.

    Adafruit_BNO08x * bno08x;  //!< Pointer to BNO08x sensor driver.
    I2C * i2c;                 //!< Pointer to I2C interface instance.
    sh2_SensorValue_t sensorValue; //!< Last sensor reading.
    sh2_SensorId_t reportType;     //!< Currently enabled report type.
    long reportIntervalUs;         //!< Report interval in microseconds.

    euler_t eulerAngles;       //!< Latest raw Euler angles.
    acceleration_t acceleration; //!< Latest raw acceleration values.

    // Averaging buffers
    euler_t promEulerAngles;     //!< Averaged Euler angles.
    acceleration_t promAcceleration; //!< Averaged acceleration values.
    int samplesCounterAccel;     //!< Count of accumulated accel samples.
    int samplesCounterAngles;    //!< Count of accumulated angle samples.
};
//=====[#include guards - end]=================================================
#endif // _IMU_MANAGER_H