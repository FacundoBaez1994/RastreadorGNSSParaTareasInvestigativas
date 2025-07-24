//=====[Libraries]=============================================================
#include "IMUManager.h"
#include "Debugger.h"
#include "mbed.h"

//=====[Declaration of private defines]========================================
#define I2C_SDA_DEFAULT PA_10
#define I2C_SCL_DEFAULT PA_9
#define I2C_INT_DEFAULT PA_12
#define I2C_RST_DEFAULT PB_0
#define RAD_TO_DEG 57.29577951308232f // = 180 / pi
#define PI 3.14159265358979323846

#define BNO080_DEFAULT_ADDRESS 0x4A

#define THRESHOLD_TO_BE_ON_MOTION 10
#define THRESHOLD_TO_BE_STATIONARY 1000

#define I2C_FREQUENCY_DEFAULT 400000
#define REFRESH_TIME_DEFAULT 100


#define SAMPLES_PROM 100

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]===================================

/**
 * @brief Contructor method creates a new trackerGPS instance ready to be used
 */
IMUManager::IMUManager() {
    this->bno08x = new Adafruit_BNO08x(I2C_RST_DEFAULT);
    this->i2c = new I2C(I2C_SDA_DEFAULT, I2C_SCL_DEFAULT);
}

IMUManager::~IMUManager() {
  delete this->bno08x;
  this->bno08x = nullptr;
  delete this->i2c;
  this->i2c = nullptr;
}

bool IMUManager::initialize (void) {
    char log[100];

    snprintf(log, sizeof(log), "Adafruit BNO08x test!\n\r");
    uartUSB.write(log, strlen(log));

    // Try to initialize with I2C bus and address
    if (!this->bno08x->begin_I2C(BNO080_DEFAULT_ADDRESS, this->i2c)) {
        snprintf(log, sizeof(log), "Failed to find BNO08x chip\n\r");
        uartUSB.write(log, strlen(log));
        return false;
    }
    snprintf(log, sizeof(log), "BNO08x Found!\n\r");
    uartUSB.write(log, strlen(log));

    this->reportType = SH2_ROTATION_VECTOR;
    this->reportIntervalUs = 4000;
    if (setReports(this->reportType, this->reportIntervalUs) == false) {
        return false;
    }

    /*
    this->reportType = SH2_ACCELEROMETER;
    setReports(this->reportType, this->reportIntervalUs);
    this->reportType = SH2_RAW_ACCELEROMETER;
    setReports(this->reportType, this->reportIntervalUs);
    */
    this->reportType = SH2_LINEAR_ACCELERATION;
    if (setReports(this->reportType, this->reportIntervalUs) == false){
        return false;
    }

    
    this->reportType = SH2_STABILITY_CLASSIFIER;
    if (this->bno08x->enableReport(this->reportType, 100000) == false){
        return false;
    }
    
    

    snprintf(log, sizeof(log), "Reading events\n\r");
    uartUSB.write(log, strlen(log));

    this->clearAcumulatedMeasurement ();
    return true;
}



