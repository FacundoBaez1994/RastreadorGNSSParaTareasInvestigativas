
#ifndef _ADAFRUIT_BNO08x_H
#define _ADAFRUIT_BNO08x_H

#include "mbed.h"
#include "sh2.h"
#include "sh2_SensorValue.h"
#include "sh2_err.h"
#include "Adafruit_I2CDevice.h"
#include "arm_book_lib.h"

// Dirección I2C por defecto
#define BNO08x_I2CADDR_DEFAULT 0x4A

// Actividades adicionales no definidas en SH-2
#define PAC_ON_STAIRS 8
#define PAC_OPTION_COUNT 9

/*!
 * @brief Clase para manejar el sensor BNO08x
 */
class Adafruit_BNO08x {
public:
  Adafruit_BNO08x(PinName reset_pin = NC);
  ~Adafruit_BNO08x();

  bool begin_I2C(uint8_t i2c_addr = BNO08x_I2CADDR_DEFAULT,
                 I2C *wire = nullptr, int32_t sensor_id = 0);
  void hardwareReset(void);
  bool wasReset(void);

  bool enableReport(sh2_SensorId_t sensor, uint32_t interval_us = 10000);
  bool getSensorEvent(sh2_SensorValue_t *value);

  sh2_ProductIds_t prodIds;

  void hal_hardwareReset(void);

protected:
 DigitalOut * reset;  // Atributo para manejar el pin de reset
  virtual bool _init(int32_t sensor_id);

  sh2_Hal_t _HAL;
};

#endif

