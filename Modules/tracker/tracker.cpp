//=====[Libraries]=============================================================
#include "mbed.h"
#include "tracker.h"
#include "QuaternionFilter.h"
#include "Debugger.h"
#include "MPU9250RegisterMap.h"

//=====[Declaration of private defines]========================================
//using namespace std::chrono;
#define I2C_SDA PA_10
#define I2C_SCL PA_9 
#define CALIB_GYRO_SENSITIVITY 131     // LSB/degrees/sec
#define CALIB_ACCEL_SENSITIVITY 16384  // LSB/g 


//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================
    
//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]===================================

/*
void tracker::print_roll_pitch_yaw() {
    char buffer[128];
    int len = snprintf(buffer, sizeof(buffer),
                       "Yaw, Pitch, Roll: %.2f, %.2f, %.2f\n\r",
                       this->imu->getYaw(), this->imu->getPitch(), this->imu->getRoll());
    uartUSB.write(buffer, len);
}

void tracker::print_acceleration () {
    char buffer[128];
    int len = snprintf(buffer, sizeof(buffer),
                       "AccX, AccY, AccZ: %.2f, %.2f, %.2f\n\r",
                       this->imu->getAccX (), this->imu->getAccY (), this->imu->getAccZ());
    uartUSB.write(buffer, len);
}

void tracker::print_Gyro () {
    char buffer[128];
    int len = snprintf(buffer, sizeof(buffer),
                       "GyroX, GyroY, GyroZ: %.2f, %.2f, %.2f\n\r",
                       this->imu->getGyroX (), this->imu->getGyroY (), this->imu->getGyroZ());
    uartUSB.write(buffer, len);
}

void tracker::print_calibration() {
    char buffer[256];
    int len = snprintf(buffer, sizeof(buffer),
                       "< calibration parameters >\n\r"
                       "accel bias [g]: %.3f, %.3f, %.3f\n\r"
                       "gyro bias [deg/s]: %.3f, %.3f, %.3f\n\r"
                       "mag bias [mG]: %.3f, %.3f, %.3f\n\r"
                       "mag scale []: %.3f, %.3f, %.3f\n\r",
                       this->imu->getAccBiasX() * 1000.f / (float) CALIB_ACCEL_SENSITIVITY,
                       this->imu->getAccBiasY() * 1000.f / (float) CALIB_ACCEL_SENSITIVITY,
                       this->imu->getAccBiasZ() * 1000.f / (float) CALIB_ACCEL_SENSITIVITY,
                       this->imu->getGyroBiasX() / (float) CALIB_GYRO_SENSITIVITY,
                       this->imu->getGyroBiasY() / (float) CALIB_GYRO_SENSITIVITY,
                       this->imu->getGyroBiasZ() / (float) CALIB_GYRO_SENSITIVITY,
                       this->imu->getMagBiasX(), this->imu->getMagBiasY(), this->imu->getMagBiasZ(),
                       this->imu->getMagScaleX(), this->imu->getMagScaleY(), this->imu->getMagScaleZ());
    uartUSB.write(buffer, len);
}
*/

/** 
* @brief Contructor method creates a new trackerGPS instance ready to be used
*/
tracker::tracker () {
   // I2C * interface = new I2C (I2C_SDA, I2C_SCL);
   // interface->frequency (400000);
   // this->imu = new MPU9250 (interface);
   this->imu = new IMU ();
}

tracker::~tracker() {

}


/** 
* @brief Main rutine of the tracker device
*   
*
*/
void tracker::update () {

    this->imu->obtainInertialMeasures();

    /*
    static bool initialize =  false; 
    I2C * interface;
    static Timer timer; // Temporizador estático
    //auto prev_time = Kernel::Clock::now(); // Obtiene el tiempo actual


    if (initialize == false) {
        wait_us (2000);
        if (!this->imu->setup(0x68)) {  // change to your own address
                uartUSB.write("MPU connection failed\n\r", strlen ("MPU connection failed\n\r")); 
                wait_us (2000);
                //delay(5000); ARDUINO
                return;
        }
        // calibrate anytime you want to
        uartUSB.write("Accel Gyro calibration will start in 5sec\n\r", 
        strlen ("Accel Gyro calibration will start in 5sec\n\r")); 
        uartUSB.write("Please leave the device still on the flat plane\n\r", 
        strlen ("Please leave the device still on the flat plane\n\r")); 
        this->imu->verbose(true);
        wait_us(5000000);
        //delay(5000);
        this->imu->calibrateAccelGyro();

        uartUSB.write("Mag calibration will start in 5sec\n\r", 
        strlen ("Mag calibration will start in 5sec\n\r")); 
        uartUSB.write("Please Wave device in a figure eight until done\n\r", 
        strlen ("Please Wave device in a figure eight until done\n\r")); 

        //delay(5000);
        wait_us(5000000);
        this->imu->calibrateMag();

        print_calibration();
        this->imu->verbose(false);

        initialize =  true; 
        timer.start(); // Inicia el temporizador después de la calibración
    }
    
    

    if (this->imu->update()) {
        if (timer.elapsed_time() >= 25ms) {   // Comprueba si han pasado xms
           print_roll_pitch_yaw();
           // print_acceleration ();
          //  print_Gyro();
        }
    }

    */
}

//=====[Implementations of private methods]==================================

