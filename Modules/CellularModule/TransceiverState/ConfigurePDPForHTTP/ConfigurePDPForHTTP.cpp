//=====[Libraries]=============================================================

#include "ConfigurePDPForHTTP.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 30
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
ConfigurePDPForHTTP::ConfigurePDPForHTTP () {
    this->mobileNetworkModule = nullptr;
    this->readyToSend = true;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS; 
    this->currentStatus = ASSING_PDPCONTEXT_TO_HTTP_STACK;
}

ConfigurePDPForHTTP::ConfigurePDPForHTTP (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS; 
    this->currentStatus = ASSING_PDPCONTEXT_TO_HTTP_STACK;
}

ConfigurePDPForHTTP::~ConfigurePDPForHTTP () {
    this->mobileNetworkModule = nullptr;
}

void ConfigurePDPForHTTP::enableTransceiver () {
    return;
}

CellularTransceiverStatus_t ConfigurePDPForHTTP::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    char StringToBeRead [200];
    char ExpectedResponse [15] = "OK";
    char StringToSend1 [50] = "AT+QHTTPCFG=\"contextid\",1";
    char StringToSendUSB [20] =  "CONFIGURING PDP";
    //char StringToSend2 [80] = "AT+QICSGP=1,1,\"internet.movil\",\"internet\",\"internet\",1";
    //internet.tuenti
    char StringToSend2 [80] = "AT+QICSGP=1,1,\"internet.movil\",\"internet\",\"internet\",3";
    char StringToSend3 [50] = "AT+QIACT=1";
    char StringToSend4 [50] = "AT+QIACT?";
    char StringToSend5 [50] = "AT+QIDNSCFG=1,\"8.8.8.8\",\"8.8.4.4\"";
    // 
    //char StringToSend6 [50] = "AT+QIDNSGIP=1,\"dns.google\"";
    char StringToSend6 [50] = "AT+QIDNSGIP=1,\"intent-lion-loudly.ngrok-free.app\"";
   
   switch (this->currentStatus) {
       case ASSING_PDPCONTEXT_TO_HTTP_STACK:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend1); // AT+QHTTPCFG=\"contextid\",1";
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

                if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                    ////   ////   ////   ////   ////   ////     
                    this->currentStatus = CONFIGURING_PDP_PROFILE;
                    this->readyToSend  = true;
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }
        break;
        case CONFIGURING_PDP_PROFILE:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend2); // AT+QICSGP=1,1,\"internet.movil\",\"\" ,\"\" ,0
                this->readyToSend  = false;
                uartUSB.write (StringToSend2  , strlen (StringToSend2  ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
            }
                
            if ( ATHandler->readATResponse ( StringToBeRead) == true) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////

                if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                    ////   ////   ////   ////   ////   ////     
                    this->currentStatus = ACTIVATING_PDP_CONTEXT;
                    this->readyToSend  = true;
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }
            break;
        case ACTIVATING_PDP_CONTEXT:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend3);
                this->readyToSend  = false;
                uartUSB.write (StringToSend3  , strlen (StringToSend3  ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
            }
                
            if ( ATHandler->readATResponse ( StringToBeRead) == true) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////

                if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                    ////   ////   ////   ////   ////   ////     
                    
                    this->readyToSend  = true;
                    this->currentStatus =  QUERING_PDP_CONTEXT;
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            }
            break;
        case QUERING_PDP_CONTEXT:
             if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend4);
                this->readyToSend  = false;
                uartUSB.write (StringToSend4  , strlen (StringToSend4  ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
            }
                            
            if ( ATHandler->readATResponse ( StringToBeRead) == true) {  // shows IP
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only 
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////
                this->readyToSend  = true;
                // CHANGE STATE
                this->currentStatus =  CONFIGURING_SERVER_DNS;
                return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
            }
            break;
        case CONFIGURING_SERVER_DNS:
            //this->currentStatus = PING_SERVER_DNS;
            //return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
            

            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend5);
                this->readyToSend  = false;
                uartUSB.write (StringToSend5  , strlen (StringToSend5  ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
            }
                            
            if ( ATHandler->readATResponse ( StringToBeRead) == true) {  // shows IP
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only 
                uartUSB.write ( "\r\n",  3 );  // debug only
                
                if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                    ////   ////   ////   ////   ////   ////     
                    this->readyToSend  = true;
                    
                    this->currentStatus = ACTIVATING_PDP_CONTEXT;
                    //this->currentStatus =  PING_SERVER_DNS;
                    this->mobileNetworkModule->changeTransceiverState(new ConfigureSSL(this->mobileNetworkModule));
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
            }

            break;
        case PING_SERVER_DNS:
            if (this->readyToSend == true) {
                ATHandler->sendATCommand(StringToSend6);
                this->readyToSend  = false;
                uartUSB.write (StringToSend6  , strlen (StringToSend6  ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////   
            }
                            
            if ( ATHandler->readATResponse ( StringToBeRead) == true) {  // shows IP
                uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only 
                uartUSB.write ( "\r\n",  3 );  // debug only
                
                if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                    ////   ////   ////   ////   ////   ////     
                    //this->currentStatus = ACTIVATING_PDP_CONTEXT;
                    this->readyToSend  = true;
                    
                    // CHANGE STATE
                    //this->mobileNetworkModule->changeTransceiverState(new PostHTTP(this->mobileNetworkModule));
                    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
                }
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
            }

            break;
        default:
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
   }




    if (refreshTime->read()) {
        this->readyToSend = true;
        this->Attempts++;
        if (this->Attempts >= this->maxAttempts) {
            return CELLULAR_TRANSCEIVER_STATUS_FAIL_TO_ACTIVATE_PDP;
        }
    }

    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
}



//=====[Implementations of private functions]==================================
