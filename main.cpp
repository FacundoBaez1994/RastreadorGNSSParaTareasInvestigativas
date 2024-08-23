#include "mbed.h"
#include "arm_book_lib.h"
#include "string.h"
#include "tracker.h"
#include "Debugger.h"

//=====[Main function, the program entry point after power on or reset]========

int main() {
    tracker tracker;
    while (true) {
        tracker.update();
    }
}



