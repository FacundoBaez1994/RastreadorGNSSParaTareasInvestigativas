//=====[Libraries]=============================================================

#include "PostHTTP.h"
#include "CellularModule.h" 
#include "Debugger.h" // due to global usbUart


//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 5
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of public methods]===================================
PostHTTP::PostHTTP () {
    this->mobileNetworkModule = NULL;
    this->readyToSend = true;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS; 
    this->currentStatus = SETTING_URL;
    this->jwt = new JWTManager ();
}


PostHTTP::PostHTTP (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS; 
    this->currentStatus = SETTING_URL;
    this->jwt = new JWTManager ();
}

PostHTTP::~PostHTTP () {
    this->mobileNetworkModule = NULL;
    delete this->jwt;
    this->jwt = nullptr;
}

void PostHTTP::enableTransceiver () {
    return;
}

CellularTransceiverStatus_t PostHTTP::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    static char StringToBeRead [500];
    static int dataLen;
    char StringToSendUSB [50] = "\r\nTRYING HTTP POST\r\n";
    char ExpectedResponse1 [3] = "OK";
    char ExpectedResponse2 [15] = "CONNECT";
    char PartialStringToSend1 [50] = "AT+QHTTPURL=";
    char StringToSend1 [50];
    char url [100] = "https://intent-lion-loudly.ngrok-free.app/api/canal/envio";
    char confirmationToSend[] = "\x1a";
    
    char PartialStringToSend2 [50] = "AT+QHTTPPOST=";
    char StringToSend2 [50]; 

    char PartialStringToSend3 [50] = "AT+QHTTPREAD=";
    char StringToSend3 [50];  
    static bool watingForResponse = false;
    static bool urlSet = false;

    int urlLength = strlen(url);

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
                //refreshTime->write(25000);
                refreshTime->write(10000);
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
                    refreshTime->restart();
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
                if (strcmp (StringToBeRead, ExpectedResponse1) == 0) { // OK
  
                    refreshTime->restart();
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
                PostResult_t postResult = checkHTTPPostResult (StringToBeRead, &dataLen);
                if (postResult == POST_OK) { 
  
                    uartUSB.write ( "\r\n",  3 );  // debug only
                    uartUSB.write ("POST Message success"  , strlen ("POST Message success"));  // debug only
                    uartUSB.write ( "\r\n",  3 );  // debug only
                    refreshTime->restart();
                    this->currentStatus = READING_DATA;
                    this->readyToSend  = true;      
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
                
            }
            break;
        case READING_DATA:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend3); // POST READ
                refreshTime->restart();
                this->readyToSend  = false;
                uartUSB.write ( "\r\n",  3 );
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
                        refreshTime->write(5000);
                        refreshTime->restart();
                        memset(receivedMessage, 0, 2048);
                        return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                    }
                }
            } else {
                if ( ATHandler->readATResponse ( StringToBeRead ) == true) { //
                    ////   ////   ////   ////   ////   ////
                    StringToBeRead [dataLen] = '\0';
                    refreshTime->restart();
                    uartUSB.write ("Read POST response\r\n"  , strlen ("Read POST response\r\n"));  // debug only
                    uartUSB.write (StringToBeRead  , strlen (StringToBeRead ));  // debug only
                    uartUSB.write ( "\r\n",  3 );  // debug only
                    watingForResponse = false;
                    this->currentStatus = DECODING_DATA;
                    this->readyToSend = false;
                    

                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }

            break;
        case DECODING_DATA:
            char  payloadRetrived [500];
            if (this->jwt->decodeJWT(StringToBeRead , payloadRetrived) == false) {
                uartUSB.write ("Error on decoding JWT:" , strlen ("Error on decoding JWT:"));  // debug only
                this->readyToSend  = true;
                this->currentStatus = READING_DATA;
                return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                break;
            }
            *newDataAvailable = true;
            strcpy (receivedMessage, payloadRetrived);
            uartUSB.write (receivedMessage , strlen (receivedMessage ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            this->currentStatus = SETTING_URL;
            this->mobileNetworkModule->changeTransceiverState  (new DeactivatePDP (this->mobileNetworkModule, true) );
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
            break;

        default:
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
   } // switch end

    if (refreshTime->read()) {
        this->readyToSend = true;
        refreshTime->write(10000);
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

//=====[Implementations of private methods]===================================

PostResult_t PostHTTP::checkHTTPPostResult(char * responseBuffer, int * dataLen) {
    if (strncmp(responseBuffer, "+QHTTPPOST: ", 12) == 0) {
        int result, httpStatus;
        *dataLen = -1;
        sscanf(responseBuffer, "+QHTTPPOST: %d,%d,%d", &result, &httpStatus, dataLen);

        uartUSB.write("Parsed POST result:\r\n", 23);
        uartUSB.write(responseBuffer, strlen(responseBuffer));
        uartUSB.write("\r\n", 2);
        if (result == 0  && (*dataLen > -1 && *dataLen < 2048)) {
            return POST_OK;
        }
        return POST_FAILURE;
    }
    return KEEP_TRYING_TO_POST;
}
