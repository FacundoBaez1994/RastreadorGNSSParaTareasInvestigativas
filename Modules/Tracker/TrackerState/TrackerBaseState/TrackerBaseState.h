//=====[#include guards - begin]===============================================

#ifndef _TRACKER_BASE_STATE_H_
#define _TRACKER_BASE_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "TrackerState.h"
#include "Tracker.h"

//=====[Declaration of public data types]======================================
class Tracker; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 * Abtract class - State desing pattern
 * 
 */
class TrackerBaseState : public TrackerState {
public:
//=====[Declaration of public methods]=========================================
    virtual void calibrateIMU (IMUManager * inertialSensor);
    virtual void obtainInertialMeasures (IMUManager * inertialSensor, IMUData_t * inertialData, 
    std::vector<IMUData_t*> &IMUDataSamples);
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);
    virtual void obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata);
    virtual void connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation);
    virtual void obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors );
    virtual void formatMessage (char * formattedMessage, const CellInformation* aCellInfo,
    const GNSSData* GNSSInfo, const std::vector<CellInformation*> &neighborsCellInformation,
    const IMUData_t * imuData, const std::vector<IMUData_t*> &IMUDataSample, const BatteryData  * batteryStatus); 
    virtual void exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage );
    virtual void goToSleep (CellularModule * cellularTransceiver);
    virtual void awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency, NonBlockingDelay * silentTimer);
    virtual void checkStabillity (IMUManager * inertialSensor, deviceMotionStatus_t * newMotionStatus);
    virtual void saveMessage (EEPROMManager * memory, char * message);
    virtual void loadMessage (EEPROMManager * memory, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    IMUData_t * imuData, std::vector<IMUData_t*> &IMUDataSamples, BatteryData  * batteryStatus);
private:
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _TRACKER_BASE_STATE_H_