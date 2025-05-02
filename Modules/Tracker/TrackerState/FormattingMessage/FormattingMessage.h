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

    virtual void formatMessage (char * formattedMessage, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
     IMUData_t * imuData, BatteryData  * batteryStatus); 

private:
    void formatMessage(char * formattedMessage, CellInformation* aCellInfo,
     GNSSData* GNSSInfo, IMUData_t * imuData, BatteryData  * batteryStatus);
    void formatMessage(char * formattedMessage, CellInformation* aCellInfo, 
    std::vector<CellInformation*> &neighborsCellInformation, IMUData_t * imuData, BatteryData  * batteryStatus); 

    //bool checkResponse (char * response, char * retrivMessage);
    Tracker * tracker;
    trackerStatus_t currentStatus;


//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _FORMATTING_MESSAGE_H_