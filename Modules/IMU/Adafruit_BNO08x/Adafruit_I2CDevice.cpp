//=====[Libraries]=============================================================
#include "Adafruit_I2CDevice.h"
#include "Debugger.h"

//=====[Declaration of private defines]========================================

Adafruit_I2CDevice::Adafruit_I2CDevice(uint8_t addr, I2C *theWire) {
    _addr = addr;
    _wire = theWire;
    _begun = false;
    _maxBufferSize = 32; 
}

Adafruit_I2CDevice::~Adafruit_I2CDevice() {
  // No resources to liberate
}


/**
 * @brief  Initializes and does basic address detection
 * @param  addr_detect Whether we should attempt to detect the I2C address
 * with a scan. 99% of sensors/devices don't mind, but once in a while they
 * don't respond well to a scan!
 *    @return True if I2C initialized and a device with the addr found
 */
bool Adafruit_I2CDevice::begin(bool addr_detect) {
  if (_wire == nullptr) {
    return false; 
  }

  _begun = true;

  if (addr_detect) {
    return this->detected();
  }
  return true;
}

void Adafruit_I2CDevice::end(void) {
    _begun = false;
}


bool Adafruit_I2CDevice::detected(void) {
    // Init I2C if not done yet
    if (!_begun && !begin()) {
        return false;
    }

    char log[100];

    snprintf(log, sizeof(log), "Address 0x%02X", _addr);
    uartUSB.write(log, strlen(log));

    // In order to force a write, send 0 bytes
    int result = _wire->write(_addr << 1, nullptr, 0);

    if (result == 0) {  
        snprintf(log, sizeof(log), " Detected\n\r");
        uartUSB.write(log, strlen(log));
        return true;
    } else {
        snprintf(log, sizeof(log), " Not detected\n\r");
        uartUSB.write(log, strlen(log));
        return false;
    }
}





/*!
 *    @brief  Write a buffer or two to the I2C device. Cannot be more than
 * maxBufferSize() bytes.
 *    @param  buffer Pointer to buffer of data to write. This is const to
 *            ensure the content of this buffer doesn't change.
 *    @param  len Number of bytes from buffer to write
 *    @param  prefix_buffer Pointer to optional array of data to write before
 * buffer. Cannot be more than maxBufferSize() bytes. This is const to
 *            ensure the content of this buffer doesn't change.
 *    @param  prefix_len Number of bytes from prefix buffer to write
 *    @param  stop Whether to send an I2C STOP signal on write
 *    @return True if write was successful, otherwise false.
 */
bool Adafruit_I2CDevice::write(const uint8_t *buffer, size_t len, bool stop,
                               const uint8_t *prefix_buffer,
                               size_t prefix_len) {
    if ((len + prefix_len) > maxBufferSize()) {
        char log[100];
        snprintf(log, sizeof(log), "\tI2CDevice could not write such a large buffer\n\r");
        uartUSB.write(log, strlen(log));
        return false;
    }

    uint8_t temp[len + prefix_len];
    size_t idx = 0;

    if ((prefix_len != 0) && (prefix_buffer != nullptr)) {
        memcpy(&temp[idx], prefix_buffer, prefix_len);
        idx += prefix_len;
    }

    memcpy(&temp[idx], buffer, len);

    int ret = _wire->write(_addr << 1, (char *)temp, len + prefix_len, !stop);

#ifdef DEBUG_SERIAL
    char log[200];
    snprintf(log, sizeof(log), "\tI2CWRITE @ 0x%02X :: ", _addr);
    uartUSB.write(log, strlen(log));
    
    for (size_t i = 0; i < (len + prefix_len); i++) {
        snprintf(log, sizeof(log), "0x%02X ", temp[i]);
        uartUSB.write(log, strlen(log));
    }

    if (stop) {
        snprintf(log, sizeof(log), "\tSTOP\n\r");
        uartUSB.write(log, strlen(log));
    } else {
        snprintf(log, sizeof(log), "\n\r");
        uartUSB.write(log, strlen(log));
    }
#endif

    if (ret == 0) {
#ifdef DEBUG_SERIAL
        snprintf(log, sizeof(log), "\tSent OK\n\r");
        uartUSB.write(log, strlen(log));
#endif
        return true;
    } else {
#ifdef DEBUG_SERIAL
        snprintf(log, sizeof(log), "\tFailed to send!\n\r");
        uartUSB.write(log, strlen(log));
#endif
        return false;
    }
}

bool Adafruit_I2CDevice::read(uint8_t *buffer, size_t len, bool stop) {
  size_t pos = 0;
  while (pos < len) {
    size_t read_len = ((len - pos) > maxBufferSize()) ? maxBufferSize() : (len - pos);
    bool read_stop = (pos < (len - read_len)) ? false : stop;

    if (!_read(buffer + pos, read_len, read_stop)) {
      return false;
    }

    pos += read_len;
  }
  return true;
}



bool Adafruit_I2CDevice::_read(uint8_t *buffer, size_t len, bool stop) {
  if (len > maxBufferSize()) {
#if defined(DEBUG_SERIAL)
    uartUSB.write("I2CDevice::_read: buffer demasiado grande\r\n", 41);
#endif
    return false;
  }


  int ret = _wire->read(_addr << 1, (char *)buffer, len, !stop);

  if (ret != 0) {
#if defined(DEBUG_SERIAL)
    uartUSB.write("I2CDevice::_read: fallo al leer desde el dispositivo\r\n", 53);
#endif
    return false;
  }

#if defined(DEBUG_SERIAL)
  char debugMsg[128];
  int offset = snprintf(debugMsg, sizeof(debugMsg), "\tI2CREAD @ 0x%02X :: ", _addr);
  for (size_t i = 0; i < len && offset < sizeof(debugMsg) - 6; ++i) {
    offset += snprintf(debugMsg + offset, sizeof(debugMsg) - offset, "0x%02X, ", buffer[i]);
    if (i % 32 == 31) {
      uartUSB.write(debugMsg, offset);
      uartUSB.write("\r\n", 2);
      offset = 0;
    }
  }
  if (offset > 0) {
    uartUSB.write(debugMsg, offset);
    uartUSB.write("\r\n", 2);
  }
#endif

  return true;
}

bool Adafruit_I2CDevice::write_then_read(const uint8_t *write_buffer,
                                         size_t write_len,
                                         uint8_t *read_buffer,
                                         size_t read_len, bool stop) {

  if (_wire->write(_addr << 1, (const char *)write_buffer, write_len, true) != 0) {
    return false;
  }


  if (_wire->read(_addr << 1, (char *)read_buffer, read_len, !stop) != 0) {
    return false;
  }

  return true;
}



uint8_t Adafruit_I2CDevice::address(void) { return _addr; }


bool Adafruit_I2CDevice::setSpeed(uint32_t desiredclk) {
#if defined(TARGET_LIKE_MBED)
  _wire->frequency(desiredclk);
  return true;
#else
  (void)desiredclk;
  return false;
#endif
}
