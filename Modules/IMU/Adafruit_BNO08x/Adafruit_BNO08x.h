
#ifndef _ADAFRUIT_BNO08x_H
#define _ADAFRUIT_BNO08x_H

#include "mbed.h"
#include "sh2.h"
#include "sh2_SensorValue.h"
#include "sh2_err.h"
#include "Adafruit_I2CDevice.h"
#include "arm_book_lib.h"

// default direction
#define BNO08x_I2CADDR_DEFAULT 0x4A

// Actividades adicionales no definidas en SH-2
#define PAC_ON_STAIRS 8
#define PAC_OPTION_COUNT 9

/**
 * @class Adafruit_BNO08x
 * @brief Driver class for the BNO08x 9-DOF IMU sensor.
 * @details This class provides methods to initialize and interface with the
 * BNO08x using I2C. It allows enabling specific sensor reports and retrieving
 * sensor events.
 */
class Adafruit_BNO08x {
public:
    /**
    * @brief Construct a new Adafruit_BNO08x::Adafruit_BNO08x object
    * @param reset_pin The arduino pin # connected to the BNO Reset pin
    */
    Adafruit_BNO08x(PinName reset_pin = NC);

    /**
    * @brief Destroy the Adafruit_BNO08x::Adafruit_BNO08x object
    *
    */
    ~Adafruit_BNO08x();

    /*!
    *    @brief  Sets up the hardware and initializes I2C
    *    @param  i2c_address
    *            The I2C address to be used.
    *    @param  wire
    *            The Wire object to be used for I2C connections.
    *    @param  sensor_id
    *            The unique ID to differentiate the sensors from others
    *    @return True if initialization was successful, otherwise false.
    */
    bool begin_I2C(uint8_t i2c_addr = BNO08x_I2CADDR_DEFAULT,
                    I2C *wire = nullptr, int32_t sensor_id = 0);

    /**
     * @brief Resets the sensor by toggling the reset pin
     */
    void hardwareReset(void);


    /**
     * @brief Checks whether a reset has occurred
     * @return true if a reset occurred since the last check
     */
    bool wasReset(void);

    /**
    * @brief Enable the given report type
    *
    * @param sensorId The report ID to enable
    * @param interval_us The update interval for reports to be generated, in
    * microseconds
    * @return true: success false: failure
    */
    bool enableReport(sh2_SensorId_t sensor, uint32_t interval_us = 10000);

    /**
    * @brief Fill the given sensor value object with a new report
    *
    * @param value Pointer to an sh2_SensorValue_t struct to fil
    * @return true: The report object was filled with a new report
    * @return false: No new report available to fill
    */
    bool getSensorEvent(sh2_SensorValue_t *value);


    sh2_ProductIds_t prodIds; //!< Holds product ID information returned by the sensor.

    /**
     * @brief (Internal) Forces a hardware reset
     */
    void hal_hardwareReset(void);

protected:
    DigitalOut * reset;  //!< Reset pin interface (controlled digitally)
    sh2_Hal_t _HAL;      //!< SH2 HAL configuration structure

    /*!  @brief Initializer for post i2c/spi init
    *   @param sensor_id Optional unique ID for the sensor set
    *   @returns True if chip identified and initialized
    */
    virtual bool _init(int32_t sensor_id);
};

#endif

