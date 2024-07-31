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

//=====[Declaration of public data types]======================================
class ConnectionState; //debido a declaracion adelantada

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
    bool sendMessage (char * message, char * ipDirection, int tcpPort );
    void changeConnectionState  (ConnectionState * newConnectionState);
    void enableTransmission ();
    void changeTransmissionState  (TransmissionState * newTransmissionState);
    void switchSIMCARD ();
    void reboot ();
    PowerManager* getPowerManager ();
    BufferedSerial* getUART ();
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