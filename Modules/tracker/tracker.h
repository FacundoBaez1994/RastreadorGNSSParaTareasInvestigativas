//=====[#include guards - begin]===============================================

#ifndef _TRACKER_H_
#define _TRACKER_H_

#include "mbed.h"
#include "Non_Blocking_Delay.h"
//#include "arm_book_lib.h"
#include "string.h"
#include "IMU.h"

//=====[Declaration of public defines]=========================================


//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================
/*
 * Class implementation for a GPS tracker
 * High hierarchy class
 * it will be instantiated and used from the main function
 */
class tracker {
public:
    tracker ();
    virtual ~tracker ();
    void update();
private:
    IMU * imu;
    //void print_roll_pitch_yaw ();
    //void print_acceleration ();
    //void print_Gyro ();
    //void print_calibration ();
    

   // NonBlockingDelay * latency;
   // InertialSensor * sensor;
   // Timer t;
};


//=====[#include guards - end]=================================================

#endif // _TRACKER_H_
