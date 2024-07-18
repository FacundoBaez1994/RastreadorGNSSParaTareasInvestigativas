//=====[#include guards - begin]===============================================

#ifndef _AT_COMMAND_HANDLER_H_
#define _AT_COMMAND_HANDLER_H_

//==================[Libraries]===============================================

#include "mbed.h"

#include <string>

//=====[Declaration of public data types]======================================



//=====[Declaration of public classes]=========================================
/*
 * 
 */
class ATCommandHandler {
public:

//=====[Declaration of public methods]=========================================
    ATCommandHandler (BufferedSerial * UART);
    virtual ~ATCommandHandler();
    void sendATCommand (char * ATCommandToBeSend);
    bool readATResponse (char * StringToBeRead);
    bool retrieveNumericalVauleFromResponse (  char * ATResponse, float numberRetrieved );
    BufferedSerial* getUART (void);

private:
//=====[Declaration of privates atributes]=========================================
    BufferedSerial* serialComunicationUART;
    int bufferIndex; // debug
    char StringRead [100];
    char buffer[100];
    
//=====[Declaration of privates methods]=========================================

};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _AT_COMMAND_HANDLER_H_