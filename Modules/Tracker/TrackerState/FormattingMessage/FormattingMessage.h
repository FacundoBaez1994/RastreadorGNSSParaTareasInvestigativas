//=====[#include guards - begin]===============================================

#ifndef _FORMATTING_MESSAGE_H_
#define _FORMATTING_MESSAGE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "TrackerBaseState.h"
#include "Tracker.h"

//=====[Declaration of public data types]======================================
class Tracker; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class FormattingMessage : public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    FormattingMessage (Tracker * tracker, trackerStatus_t trackerStatus);
    virtual ~FormattingMessage ();
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);

    virtual void formatMessage (char * formattedMessage, const CellInformation* aCellInfo,
    const GNSSData* GNSSInfo, const std::vector<CellInformation*> &neighborsCellInformation,
    const IMUData_t * imuData, const BatteryData  * batteryStatus); 

private:
    void formatMessage(char * formattedMessage, const CellInformation* aCellInfo,
    const GNSSData* GNSSInfo, const IMUData_t * imuData, const BatteryData  * batteryStatus);

    void formatMessage(char * formattedMessage, const CellInformation* aCellInfo, 
    const std::vector<CellInformation*> &neighborsCellInformation, const IMUData_t * imuData, const BatteryData  * batteryStatus);

    void formatLoRaMessage(char * formattedMessage, const CellInformation* aCellInfo,
    const GNSSData* GNSSInfo, const IMUData_t * imuData, const BatteryData  * batteryStatus);

    void formatLoRaMessage(char * formattedMessage, const CellInformation* aCellInfo, 
    const IMUData_t * imuData, const BatteryData  * batteryStatus);

    void formatMNMNMemoryMessage (char * formattedMessage, const CellInformation* aCellInfo, const std::vector<CellInformation*> &neighborsCellInformation, 
    const IMUData_t * imuData, const BatteryData  * batteryStatus);

    void formatMemoryMessage(char * formattedMessage, const CellInformation* aCellInfo, const GNSSData* GNSSInfo, 
    const IMUData_t * inertialData, const BatteryData  * batteryStatus);

    //bool checkResponse (char * response, char * retrivMessage);
    Tracker * tracker;
    trackerStatus_t currentStatus;


//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _FORMATTING_MESSAGE_H_