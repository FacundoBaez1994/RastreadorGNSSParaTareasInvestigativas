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
#include "IdleState.h"
#include "PowerOFFState.h"
#include "PowerState.h"

//=====[Declaration of public data types]======================================
class ConnectionState; //debido a declaracion adelantada

 struct TcpSocket {
    char * IpDirection; // IPV4
    int TcpPort;        // 
 };

  struct CellInformation {
    float signalLevel;
    long long int IMEI;
    int accessTechnology;
    int registrationStatus;
    int channel;
    char * currentOperator;
    char *  dateTimeAndTimeZoneString;
    char *  band;
    // Cell identifiers codes ////
    int lac;
    unsigned long cellId; 
    char *  mcc;
    char * mnc;
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
    void connectToMobileNetwork ();
    bool sendMessage (char * message, TcpSocket * socketTargetted);
    void changeConnectionState  (ConnectionState * newConnectionState);
    void enableTransmission ();
    void changeTransmissionState  (TransmissionState * newTransmissionState);
    void switchSIMCARD ();
    void reboot ();
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