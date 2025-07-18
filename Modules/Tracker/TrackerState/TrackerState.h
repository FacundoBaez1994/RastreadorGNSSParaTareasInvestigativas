//=====[#include guards - begin]===============================================

#ifndef _TRACKER_STATE_H_
#define _TRACKER_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "CellularModule.h"
#include "GNSSModule.h"
#include "IMUManager.h"


//=====[Declaration of public data types]======================================
struct TcpSocket;
struct BatteryData;

//=====[Declaration of public classes]=========================================
/*
 *  Interface - State desing pattern
 * 
 */
class TrackerState {
public:
//=====[Declaration of public methods]=========================================
    virtual void calibrateIMU (IMUManager * inertialSensor);
    virtual void obtainInertialMeasures (IMUManager * inertialSensor, IMUData_t * inertialData);
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);
    virtual void obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata);
    virtual void connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation);
    virtual void obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors );
    virtual void formatMessage (char * formattedMessage, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
     IMUData_t * imuData, BatteryData  * batteryStatus); 
    // agregar LoRa // exchageMessages (Lora * LoRaModule);
    virtual void exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage );
    virtual void goToSleep (CellularModule * cellularTransceiver);
    virtual void awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency);
    

//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _TRACKER_STATE_H_