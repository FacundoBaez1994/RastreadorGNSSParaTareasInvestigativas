//=====[#include guards - begin]===============================================
#ifndef Adafruit_I2CDevice_h
#define Adafruit_I2CDevice_h

//==================[Libraries]===============================================
#include "mbed.h"

//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================================

/**
 * @class Adafruit_I2CDevice
 * @brief manages communication with an I2C device.
 * @details This class provides methods to communicate with I2C devices using the
 * mbed I2C interface. It supports initialization, address detection, reading,
 * writing, and combined write-then-read operations.
 */
class Adafruit_I2CDevice {
public:
//=====[Declaration of public methods]=========================================
    /**
    *    @brief  Create an I2C device at a given address
    *    @param  addr The 7-bit I2C address for the device
    *    @param  theWire The I2C bus to use, defaults to nullptr (debe pasar un I2C válido)
    */
    Adafruit_I2CDevice(uint8_t addr, I2C* theWire = nullptr);

    
    /**
    *    @brief  eliminates an I2C device
    */
    ~Adafruit_I2CDevice ();

    /**
    *    @brief  Returns the 7-bit address of this device
    *    @return The 7-bit address of this device
    */
    uint8_t address(void);

    /**
    *    @brief  Initializes and does basic address detection
    *    @param  addr_detect Whether we should attempt to detect the I2C address
    * with a scan. 99% of sensors/devices don't mind, but once in a while they
    * don't respond well to a scan!
    *    @return True if I2C initialized and a device with the addr found
    */
    bool begin(bool addr_detect = true);

    /**
     * @brief Marks the device as no longer in use.
     */
    void end(void);

    /**
     * @brief Checks if the device responds on the I2C bus.
     * @return true if detected, false otherwise.
     */
    bool detected(void);

    /**
    *    @brief  Read from I2C into a buffer from the I2C device.
    *    Cannot be more than maxBufferSize() bytes.
    *    @param  buffer Pointer to buffer of data to read into
    *    @param  len Number of bytes from buffer to read.
    *    @param  stop Whether to send an I2C STOP signal on read
    *    @return True if read was successful, otherwise false.
    */
    bool read(uint8_t* buffer, size_t len, bool stop = true);

    /**
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
    bool write(const uint8_t *buffer, size_t len, bool stop = true,
                const uint8_t *prefix_buffer = nullptr, size_t prefix_len = 0);

    /**
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
    bool write_then_read(const uint8_t *write_buffer, size_t write_len,
                        uint8_t *read_buffer, size_t read_len,
                        bool stop = false);

    /**
    *    @brief  Change the I2C clock speed to desired (relies on
    *    underlying Wire support!
    *    @param desiredclk The desired I2C SCL frequency
    *    @return True if this platform supports changing I2C speed.
    *    Not necessarily that the speed was achieved!
    */
    bool setSpeed(uint32_t desiredclk);

    /**
     * @brief Returns the maximum buffer size allowed for I2C operations.
     * @return Maximum buffer size in bytes.
     */
    size_t maxBufferSize() { return _maxBufferSize; }

private:
//=====[Declaration of privates atributes]=========================================
    uint8_t _addr;           //!< 7-bit I2C address of the device.
    I2C* _wire;              //!< Pointer to the I2C interface.
    bool _begun;             //!< Tracks whether the device has been initialized.
    size_t _maxBufferSize;   //!< Maximum I2C buffer size.

//=====[Declaration of privates methods]=========================================
    /**
     * @brief Internal low-level read function.
     * @param buffer Pointer to buffer where data will be stored.
     * @param len Number of bytes to read.
     * @param stop If true, sends an I2C STOP after the read.
     * @return true if read was successful.
     */
    bool _read(uint8_t *buffer, size_t len, bool stop);
};

//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]================================================= 
#endif // Adafruit_I2CDevice_h
