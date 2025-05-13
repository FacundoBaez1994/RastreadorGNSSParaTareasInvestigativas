#include "Adafruit_I2CDevice.h"
#include "Debugger.h"


/*!
 *    @brief  Create an I2C device at a given address
 *    @param  addr The 7-bit I2C address for the device
 *    @param  theWire The I2C bus to use, defaults to nullptr (debe pasar un I2C válido)
 */
Adafruit_I2CDevice::Adafruit_I2CDevice(uint8_t addr, I2C *theWire) {
    _addr = addr;
    _wire = theWire;
    _begun = false;
    _maxBufferSize = 32; //
}


/*!
 *    @brief  eliminates an I2C device
 */
Adafruit_I2CDevice::~Adafruit_I2CDevice() {
  // No hay recursos dinámicos que liberar actualmente
}



/*!
 *    @brief  Initializes and does basic address detection
 *    @param  addr_detect Whether we should attempt to detect the I2C address
 * with a scan. 99% of sensors/devices don't mind, but once in a while they
 * don't respond well to a scan!
 *    @return True if I2C initialized and a device with the addr found
 */
bool Adafruit_I2CDevice::begin(bool addr_detect) {
  if (_wire == nullptr) {
    return false;  // Si no se pasó un bus válido
  }

  _begun = true;

  if (addr_detect) {
    return this->detected();
  }
  return true;
}

// Eliminar luego al metodo no aporta nada
void Adafruit_I2CDevice::end(void) {
    _begun = false;
}


bool Adafruit_I2CDevice::detected(void) {
    // Init I2C if not done yet
    if (!_begun && !begin()) {
        return false;
    }

    char log[100];

    // Formato de la dirección
    snprintf(log, sizeof(log), "Address 0x%02X", _addr);
    uartUSB.write(log, strlen(log));

    // Para forzar un write, enviamos 0 bytes
    int result = _wire->write(_addr << 1, nullptr, 0);

    if (result == 0) {  // 0 en Mbed significa éxito (ACK recibido)
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

    // Crear un buffer de transmisión combinado
    uint8_t temp[len + prefix_len];
    size_t idx = 0;

    // Primero copiamos el prefix si existe
    if ((prefix_len != 0) && (prefix_buffer != nullptr)) {
        memcpy(&temp[idx], prefix_buffer, prefix_len);
        idx += prefix_len;
    }

    // Luego copiamos los datos principales
    memcpy(&temp[idx], buffer, len);

    // Escribir todo de una sola vez
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





/*!
 *    @brief  Read from I2C into a buffer from the I2C device.
 *    Cannot be more than maxBufferSize() bytes.
 *    @param  buffer Pointer to buffer of data to read into
 *    @param  len Number of bytes from buffer to read.
 *    @param  stop Whether to send an I2C STOP signal on read
 *    @return True if read was successful, otherwise false.
 */
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

  // Mbed usa _addr << 1 porque el bit menos significativo indica lectura/escritura
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





/*!
 *    @brief  Write some data, then read some data from I2C into another buffer.
 *    Cannot be more than maxBufferSize() bytes. The buffers can point to
 *    same/overlapping locations.
 *    @param  write_buffer Pointer to buffer of data to write from
 *    @param  write_len Number of bytes from buffer to write.
 *    @param  read_buffer Pointer to buffer of data to read into.
 *    @param  read_len Number of bytes from buffer to read.
 *    @param  stop Whether to send an I2C STOP signal between the write and read
 *    @return True if write & read was successful, otherwise false.
 */
bool Adafruit_I2CDevice::write_then_read(const uint8_t *write_buffer,
                                         size_t write_len,
                                         uint8_t *read_buffer,
                                         size_t read_len, bool stop) {
  // Escribir al dispositivo con repeated start
  if (_wire->write(_addr << 1, (const char *)write_buffer, write_len, true) != 0) {
    return false;
  }

  // Leer del dispositivo, enviar STOP según el parámetro
  if (_wire->read(_addr << 1, (char *)read_buffer, read_len, !stop) != 0) {
    return false;
  }

  return true;
}


/*!
 *    @brief  Returns the 7-bit address of this device
 *    @return The 7-bit address of this device
 */
uint8_t Adafruit_I2CDevice::address(void) { return _addr; }

/*!
 *    @brief  Change the I2C clock speed to desired (relies on
 *    underlying Wire support!
 *    @param desiredclk The desired I2C SCL frequency
 *    @return True if this platform supports changing I2C speed.
 *    Not necessarily that the speed was achieved!
 */
bool Adafruit_I2CDevice::setSpeed(uint32_t desiredclk) {
#if defined(TARGET_LIKE_MBED)
  _wire->frequency(desiredclk);
  return true;
#else
  (void)desiredclk;
  return false;
#endif
}