void IMUManager::checkStability(deviceMotionStatus_t * currentMotionStatus) {
    char log[100];

    // Si el sensor se reseteó, reinicializar los reportes
    if (this->bno08x->wasReset()) {
        snprintf(log, sizeof(log), "Sensor was reset\n\r");
        uartUSB.write(log, strlen(log));
        setReports(this->reportType, this->reportIntervalUs); // O podés usar initialize()
    }

    // Verifica si hay un nuevo evento de sensor
    if (this->bno08x->getSensorEvent(&this->sensorValue)) {

        // Mostrar el sensor ID para debugging
        //snprintf(log, sizeof(log), "Sensor ID: %d\n\r", this->sensorValue.sensorId);
        //uartUSB.write(log, strlen(log));

        // Procesar únicamente si es Stability Classifier
        if (this->sensorValue.sensorId == SH2_STABILITY_CLASSIFIER) {
            sh2_StabilityClassifier_t stability = this->sensorValue.un.stabilityClassifier;

            // Mostrar clasificación de estabilidad
            switch (stability.classification) {
                case STABILITY_CLASSIFIER_UNKNOWN:
                    //uartUSB.write("Stability: Unknown\n\r", strlen("Stability: Unknown\n\r"));
                    
                    break;
                case STABILITY_CLASSIFIER_ON_TABLE:
                    //uartUSB.write("Stability: On Table\n\r", strlen("Stability: On Table\n\r"));
                    if (this->currentMotionStatus == DEVICE_ON_MOTION) {
                        this->stillnessCounter++;
                    }
                    break;
                case STABILITY_CLASSIFIER_STATIONARY:
                    if (this->currentMotionStatus == DEVICE_ON_MOTION) {
                        this->stillnessCounter++;
                    }
                    //uartUSB.write("Stability: Stationary\n\r", strlen("Stability: Stationary\n\r"));
                    break;
                case STABILITY_CLASSIFIER_STABLE:
                    if (this->currentMotionStatus == DEVICE_ON_MOTION) {
                        this->stillnessCounter++;
                    }
                   // uartUSB.write("Stability: Stable\n\r", strlen("Stability: Stable\n\r"));
                    break;
                case STABILITY_CLASSIFIER_MOTION:
                    if (this->currentMotionStatus == DEVICE_STATIONARY) {
                       this->motionCounter++;
                    }  else {
                        this->stillnessCounter = 0;
                    }
                    //uartUSB.write("Stability: In Motion\n\r", strlen("Stability: In Motion\n\r"));
                     break;
                default:
                    //uartUSB.write("Stability: Unrecognized\n\r", strlen("Stability: Unrecognized\n\r"));
                     break;
            }
        } else {
            // No es el sensor de estabilidad, lo ignoramos
            return;
        }
    }
    if (motionCounter >= THRESHOLD_TO_BE_ON_MOTION &&  this->currentMotionStatus == DEVICE_STATIONARY ) {
        this->motionCounter = 0;
        this->stillnessCounter = 0;
        this->currentMotionStatus = DEVICE_ON_MOTION;
        uartUSB.write("\n\rDEVICE_ON_MOTION\n\r", strlen("\n\rDEVICE_ON_MOTION\n\r"));
    }

    if (stillnessCounter >= THRESHOLD_TO_BE_STATIONARY &&  this->currentMotionStatus ==  DEVICE_ON_MOTION ) {
        this->motionCounter = 0;
        this->stillnessCounter = 0;
        this->currentMotionStatus = DEVICE_STATIONARY;
        uartUSB.write("\n\rDEVICE_STATIONARY\n\r", strlen("\n\rDEVICE_STATIONARY\n\r"));
    }
    *currentMotionStatus =  this->currentMotionStatus;


    // No hubo evento
    return;
}



bool IMUManager::obtainInertialMeasures(IMUData_t * inertialMeasures) {
  char log[100];

  if (this->bno08x->wasReset()) {

    snprintf(log, sizeof(log), "sensor was reset \n\r");
    uartUSB.write(log, strlen(log));
    setReports(reportType, reportIntervalUs);
  }

  if (this->bno08x->getSensorEvent(&this->sensorValue)) {
    // in this demo only one report type will be received depending on FAST_MODE
    // define (above)
    switch (this->sensorValue.sensorId) {
        case SH2_ROTATION_VECTOR:
            quaternionToEuler(this->sensorValue.un.rotationVector.real,
                                this->sensorValue.un.rotationVector.i,
                                this->sensorValue.un.rotationVector.j,
                                this->sensorValue.un.rotationVector.k,
                                &this->eulerAngles, true);
            /*
            snprintf(log, sizeof(log),
                    "Status: %d\tYaw: %.2f\tPitch: %.2f\tRoll: %.2f\n\r",
                    this->sensorValue.status, this->eulerAngles.yaw,
                    this->eulerAngles.pitch, this->eulerAngles.roll);
            uartUSB.write(log, strlen(log));
            */
            this->promEulerMeasurement ();
            break;
            /*
            // RAW
        case SH2_RAW_ACCELEROMETER:
            snprintf(log, sizeof(log),
                    "SH2_RAW_ACCELEROMETER: ax: %d\t ay: %d\t az: %d\n\r",
                    this->sensorValue.un.rawAccelerometer.x, this->sensorValue.un.rawAccelerometer.y,
                    this->sensorValue.un.rawAccelerometer.z);
            uartUSB.write(log, strlen(log));
            break;
            // WITH GRAVITY
        case SH2_ACCELEROMETER:
            snprintf(log, sizeof(log),
                    "SH2_ACCELEROMETER: ax: %.2f\t ay: %.2f\t az: %.2f\n\r",
                    this->sensorValue.un.accelerometer.x, this->sensorValue.un.accelerometer.y,
                    this->sensorValue.un.accelerometer.z);
            uartUSB.write(log, strlen(log));
            break;
        */
        // WITHOUT GRAVITY
        case  SH2_LINEAR_ACCELERATION:
            this->acceleration.ax = this->sensorValue.un.linearAcceleration.x;
            this->acceleration.ay = this->sensorValue.un.linearAcceleration.y;
            this->acceleration.az = this->sensorValue.un.linearAcceleration.z;
            this->promAccelMeasurement ();
            /*
                snprintf(log, sizeof(log),
                        "SH2_LINEAR_ACCELERATION: ax: %.2f\t ay: %.2f\t az: %.2f\n\r",
                        this->sensorValue.un.linearAcceleration.x, this->sensorValue.un.linearAcceleration.y,
                        this->sensorValue.un.linearAcceleration.z);
                uartUSB.write(log, strlen(log));
            */
            break;
        }
    }
    if (this->samplesCounterAccel >= SAMPLES_PROM && this->samplesCounterAngles >= SAMPLES_PROM) {
        /*
        snprintf(log, sizeof(log), "\n\r \n\r PROM RESULT:\n\r \n\r");
        uartUSB.write(log, strlen(log));
        snprintf(log, sizeof(log),
            "SH2_LINEAR_ACCELERATION: ax: %.2f\t ay: %.2f\t az: %.2f\n\r",
            this->promAcceleration.ax, this->promAcceleration.ay,
            this->promAcceleration.az);
        uartUSB.write(log, strlen(log));
        snprintf(log, sizeof(log),
            "Status: %d\tYaw: %.2f\tPitch: %.2f\tRoll: %.2f\n\r",
            this->sensorValue.status, this->promEulerAngles.yaw,
            this->promEulerAngles.pitch, this->promEulerAngles.roll);
        uartUSB.write(log, strlen(log));
        */
       
        inertialMeasures->status = sensorValue.status;
        inertialMeasures->acceleration.ax = this->promAcceleration.ax;
        inertialMeasures->acceleration.ay = this->promAcceleration.ay;
        inertialMeasures->acceleration.az = this->promAcceleration.az;
        inertialMeasures->angles.yaw = this->promEulerAngles.yaw;
        inertialMeasures->angles.roll = this->promEulerAngles.roll;
        inertialMeasures->angles.pitch = this->promEulerAngles.pitch;
        this->clearAcumulatedMeasurement ();
        return true;
    }
    return false;
}

