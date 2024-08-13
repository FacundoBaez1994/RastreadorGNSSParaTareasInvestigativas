//=====[#include guards - begin]===============================================

#ifndef _MANUAL_POWER_OFF_STATE_H_
#define _MANUAL_POWER_OFF_STATE_H_

//==================[Libraries]===============================================

#include "PowerManager.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "PowerState.h"
#include "PowerONState.h"
#include "Non_Blocking_Delay.h"
#include "PowerManager.h"

//=====[Declaration of public data types]======================================
class PowerManager; //debido a declaracion adelantada
struct BatteryData;
//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ManualPowerOFFState : public PowerState {
public:
//=====[Declaration of public methods]=========================================
    ManualPowerOFFState ();
    ManualPowerOFFState (PowerManager * newManager);
    virtual ~ManualPowerOFFState ();
    powerStatus_t startStopUpdate (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);
    void reboot (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);
    void goToSleep (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);
    void awake (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);
    bool measureBattery (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer
    ,  BatteryData * currentBatteryData);
private:
    PowerManager * manager;
    powerStatus_t status;
    bool ManualTurningPower;
    bool SignalTurningPowerUp;
    bool TurningUP;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _MANUAL_POWER_OFF_STATE_H_