#include "Debugger.h"
#include <cstddef>

Debugger::Debugger(PinName tx, PinName rx, int baud)
    : uart(tx, rx, baud), enabled(false) {}

void Debugger::enable() {
    enabled = true;
}

void Debugger::disable() {
    enabled = false;
}

bool Debugger::isEnabled() const {
    return enabled;
}

void Debugger::write (const void* buffer, size_t len) {
    if (this->enabled == false) return;

    uart.write(buffer, len);

}

/// Create global instance (maps to USBTX/USBRX)
Debugger uartUSB(USBTX, USBRX, 115200);
