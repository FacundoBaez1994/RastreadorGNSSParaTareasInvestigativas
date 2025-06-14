//=====[#include guards - begin]===============================================

#ifndef _LOADING_MESSAGE_H_
#define _LOADING_MESSAGE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "TrackerBaseState.h"
#include "Tracker.h"
#include "string.h"

//=====[Declaration of public data types]======================================
class Tracker; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class LoadingMessage : public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    LoadingMessage (Tracker * tracker);
    virtual ~LoadingMessage ();
    virtual void loadMessage (EEPROMManager * memory, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    IMUData_t * imuData,  BatteryData  * batteryStatuss);
private:
//=====[Declaration of privates atributes]=========================================
    Tracker * tracker;
    //char * poppedString = nullptr;
    //size_t sizeOfPoppedString = 2048;
//=====[Declaration of privates methods]=========================================
    trackerStatus_t parseDecryptedMessage(const char* decryptedString, 
        CellInformation* aCellInfo,
        GNSSData* GNSSInfo,
        std::vector<CellInformation*>& neighborsCellInformation,
        IMUData_t* imuData,
        BatteryData* batteryStatus);
    void parseMNMN(const char* message,
        CellInformation* aCellInfo,
        std::vector<CellInformation*>& neighborsCellInformation,
        IMUData_t* imuData,
        BatteryData* batteryStatus);
    void parseMNGNSS(const char* message,
        CellInformation* aCellInfo,
        GNSSData* GNSSInfo,
        IMUData_t* imuData,
        BatteryData* batteryStatus);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _LOADING_MESSAGE_H_