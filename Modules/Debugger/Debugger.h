#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "mbed.h"
#include <cstdarg>

/**
 * @brief Simple UART debugger wrapper to enable/disable logging.
 */
class Debugger {
public:
    /**
     * @brief Construct a new Debugger object.
     * @param tx Pin for UART TX
     * @param rx Pin for UART RX
     * @param baud Baudrate
     */
    Debugger(PinName tx, PinName rx, int baud = 115200);

    /**
     * @brief Enable log output.
     */
    void enable();

    /**
     * @brief Disable log output.
     */
    void disable();

    /**
     * @brief Print formatted text to UART (like printf).
     * @param fmt C-style format string
     * @param ... Variable arguments
     */
    void write (const void* buffer, size_t len);

    /**
     * @brief Check if debugger is enabled.
     * @return true if enabled
     * @return false if disabled
     */
    bool isEnabled() const;

private:
    UnbufferedSerial uart;
    bool enabled;
};

/// Global debugger instance
extern Debugger uartUSB;

#endif // DEBUGGER_H
