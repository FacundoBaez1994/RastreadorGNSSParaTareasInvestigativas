//=====[#include guards - begin]===============================================

#ifndef _CELLULAR_MODULE_H_
#define _CELLULAR_MODULE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "IdleState.h"

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
    void changeConnectionState  (ConnectionState * newConnectionState);
    void switchSIMCARD ();
    BufferedSerial* getUART ();
private:
//=====[Declaration of privates atributes]=========================================
    NonBlockingDelay* powerChangeDurationtimer;
    NonBlockingDelay* refreshTime;
    ATCommandHandler* ATHandler;
    ConnectionState* currentConnectionState;

    bool turningPowerManual;
    bool turningPowerAutomatic;
    bool wasManualyTurnOff;
    DigitalIn* powerStatusInput; // negative value if power on
    DigitalIn* powerControlButtonInput;
    DigitalOut* powerKeyOutput; // soft power control
    DigitalOut* powerDownOutput; // Power source enable - hard power control
    DigitalOut* simCardSwitchOutput; // Switch between Simcards
    bool watingForResponse;


//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _CELLULAR_MODULE_H_