#include "mbed.h"
#include <ctime>

time_t timestampToEpoch(const char* timestamp);

void epochToTimestamp(time_t seconds, char* timestampOut, size_t len);