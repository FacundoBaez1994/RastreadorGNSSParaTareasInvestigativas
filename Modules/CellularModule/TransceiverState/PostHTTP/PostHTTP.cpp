//=====[Libraries]=============================================================

#include "PostHTTP.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart


//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 5
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============




//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of private methods]===================================
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================
/** 
* @brief
* 
* @param 
*/
PostHTTP::PostHTTP () {
    this->mobileNetworkModule = NULL;
    this->readyToSend = true;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS; 
    this->currentStatus = SETTING_URL;
}


/** 
* @brief
* 
* @param 
*/
PostHTTP::PostHTTP (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS; 
    this->currentStatus = SETTING_URL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
PostHTTP::~PostHTTP () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
void PostHTTP::enableTransceiver () {
    return;
}

/** 
* @brief 
* 
* 
* @returns 
*/
CellularTransceiverStatus_t PostHTTP::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    char StringToBeRead [2000];
    char StringToSendUSB [50] = "\r\nTRYING HTTP POST\r\n";
    char ExpectedResponse1 [3] = "OK";
    char ExpectedResponse2 [15] = "CONNECT";
    char PartialStringToSend1 [50] = "AT+QHTTPURL=";
    char StringToSend1 [50];
    //"http://intent-lion-loudly.ngrok-free.app/api/canal/envio"
    char url [100] = "https://intent-lion-loudly.ngrok-free.app/api/canal/envio";
    //char url [100] = "http://webhook.site/572d2d5b-404f-42c7-be4e-5a22592009f7";
    char confirmationToSend[] = "\x1a";
    

    char PartialStringToSend2 [50] = "AT+QHTTPPOST=";
    char StringToSend2 [50]; 

    char PartialStringToSend3 [50] = "AT+QHTTPREAD=";
    char StringToSend3 [50];  
    static bool watingForResponse = false;
    static bool urlSet = false;
    

    int urlLength = strlen(url);

    // Tiempo máximo de ingreso del URL (en segundos)
    int inputTimeout = 80;

    int responseTimeout = 80;

    sprintf(StringToSend1, "AT+QHTTPURL=%d,%d", urlLength, inputTimeout);

    sprintf(StringToSend2, "AT+QHTTPPOST=%d,%d,%d", strlen(message), inputTimeout, responseTimeout);

    sprintf(StringToSend3, "AT+QHTTPREAD=%d", responseTimeout);



   
   switch (this->currentStatus) {
       case SETTING_URL:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend1);
                this->readyToSend  = false;
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                uartUSB.write (StringToSend1  , strlen (StringToSend1  ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
                refreshTime->write(25000);
                refreshTime->restart();
            }
                
            if ( ATHandler->readATResponse ( StringToBeRead) == true) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////

                if (strcmp (StringToBeRead, ExpectedResponse2) == 0) { // CONNECT
                    ////   ////   ////   ////   ////   ////     
                    ATHandler->sendATCommand(url);
                    ////   ////   ////   ////   ////   ////
                    ATHandler->sendATCommand(confirmationToSend);
                    uartUSB.write (url  , strlen (url ));  // debug only
                    uartUSB.write ( "\r\n",  3 );  // debug only
                    urlSet = true;
                }
                
                if (strcmp (StringToBeRead, ExpectedResponse1) == 0 && urlSet == true) { // OK
                    ////   ////   ////   ////   ////   ////     
                    this->currentStatus = POSTING_DATA;
                    this->readyToSend = true;
                    urlSet = false;
                }
            }
            break;
        case POSTING_DATA:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend2); // POST
                this->readyToSend  = false;
                uartUSB.write (StringToSend2  , strlen (StringToSend2  ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
            }
                
            if ( ATHandler->readATResponse ( StringToBeRead) == true) { //
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////

                if (strcmp (StringToBeRead, ExpectedResponse2) == 0) { // CONNECT
                    ////   ////   ////   ////   ////   ////  
                    ATHandler->sendATCommand(message);   // The message 
                    ATHandler->sendATCommand(confirmationToSend);
                    uartUSB.write ( "\r\n",  3 );  // debug only
                    uartUSB.write ("POST Message\r\n"  , strlen ("POST Message\r\n"));  // debug only
                    uartUSB.write (message, strlen(message));
                    uartUSB.write ( "\r\n",  3 );  // debug only

                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
                if (strcmp (StringToBeRead, ExpectedResponse1) == 0) { // OK
  
                    uartUSB.write ( "\r\n",  3 );  // debug only
                    uartUSB.write ("POST Message success"  , strlen ("POST Message success"));  // debug only
                    uartUSB.write ( "\r\n",  3 );  // debug only

                    this->currentStatus = READING_DATA;
                    this->readyToSend  = true;      
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }
            break;
        case READING_DATA:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend3); // POST READ
                this->readyToSend  = false;
                uartUSB.write (StringToSend3  , strlen (StringToSend3 ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
            }
            if ( watingForResponse == false) {
                if ( ATHandler->readATResponse ( StringToBeRead) == true) { //
                    ////   ////   ////   ////   ////   ////
                    uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
                    uartUSB.write ( "\r\n",  3 );  // debug only
                    ////   ////   ////   ////   ////   ////
                    if (strcmp (StringToBeRead, ExpectedResponse2) == 0) { // CONNECT
                        ////   ////   ////   ////   ////   ////  
                        watingForResponse = true;
                        return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                    }
                }
            } else {
                if ( ATHandler->readATResponse ( receivedMessage ) == true) { //
                    ////   ////   ////   ////   ////   ////
                    uartUSB.write ("Read POST response\r\n"  , strlen ("Read POST response\r\n"));  // debug only
                    uartUSB.write (receivedMessage , strlen (receivedMessage));  // debug only
                    uartUSB.write ( "\r\n",  3 );  // debug only
                    watingForResponse = false;
                    this->currentStatus = SETTING_URL;
                    ////   ////   ////   ////   ////   ////
                    *newDataAvailable = true;
                    this->mobileNetworkModule->changeTransceiverState  (new DeactivatePDP (this->mobileNetworkModule, true) );
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }

            break;
        default:
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
   }




    if (refreshTime->read()) {
        this->readyToSend = true;
        this->Attempts++;
        if (this->Attempts >= this->maxAttempts && watingForResponse == false) {
            this->currentStatus = SETTING_URL;
             this->mobileNetworkModule->changeTransceiverState  (new DeactivatePDP (this->mobileNetworkModule, false) );
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
        }
        if (this->Attempts >= this->maxAttempts && watingForResponse == true) {
            this->currentStatus = SETTING_URL;
             this->mobileNetworkModule->changeTransceiverState  (new DeactivatePDP (this->mobileNetworkModule, true) );
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
        }
    }

    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
}



//=====[Implementations of private functions]==================================