//=====[Implementations of private methods]==================================
/*
void IMUManager::handleStabilityEvent(const sh2_StabilityClassifier_t& stability) {
    switch (stability.classification) {
        case STABILITY_CLASSIFIER_UNKNOWN:
            //uartUSB.write("Stability: Unknown\n\r", strlen("Stability: Unknown\n\r"));
            
            break;
        case STABILITY_CLASSIFIER_ON_TABLE:
            //uartUSB.write("Stability: On Table\n\r", strlen("Stability: On Table\n\r"));
            if (this->currentMotionStatus == DEVICE_ON_MOTION) {
                this->stillnessCounter++;
            }
            break;
        case STABILITY_CLASSIFIER_STATIONARY:
            if (this->currentMotionStatus == DEVICE_ON_MOTION) {
                this->stillnessCounter++;
            }
            //uartUSB.write("Stability: Stationary\n\r", strlen("Stability: Stationary\n\r"));
            break;
        case STABILITY_CLASSIFIER_STABLE:
            if (this->currentMotionStatus == DEVICE_ON_MOTION) {
                this->stillnessCounter++;
            }
            // uartUSB.write("Stability: Stable\n\r", strlen("Stability: Stable\n\r"));
            break;
        case STABILITY_CLASSIFIER_MOTION:
            if (this->currentMotionStatus == DEVICE_STATIONARY) {
                this->motionCounter++;
            }
            //uartUSB.write("Stability: In Motion\n\r", strlen("Stability: In Motion\n\r"));
                break;
        default:
            //uartUSB.write("Stability: Unrecognized\n\r", strlen("Stability: Unrecognized\n\r"));
                break;
    }

    if (motionCounter >= THRESHOLD_TO_BE_ON_MOTION &&
        this->currentMotionStatus == DEVICE_STATIONARY) {
        this->motionCounter = 0;
        this->stillnessCounter = 0;
        this->currentMotionStatus = DEVICE_ON_MOTION;
        uartUSB.write("\n\rDEVICE_ON_MOTION\n\r", strlen("\n\rDEVICE_ON_MOTION\n\r"));
    }

    if (stillnessCounter >= THRESHOLD_TO_BE_STATIONARY &&
        this->currentMotionStatus == DEVICE_ON_MOTION) {
        this->motionCounter = 0;
        this->stillnessCounter = 0;
        this->currentMotionStatus = DEVICE_STATIONARY;
        uartUSB.write("\n\rDEVICE_STATIONARY\n\r", strlen("\n\rDEVICE_STATIONARY\n\r"));
    }
}
*/


void IMUManager::promAccelMeasurement() {
    if (this->samplesCounterAccel < SAMPLES_PROM) {
        this->samplesCounterAccel++;

        this->promAcceleration.ax += (this->acceleration.ax
         - this->promAcceleration.ax) / this->samplesCounterAccel;
        this->promAcceleration.ay += (this->acceleration.ay
        - this->promAcceleration.ay) / this->samplesCounterAccel;
        this->promAcceleration.az += (this->acceleration.az
        - this->promAcceleration.az) / this->samplesCounterAccel;
    }
}


