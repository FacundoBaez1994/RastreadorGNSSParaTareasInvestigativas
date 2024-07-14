//=====[#include guards - begin]===============================================

#ifndef _CELLULAR_MODULE_H_
#define _CELLULAR_MODULE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "non_Blocking_Delay.h"
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
class cellularModule {
public:

//=====[Declaration of public methods]=========================================
    cellularModule ( );
    virtual ~cellularModule ();
    void startStopUpdate ();
    void connectToMobileNetwork ();
    BufferedSerial* getUART ();
private:
//=====[Declaration of privates atributes]=========================================
    nonBlockingDelay* powerChangeDurationtimer;
    nonBlockingDelay* refreshTime;
    ATCommandHandler* ATHandler;
    ConnectionState* currentConnectionState;

    bool turningPower;
    DigitalIn* powerStatusInput; // negative value if power on
    DigitalIn* powerControlButtonInput;
    DigitalOut* powerKeyOutput; // soft power control
    DigitalOut* powerDownOutput; // Power source enable - hard power control
    bool watingForResponse;


//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _CELLULAR_MODULE_H_