//=====[#include guards - begin]===============================================

#ifndef _IMU_H_
#define _IMU_H_

#include "Debugger.h" // due to global usbUart
#include "IMUState.h"
#include "Non_Blocking_Delay.h"
#include "I2CInterfaceHandler.h"
#include "MPU9250RegisterMap.h"
#include "initializeMPU9250.h"

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================
#define DEG_TO_RAD (3.14159265358979323846 / 180.0)
#define CALIB_GYRO_SENSITIVITY 131     // LSB/degrees/sec
#define CALIB_ACCEL_SENSITIVITY 16384  // LSB/g 

enum class ACCEL_FS_SEL {
    A2G,
    A4G,
    A8G,
    A16G
};
enum class GYRO_FS_SEL {
    G250DPS,
    G500DPS,
    G1000DPS,
    G2000DPS
};
enum class MAG_OUTPUT_BITS {
    M14BITS,
    M16BITS
};

enum class FIFO_SAMPLE_RATE : uint8_t {
    SMPL_1000HZ,
    SMPL_500HZ,
    SMPL_333HZ,
    SMPL_250HZ,
    SMPL_200HZ,
    SMPL_167HZ,
    SMPL_143HZ,
    SMPL_125HZ,
};

enum class GYRO_DLPF_CFG : uint8_t {
    DLPF_250HZ,
    DLPF_184HZ,
    DLPF_92HZ,
    DLPF_41HZ,
    DLPF_20HZ,
    DLPF_10HZ,
    DLPF_5HZ,
    DLPF_3600HZ,
};

enum class ACCEL_DLPF_CFG : uint8_t {
    DLPF_218HZ_0,
    DLPF_218HZ_1,
    DLPF_99HZ,
    DLPF_45HZ,
    DLPF_21HZ,
    DLPF_10HZ,
    DLPF_5HZ,
    DLPF_420HZ,
};

static constexpr uint8_t MPU9250_WHOAMI_DEFAULT_VALUE {0x71};
static constexpr uint8_t MPU9255_WHOAMI_DEFAULT_VALUE {0x73};
static constexpr uint8_t MPU6500_WHOAMI_DEFAULT_VALUE {0x70};
 static constexpr uint8_t MAG_MODE {0x06};  // 0x02 for 8 Hz, 0x06 for 100 Hz continuous magnetometer data read

//   static constexpr uint8_t MPU9250_DEFAULT_ADDRESS {0x68};  // Device address when ADO = 0
static constexpr uint8_t AK8963_ADDRESS {0x0C};           //  Address of magnetometer
static constexpr uint8_t AK8963_WHOAMI_DEFAULT_VALUE {0x48};
//    uint8_t mpu_i2c_addr {MPU9250_DEFAULT_ADDRESS};

struct MPU9250Setting {
    ACCEL_FS_SEL accel_fs_sel {ACCEL_FS_SEL::A16G};
    GYRO_FS_SEL gyro_fs_sel {GYRO_FS_SEL::G2000DPS};
    MAG_OUTPUT_BITS mag_output_bits {MAG_OUTPUT_BITS::M16BITS};
    FIFO_SAMPLE_RATE fifo_sample_rate {FIFO_SAMPLE_RATE::SMPL_200HZ};
    uint8_t gyro_fchoice {0x03};
    GYRO_DLPF_CFG gyro_dlpf_cfg {GYRO_DLPF_CFG::DLPF_41HZ};
    uint8_t accel_fchoice {0x01};
    ACCEL_DLPF_CFG accel_dlpf_cfg {ACCEL_DLPF_CFG::DLPF_45HZ};
};

//=====[Declaration of public classes]=========================
/*
 * Class implementation for a GPS tracker
 * High hierarchy class
 * it will be instantiated and used from the main function
 */
class IMU {
public:
    IMU ();
    virtual ~IMU ();
    void obtainInertialMeasures();
    void changeState  (IMUState * newIMUState);

    uint8_t getMPUI2CAddress ();

    //Settings
    MPU9250Setting getMPUSettings ();

    float get_acc_resolution(const ACCEL_FS_SEL accel_af_sel) const;
    float get_gyro_resolution(const GYRO_FS_SEL gyro_fs_sel) const;
    float get_mag_resolution(const MAG_OUTPUT_BITS mag_output_bits) const;
    ///

    void get_mag_bias_factory (float &xMagBias, float &yMagBias, float &zMagBias);
    void set_mag_bias_factory (float xMagBias, float yMagBias, float zMagBias);

    void get_accel_bias (float &xa_bias, float &ya_bias, float &za_bias);
    void set_accel_bias (float xa_bias, float ya_bias, float za_bias);
    
    void get_gyro_bias (float &xg_bias, float &yg_bias, float &zg_bias);
    void set_gyro_bias (float xg_bias, float yg_bias, float zg_bias);

    void get_mag_bias (float &xmag_bias, float &ymag_bias, float &zmag_bias);
    void set_mag_bias (float xmag_bias, float ymag_bias, float zmag_bias);

    void get_mag_scale (float &xmag_scale, float &ymag_scale, float &zmag_scale);
    void set_mag_scale (float xmag_scale, float ymag_scale, float zmag_scale);
    
private:
    I2CInterfaceHandler * interface;
    IMUState * currentState;
    NonBlockingDelay * refreshTime;

    MPU9250Setting setting;
    uint8_t mpuI2CAddress;

    float mag_bias_factory[3] {0., 0., 0.};
    float a_bias[3] {0., 0., 0.};   // acc calibration value in ACCEL_FS_SEL: 2g
    float g_bias[3] {0., 0., 0.};  // gyro calibration value in GYRO_FS_SEL: 250dps
    float mag_bias[3] {0., 0., 0.};  // mag calibration value in MAG_OUTPUT_BITS: 16BITS
    float mag_scale[3] {1., 1., 1.};

};


//=====[#include guards - end]=================================================

#endif // _TRACKER_H_
