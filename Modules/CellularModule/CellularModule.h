//=====[#include guards - begin]===============================================

#ifndef _CELLULAR_MODULE_H_
#define _CELLULAR_MODULE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "TransmissionState.h"
#include "ActivatePDP.h"
#include "PowerManager.h"
#include "TransmissionUnavailable.h"
#include "ConnectionUnavailableState.h"
#include "PowerOFFState.h"
#include "PowerState.h"

//=====[Declaration of public data types]======================================
class ConnectionState; //debido a declaracion adelantada
struct BatteryData;

 struct TcpSocket {
    char * IpDirection; // IPV4
    int TcpPort;        // 
 };

  struct CellInformation {
    float signalLevel;
    long long int IMEI;
    int accessTechnology; //
    int registrationStatus; // pass as int in order to decrese data lenght (translate in server)
    int channel;
    char *  date;
    char * time;
    char * band;
    // Cell identifiers codes ////
    char * lac;
    char * cellId; 
    int  mcc; // with mnc identifies operator
    int mnc; // 
    //////////////////////////////
 };


//=====[Declaration of public classes]=========================================
/*
 *  
 * 
 */
class CellularModule {
public:

//=====[Declaration of public methods]=========================================
    CellularModule ( );
    virtual ~CellularModule ();
    void startStopUpdate ();
    void reboot ();
    void awake ();
    bool goToSleep ();
    bool measureBattery (BatteryData * currentBatteryData);
    CellularConnectionStatus_t connectToMobileNetwork (CellInformation * currentCellInformation);
    CellularTransmissionStatus_t sendMessage (char * message, TcpSocket * socketTargetted);
    void changeConnectionState  (ConnectionState * newConnectionState);
    void enableTransmission ();
    void enableConnection ();
    void changeTransmissionState  (TransmissionState * newTransmissionState);
    void switchSIMCARD ();

    PowerManager* getPowerManager ();
    BufferedSerial* getUART ();
    ATCommandHandler* getATHandler ();
private:
//=====[Declaration of privates atributes]=========================================
    NonBlockingDelay* refreshTime;
    ATCommandHandler* ATHandler;
    ConnectionState* currentConnectionState;
    TransmissionState* currentTransmissionState;
    PowerManager * modulePowerManager;

    DigitalOut* simCardSwitchOutput; // Switch between Simcards
    bool watingForResponse;
    powerStatus_t currentPowerStatus;


//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _CELLULAR_MODULE_H_