void IMUManager::promEulerMeasurement() {
    
    if (this->samplesCounterAngles < SAMPLES_PROM) {
        this->samplesCounterAngles++;

        // Promediar ángulos correctamente usando seno y coseno
        static float rollSinAccum = 0, rollCosAccum = 0;
        static float pitchSinAccum = 0, pitchCosAccum = 0;
        static float yawSinAccum = 0, yawCosAccum = 0;

        float rollSin = sin(this->eulerAngles.roll * PI / 180.0);
        float rollCos = cos(this->eulerAngles.roll  * PI / 180.0);
        float pitchSin = sin(this->eulerAngles.pitch * PI / 180.0);
        float pitchCos = cos(this->eulerAngles.pitch * PI / 180.0);
        float yawSin = sin(this->eulerAngles.yaw * PI / 180.0);
        float yawCos = cos(this->eulerAngles.yaw * PI / 180.0);

        rollSinAccum  += (rollSin  - rollSinAccum)  /  this->samplesCounterAngles;
        rollCosAccum  += (rollCos  - rollCosAccum)  / this->samplesCounterAngles;
        pitchSinAccum += (pitchSin - pitchSinAccum) / this->samplesCounterAngles;
        pitchCosAccum += (pitchCos - pitchCosAccum) / this->samplesCounterAngles;
        yawSinAccum   += (yawSin   - yawSinAccum)   / this->samplesCounterAngles;
        yawCosAccum   += (yawCos   - yawCosAccum)   / this->samplesCounterAngles;

        this->promEulerAngles.roll = atan2(rollSinAccum, rollCosAccum) * 180.0 / PI;
        this->promEulerAngles.pitch = atan2(pitchSinAccum, pitchCosAccum) * 180.0 / PI;
        this->promEulerAngles.yaw = atan2(yawSinAccum, yawCosAccum) * 180.0 / PI;
        if (this->samplesCounterAngles == SAMPLES_PROM) {
            // reset internal aculation
            rollSinAccum = 0, rollCosAccum = 0;
            pitchSinAccum = 0, pitchCosAccum = 0;
            yawSinAccum = 0, yawCosAccum = 0;
        }
    } 
}

void IMUManager::clearAcumulatedMeasurement() {
    this->promEulerAngles.yaw = 0;
    this->promEulerAngles.roll = 0;
    this->promEulerAngles.pitch = 0;
    this->promAcceleration.ax = 0;
    this->promAcceleration.ay = 0;
    this->promAcceleration.az = 0;
    this->samplesCounterAccel = 0;
    this->samplesCounterAngles = 0;
  
}



bool IMUManager::setReports(sh2_SensorId_t reportType, long report_interval) {
    char log[100];
    snprintf(log, sizeof(log), "Setting desired reports\n\r");
    uartUSB.write(log, strlen(log));
    if (!this->bno08x->enableReport(reportType, report_interval)) {
        snprintf(log, sizeof(log), "Could not enable stabilized remote vector\n\r");
        uartUSB.write(log, strlen(log));
        return false;
    }
    return true;
}

inline float IMUManager::sq(float x) { return x * x; }

void IMUManager::quaternionToEuler(float qr, float qi, float qj, float qk,
                                   euler_t *ypr, bool degrees = false) {

  float sqr = sq(qr);
  float sqi = sq(qi);
  float sqj = sq(qj);
  float sqk = sq(qk);

  ypr->yaw = atan2(2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr));
  ypr->pitch = asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr));
  ypr->roll = atan2(2.0 * (qj * qk + qi * qr), (-sqi - sqj + sqk + sqr));

  if (degrees) {
    ypr->yaw *= RAD_TO_DEG;
    ypr->pitch *= RAD_TO_DEG;
    ypr->roll *= RAD_TO_DEG;
  }
}

void IMUManager::quaternionToEulerRV(
    sh2_RotationVectorWAcc_t *rotational_vector, euler_t *ypr,
    bool degrees = false) {
  this->quaternionToEuler(rotational_vector->real, rotational_vector->i,
                          rotational_vector->j, rotational_vector->k, ypr,
                          degrees);
}

void IMUManager::quaternionToEulerGI(sh2_GyroIntegratedRV_t *rotational_vector,
                                     euler_t *ypr, bool degrees = false) {
  this->quaternionToEuler(rotational_vector->real, rotational_vector->i,
                          rotational_vector->j, rotational_vector->k, ypr,
                          degrees);
}