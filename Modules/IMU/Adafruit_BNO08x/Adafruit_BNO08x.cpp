/*!
 *  @file Adafruit_BNO08x.cpp
 *
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for the Library for the BNO08x 9-DOF Orientation IMU Fusion
 * Breakout
 *
 * 	This is a library for the Adafruit BNO08x breakout:
 * 	https://www.adafruit.com/product/4754
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section dependencies Dependencies
 *  This library depends on the Adafruit BusIO library
 *
 *  This library depends on the Adafruit Unified Sensor library
 *
 *  @section author Author
 *
 *  Bryan Siepert for Adafruit Industries
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "mbed.h"
#include "Adafruit_BNO08x.h"
#include "Debugger.h"

static Adafruit_I2CDevice *i2c_dev = nullptr;  //!< Pointer to the I2C device interface

static sh2_SensorValue_t *_sensor_value = nullptr; //!< Pointer to current sensor event (populated by interrupt handler)

static bool _reset_occurred = false; //!< Flag indicating if a reset has occurred

/**
 * @brief Writes data to the sensor over I2C
 * @param self Pointer to HAL structure (unused)
 * @param pBuffer Pointer to buffer to send
 * @param len Length of buffer
 * @return Number of bytes written
 */
static int i2chal_write(sh2_Hal_t *self, uint8_t *pBuffer, unsigned len);

/**
 * @brief Reads data from the sensor over I2C
 * @param self Pointer to HAL structure (unused)
 * @param pBuffer Pointer to destination buffer
 * @param len Length of the buffer
 * @param t_us Pointer to timestamp (optional, unused)
 * @return Number of bytes read
 */
static int i2chal_read(sh2_Hal_t *self, uint8_t *pBuffer, unsigned len,
                       uint32_t *t_us);

/**
 * @brief Closes the I2C HAL connection
 * @param self Pointer to HAL structure (unused)
 */
static void i2chal_close(sh2_Hal_t *self);

/**
 * @brief Opens the I2C HAL layer and sends a soft reset command
 * @param self Pointer to HAL structure (unused)
 * @return 0 on success, -1 on failure
 */
static int i2chal_open(sh2_Hal_t *self);
//////////////////////////////////////////////////
/**
 * @brief Returns system uptime in microseconds
 * @param self Pointer to HAL structure
 * @return Uptime in microseconds
 */
static uint32_t hal_getTimeUs(sh2_Hal_t *self);


/**
 * @brief Handles asynchronous system-level events from the sensor
 * @param cookie User data pointer (unused)
 * @param pEvent Pointer to event struct
 */
static void hal_callback(void *cookie, sh2_AsyncEvent_t *pEvent);



/**
 * @brief Callback that processes incoming sensor data
 * @param cookie User data pointer (unused)
 * @param event Pointer to raw sensor event
 */
static void sensorHandler(void *cookie, sh2_SensorEvent_t *pEvent);


/**
 * @brief Performs a hardware reset using the reset pin
 */
static void hal_hardwareReset(void);


Adafruit_BNO08x::Adafruit_BNO08x (PinName reset_pin) { 
    reset = new DigitalOut (reset_pin); 
}


Adafruit_BNO08x::~Adafruit_BNO08x(void) {
    delete reset;
}

 bool Adafruit_BNO08x::begin_I2C(uint8_t i2c_address, I2C *i2c_bus, int32_t sensor_id) {
    char log [100];

    if (i2c_dev) {
        delete i2c_dev;
    }

    i2c_dev = new Adafruit_I2CDevice(i2c_address, i2c_bus);

    if (!i2c_dev->begin()) {
        snprintf(log, sizeof(log),"I2C address not found\n\r");
        uartUSB.write(log, strlen(log));
        return false;
    }

    _HAL.open = i2chal_open;
    _HAL.close = i2chal_close;
    _HAL.read = i2chal_read;
    _HAL.write = i2chal_write;
    _HAL.getTimeUs = hal_getTimeUs;

    return _init(sensor_id);
}



bool Adafruit_BNO08x::_init(int32_t sensor_id) {
  int status;

  hardwareReset();

  // Open SH2 interface (also registers non-sensor event handler.)
  status = sh2_open(&_HAL, hal_callback, NULL);
  if (status != SH2_OK) {
    return false;
  }

  // Check connection partially by getting the product id's
  memset(&prodIds, 0, sizeof(prodIds));
  status = sh2_getProdIds(&prodIds);
  if (status != SH2_OK) {
    return false;
  }

  // Register sensor listener
  sh2_setSensorCallback(sensorHandler, NULL);

  return true;
}


void Adafruit_BNO08x::hardwareReset(void) { hal_hardwareReset(); }

bool Adafruit_BNO08x::wasReset(void) {
  bool x = _reset_occurred;
  _reset_occurred = false;

  return x;
}

bool Adafruit_BNO08x::getSensorEvent(sh2_SensorValue_t *value) {
  _sensor_value = value;

  value->timestamp = 0;

  sh2_service();

  if (value->timestamp == 0 && value->sensorId != SH2_GYRO_INTEGRATED_RV) {
    // no new events
    return false;
  }

  return true;
}


