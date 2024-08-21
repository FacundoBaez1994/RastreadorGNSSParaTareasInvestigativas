#include "mbed.h"
#include "arm_book_lib.h"
#include "string.h"
#include "tracker.h"
#include "MPU9250.h"
#include "Debugger.h"

//=====[Main function, the program entry point after power on or reset]========

int main() {
    //Set up I2C
    MPU9250 mpu9250;
    i2c.frequency(400000);  // use fast (400 kHz) I2C  
    char buffer[80]; // Increase buffer size to accommodate longer messages
    
    while (true) {
        

          wait_us(5000000);
        // Read the WHO_AM_I register, this is a good test of communication
        uint8_t whoami = mpu9250.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);  // Read WHO_AM_I register for MPU-9250
        sprintf(buffer, "I AM 0x%x\n\r", whoami);  // Format message with whoami value
        uartUSB.write(buffer, strlen(buffer));     // Send formatted message over UART

        if (whoami == 0x71) // WHO_AM_I should always be 0x68
        {  
            sprintf(buffer, "MPU9250 is online...\n\r");
            uartUSB.write(buffer, strlen(buffer));
            wait_us(1000000);
            
            mpu9250.resetMPU9250(); // Reset registers to default in preparation for device calibration
            mpu9250.calibrateMPU9250(gyroBias, accelBias); // Calibrate gyro and accelerometers, load biases in bias registers  

            sprintf(buffer, "x gyro bias = %f\n\r", gyroBias[0]);
            uartUSB.write(buffer, strlen(buffer));
            sprintf(buffer, "y gyro bias = %f\n\r", gyroBias[1]);
            uartUSB.write(buffer, strlen(buffer));
            sprintf(buffer, "z gyro bias = %f\n\r", gyroBias[2]);
            uartUSB.write(buffer, strlen(buffer));
            sprintf(buffer, "x accel bias = %f\n\r", accelBias[0]);
            uartUSB.write(buffer, strlen(buffer));
            sprintf(buffer, "y accel bias = %f\n\r", accelBias[1]);
            uartUSB.write(buffer, strlen(buffer));
            sprintf(buffer, "z accel bias = %f\n\r", accelBias[2]);
            uartUSB.write(buffer, strlen(buffer));
            wait_us(2000000);
            
            mpu9250.initMPU9250(); 
            sprintf(buffer, "MPU9250 initialized for active data mode....\n\r"); // Initialize device for active mode read of accelerometer, gyroscope, and temperature
            uartUSB.write(buffer, strlen(buffer));
            mpu9250.initAK8963(magCalibration);
            sprintf(buffer, "AK8963 initialized for active data mode....\n\r"); // Initialize device for active mode read of magnetometer
            uartUSB.write(buffer, strlen(buffer));
            sprintf(buffer, "Accelerometer full-scale range = %f  g\n\r", 2.0f*(float)(1<<Ascale));
            uartUSB.write(buffer, strlen(buffer));
            sprintf(buffer, "Gyroscope full-scale range = %f  deg/s\n\r", 250.0f*(float)(1<<Gscale));
            uartUSB.write(buffer, strlen(buffer));
            if(Mscale == 0) uartUSB.write("Magnetometer resolution = 14  bits\n\r", 36);
            if(Mscale == 1) uartUSB.write("Magnetometer resolution = 16  bits\n\r", 36);
            if(Mmode == 2) uartUSB.write("Magnetometer ODR = 8 Hz\n\r", 25);
            if(Mmode == 6) uartUSB.write("Magnetometer ODR = 100 Hz\n\r", 28);
            wait_us(2000000);
        }
        else {
            sprintf(buffer, "Could not connect to MPU9250: \n\r");
            uartUSB.write(buffer, strlen(buffer));
            sprintf(buffer, "%#x \n", whoami);
            uartUSB.write(buffer, strlen(buffer));
        
            while(1) ; // Loop forever if communication doesn't happen
        }
    }
}



