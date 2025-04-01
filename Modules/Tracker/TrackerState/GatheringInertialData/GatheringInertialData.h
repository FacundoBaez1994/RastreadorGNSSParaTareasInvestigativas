//=====[#include guards - begin]===============================================

#ifndef _GATHERING_INERTIAL_DATA_H_
#define _GATHERING_INERTIAL_DATA_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "TrackerState.h"
#include "Tracker.h"
#include "Non_Blocking_Delay.h"

//=====[Declaration of public data types]======================================
class Tracker; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class GatheringInertialData: public TrackerState {
public:
//=====[Declaration of public methods]=========================================
    GatheringInertialData (Tracker * tracker, trackerStatus_t trackerStatus);
    virtual ~GatheringInertialData ();
    virtual void calibrateIMU (IMU * inertialSensor);
    virtual void obtainInertialMeasures (IMU * inertialSensor, char * dataObtain, float * temperatureObtain);
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);
    virtual void obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata);
    virtual void connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation);
    virtual void obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors );
    virtual void formatMessage (char * formattedMessage, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    BatteryData  * batteryStatus); 
    virtual void exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage );
    // agregar LoRa // exchageMessages (Lora * LoRaModule);
    virtual void goToSleep (CellularModule * cellularTransceiver);
    virtual void awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency);
private:
    Tracker * tracker;
    NonBlockingDelay * timeBeetwenSamples;
    trackerStatus_t currentStatus;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _GATHERING_INERTIAL_DATA_H_