bool Adafruit_BNO08x::enableReport(sh2_SensorId_t sensorId,
                                   uint32_t interval_us) {
  static sh2_SensorConfig_t config;

  // These sensor options are disabled or not used in most cases
  config.changeSensitivityEnabled = false;
  config.wakeupEnabled = false;
  config.changeSensitivityRelative = false;
  config.alwaysOnEnabled = false;
  config.changeSensitivity = 0;
  config.batchInterval_us = 0;
  config.sensorSpecific = 0;

  config.reportInterval_us = interval_us;
  int status = sh2_setSensorConfig(sensorId, &config);

  if (status != SH2_OK) {
    return false;
  }

  return true;
}

/**************************************** I2C interface
 * ***********************************************************/

static int i2chal_open(sh2_Hal_t *self) {
  char log[100];
  uint8_t softreset_pkt[] = {5, 0, 1, 0, 1};
  bool success = false;

  for (uint8_t attempts = 0; attempts < 5; attempts++) {
    if (i2c_dev->write(softreset_pkt, 5)) {
      success = true;
      break;
    }
    ThisThread::sleep_for(30ms);
  }

  if (!success) {
    snprintf(log, sizeof(log), "i2chal_open: failed to write softreset\n\r");
    uartUSB.write(log, strlen(log));
    return -1;
  }

  ThisThread::sleep_for(300ms);
  return 0;
}


static void i2chal_close(sh2_Hal_t *self) {
  // Serial.println("I2C HAL close");
}

static int i2chal_read(sh2_Hal_t *self, uint8_t *pBuffer, unsigned len,
                       uint32_t *t_us) {


  uint8_t header[4];
  if (!i2c_dev->read(header, 4)) {
    return 0;
  }

  // Determine amount to read
  uint16_t packet_size = (uint16_t)header[0] | (uint16_t)header[1] << 8;
  // Unset the "continue" bit
  packet_size &= ~0x8000;

  size_t i2c_buffer_max = i2c_dev->maxBufferSize();

  if (packet_size > len) {
    // packet wouldn't fit in our buffer
    return 0;
  }
  // the number of non-header bytes to read
  uint16_t cargo_remaining = packet_size;
  uint8_t i2c_buffer[i2c_buffer_max];
  uint16_t read_size;
  uint16_t cargo_read_amount = 0;
  bool first_read = true;

  while (cargo_remaining > 0) {
    if (first_read) {
      read_size = min(i2c_buffer_max, (size_t)cargo_remaining);
    } else {
      read_size = min(i2c_buffer_max, (size_t)cargo_remaining + 4);
    }


    if (!i2c_dev->read(i2c_buffer, read_size)) {
      return 0;
    }

    if (first_read) {
      // The first time we're saving the "original" header, so include it in the
      // cargo count
      cargo_read_amount = read_size;
      memcpy(pBuffer, i2c_buffer, cargo_read_amount);
      first_read = false;
    } else {
      // this is not the first read, so copy from 4 bytes after the beginning of
      // the i2c buffer to skip the header included with every new i2c read and
      // don't include the header in the amount of cargo read
      cargo_read_amount = read_size - 4;
      memcpy(pBuffer, i2c_buffer + 4, cargo_read_amount);
    }
    // advance our pointer by the amount of cargo read
    pBuffer += cargo_read_amount;
    // mark the cargo as received
    cargo_remaining -= cargo_read_amount;
  }


  return packet_size;
}

static int i2chal_write(sh2_Hal_t *self, uint8_t *pBuffer, unsigned len) {
  size_t i2c_buffer_max = i2c_dev->maxBufferSize();

  uint16_t write_size = min(i2c_buffer_max, len);
  if (!i2c_dev->write(pBuffer, write_size)) {
    return 0;
  }

  return write_size;
}


/**************************************** HAL interface
 * ***********************************************************/

void  Adafruit_BNO08x::hal_hardwareReset(void) {
    this->reset->write (HIGH);
    ThisThread::sleep_for(10ms);
    this->reset->write (LOW);
    ThisThread::sleep_for(10ms);
    this->reset->write (HIGH);
}


static uint32_t hal_getTimeUs(sh2_Hal_t *self) {
  return Kernel::get_ms_count() * 1000;
}

static void hal_callback(void *cookie, sh2_AsyncEvent_t *pEvent) {
  // If we see a reset, set a flag so that sensors will be reconfigured.
  if (pEvent->eventId == SH2_RESET) {
    _reset_occurred = true;
  }
}

// Handle sensor events.
static void sensorHandler(void *cookie, sh2_SensorEvent_t *event) {
  int rc;
  char log [100];


  rc = sh2_decodeSensorEvent(_sensor_value, event);
  if (rc != SH2_OK) {
    snprintf(log, sizeof(log), "BNO08x - Error decoding sensor event\n\r");
    uartUSB.write(log, strlen(log));
    _sensor_value->timestamp = 0;
    return;
  }
}
