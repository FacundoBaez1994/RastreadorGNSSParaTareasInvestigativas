//=====[#include guards - begin]===============================================

#ifndef _LOADING_MESSAGE_H_
#define _LOADING_MESSAGE_H_

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
class LoadingMessage : public TrackerBaseState {
public:
//=====[Declaration of public methods]=========================================
    LoadingMessage (Tracker * tracker);
    virtual ~LoadingMessage ();
    virtual void loadMessage (EEPROMManager * memory, char * message);
private:
    Tracker * tracker;

//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _LOADING_MESSAGE_H_