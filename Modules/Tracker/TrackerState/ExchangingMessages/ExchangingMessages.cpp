//=====[Libraries]=============================================================

#include "ExchangingMessages.h"
#include "GatheringInertialData.h"
#include "LoadingMessage.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "GoingToSleep.h"
#include "SavingMessage.h"
#include "FormattingMessage.h"
#include "GatheringInertialData.h"

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
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
ExchangingMessages::ExchangingMessages (Tracker * tracker, trackerStatus_t trackerStatus) {
    this->tracker = tracker;
    this->currentStatus = trackerStatus;
}

/** 
* @brief
* 
* @param 
*/
ExchangingMessages::~ExchangingMessages () {
    this->tracker = NULL;
}

void ExchangingMessages::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void ExchangingMessages::exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage ){
    static CellularTransceiverStatus_t currentTransmitionStatus;
    static bool newDataAvailable = false;
    static bool enableTransceiver = false;
    static char payloadRetrived [2048];
    char logMessage [100];
    
    // if conected to mobile network send the message throght LTE Modem
    if (this->currentStatus == TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK
     || this->currentStatus == TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK 
     || this->currentStatus == TRACKER_STATUS_GNSS_LOADED_MESSAGE
     || this->currentStatus ==  TRACKER_STATUS_IMU_LOADED_MESSAGE) {
        if (enableTransceiver == false) {
            cellularTransceiver->enableTransceiver();
            enableTransceiver = true; 
        }
       currentTransmitionStatus = cellularTransceiver->exchangeMessages (message, socketTargetted,
       receivedMessage, &newDataAvailable);

        if (currentTransmitionStatus == CELLULAR_TRANSCEIVER_STATUS_SEND_OK) {
            snprintf(logMessage, sizeof(logMessage), "The message was send with success");
            uartUSB.write (logMessage , strlen (logMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}

        if (newDataAvailable == true) {

            //////////////////   MESSAGE INTERPRETATION ////////////////
            ////////////////////////////////////////////////////////////////
            snprintf(logMessage, sizeof(logMessage),"new Message received:");
            uartUSB.write (logMessage , strlen (logMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            if (this->tracker->decodeJWT(receivedMessage, payloadRetrived) == false) {
                // new state formatting Message in order to be saved in memory
                snprintf(logMessage, sizeof(logMessage),"Error on decoding JWT:");
                uartUSB.write (logMessage , strlen (logMessage));  // debug only
                newDataAvailable = false;
                enableTransceiver = false;
                if (this->currentStatus == TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK) {
                    this->currentStatus = TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK_SAVING_MESSAGE;
                }
                if (this->currentStatus == TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK) {
                    this->currentStatus = TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK_SAVING_MESSAGE;
                }
                if (this->currentStatus == TRACKER_STATUS_GNSS_LOADED_MESSAGE) {
                    this->currentStatus = TRACKER_STATUS_GNSS_OBTAIN_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_LORA_UNAVAILABLE_SAVING_MESSAGE;
                }
                this->tracker->changeState (new FormattingMessage (this->tracker, this->currentStatus));
                return;
            }
            strcpy (receivedMessage, payloadRetrived);
            uartUSB.write (receivedMessage , strlen (receivedMessage ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            newDataAvailable = false;
            enableTransceiver = false;

            char success[30];
            char latency[30];
            char mode[30];

            if (extractField(receivedMessage, "\"SUCCESS\"", success, sizeof(success)) == false) {
                snprintf(logMessage, sizeof(logMessage), "Corrupted Server Message\r\n");
                uartUSB.write(logMessage, strlen(logMessage));

                newDataAvailable = false;
                enableTransceiver = false;
                if (this->currentStatus == TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK) {
                    this->currentStatus = TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK_SAVING_MESSAGE;
                }
                if (this->currentStatus == TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK) {
                    this->currentStatus = TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK_SAVING_MESSAGE;
                }
                if (this->currentStatus == TRACKER_STATUS_GNSS_LOADED_MESSAGE) {
                    this->currentStatus = TRACKER_STATUS_GNSS_OBTAIN_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_LORA_UNAVAILABLE_SAVING_MESSAGE;
                }
                // new state formatting Message in order to be saved in memory
                this->tracker->changeState (new FormattingMessage (this->tracker, this->currentStatus));
                return;
            }
            snprintf(logMessage, sizeof(logMessage), "SUCCESS: %s\r\n", success);
            uartUSB.write(logMessage, strlen(logMessage));

            if (strcmp (success, "true") != 0) {
                snprintf(logMessage, sizeof(logMessage), "Server returns error\r\n");
                uartUSB.write(logMessage, strlen(logMessage));

                newDataAvailable = false;
                enableTransceiver = false;
                if (this->currentStatus == TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK) {
                    this->currentStatus = TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK_SAVING_MESSAGE;
                }
                if (this->currentStatus == TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK) {
                    this->currentStatus = TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK_SAVING_MESSAGE;
                }
                if (this->currentStatus == TRACKER_STATUS_GNSS_LOADED_MESSAGE) {
                    this->currentStatus = TRACKER_STATUS_GNSS_OBTAIN_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_LORA_UNAVAILABLE_SAVING_MESSAGE;
                }
                // new state formatting Message in order to be saved in memory
                this->tracker->changeState (new FormattingMessage (this->tracker, this->currentStatus));
                return;
            }

            if (extractField(receivedMessage, "\"LATENCY\"", latency, sizeof(latency)) == false) {    
                newDataAvailable = false;
                enableTransceiver = false;
                this->tracker->changeState (new LoadingMessage (this->tracker));
                return;
            }
            snprintf(logMessage, sizeof(logMessage), "LATENCY: %s\r\n", latency);
            uartUSB.write(logMessage, strlen(logMessage));
            LatencyLevel_t newLatencyLevel;
            if (this->parseLatencyLevel(latency, &newLatencyLevel)) {
                this->tracker->setLatency(newLatencyLevel);
            }
            

            if (extractField(receivedMessage, "\"MODE\"", mode, sizeof(mode)) == false) {
                newDataAvailable = false;
                enableTransceiver = false;
                this->tracker->changeState (new LoadingMessage (this->tracker));
                return;
            }
            OperationMode_t newOperationMode;
            if (this->parseOperationMode(mode, &newOperationMode)) {
                this->tracker->setOperationMode(newOperationMode);
            }


            snprintf(logMessage, sizeof(logMessage), "MODE: %s\r\n", mode);
            uartUSB.write(logMessage, strlen(logMessage));
            

            newDataAvailable = false;
            enableTransceiver = false;
            this->tracker->changeState (new LoadingMessage (this->tracker));
            return;
            //////////////////   MESSAGE INTERPRETATION ////////////////
            ////////////////////////////////////////////////////////////////
            
            } else {
                snprintf(logMessage, sizeof(logMessage),"No Messages received:");
                uartUSB.write (logMessage , strlen (logMessage));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only}
                newDataAvailable = false;
                enableTransceiver = false;
                this->tracker->changeState (new LoadingMessage (this->tracker));
                return;
            }    
        }  else if (currentTransmitionStatus != CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND
        && currentTransmitionStatus != CELLULAR_TRANSCEIVER_STATUS_UNAVAIBLE) {

            snprintf(logMessage, sizeof(logMessage),"The message couldn't be sent");
            uartUSB.write (logMessage , strlen (logMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            newDataAvailable = false;
            enableTransceiver = false;
            if (this->currentStatus == TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK) {
                this->currentStatus = TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK_SAVING_MESSAGE;
            }
            if (this->currentStatus == TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK) {
                this->currentStatus = TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK_SAVING_MESSAGE;
            }
            if (this->currentStatus == TRACKER_STATUS_GNSS_LOADED_MESSAGE) {
                this->currentStatus = TRACKER_STATUS_GNSS_OBTAIN_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_LORA_UNAVAILABLE_SAVING_MESSAGE;
            }
            // new state formatting Message in order to be saved in memory
            this->tracker->changeState (new FormattingMessage (this->tracker, this->currentStatus));
            return;
        }
    } else if (this->currentStatus == TRACKER_STATUS_GNSS_OBTAIN_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_TRYING_LORA || 
    this->currentStatus == TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_TRYING_LORA) {
        /// LORAS STUFF
        snprintf(logMessage, sizeof(logMessage),"LoRa Sending");
        uartUSB.write (logMessage , strlen (logMessage));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only}
        
        // try with LoRa
        // NOTA: ESTA PARTE DEBERIA DE SER UNO O DOS ESTADOS APARTE

        snprintf(logMessage, sizeof(logMessage),"LoRa Unavailable");
        uartUSB.write (logMessage , strlen (logMessage));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only}
        if (this->currentStatus == TRACKER_STATUS_GNSS_OBTAIN_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_TRYING_LORA ){
            this->tracker->changeState (new FormattingMessage (this->tracker, TRACKER_STATUS_GNSS_OBTAIN_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_LORA_UNAVAILABLE_SAVING_MESSAGE));
            return;
        }
        // aca iria a gathering otra vez para llenar el vector
        this->tracker->changeState (new GatheringInertialData (this->tracker, TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_LORA_UNAVAILABLE_GATHERING_INERTIAL_INFO));
        return;
    }

  
    return;
}
 
//=====[Implementations of private methods]==================================


bool ExchangingMessages::extractField(const char* json, const char* key, char* output, size_t maxLen) {
    const char* found = strstr(json, key);
    if (!found) return false;

    found = strchr(found, ':');
    if (!found) return false;

    found++; // avanzar al valor
    while (*found == ' ' || *found == '\"') found++; // salteamos espacios o comillas

    size_t i = 0;
    while (*found && *found != '\"' && *found != ',' && i < maxLen - 1) {
        output[i++] = *found++;
    }
    output[i] = '\0';
    return true;
}

bool ExchangingMessages::parseLatencyLevel(const char* latencyStr, LatencyLevel_t * newLatencyLevel) {
    if (strcmp(latencyStr, "EXTREMELY_LOW_LATENCY") == 0) {
        *newLatencyLevel = EXTREMELY_LOW_LATENCY;
        return true;
    } 
    if (strcmp(latencyStr, "VERY_LOW_LATENCY") == 0) {
        *newLatencyLevel = VERY_LOW_LATENCY;
        return true;
    } 
    if (strcmp(latencyStr, "LOW_LATENCY") == 0) {
        *newLatencyLevel = LOW_LATENCY;
        return true;
    } 
    if (strcmp(latencyStr, "MEDIUM_LATENCY") == 0) {
        *newLatencyLevel = MEDIUM_LATENCY;
        return true;
    } 
    if (strcmp(latencyStr, "HIGH_LATENCY") == 0) {
        *newLatencyLevel = HIGH_LATENCY;
        return true;
    } 
    if (strcmp(latencyStr, "VERY_HIGH_LATENCY") == 0) {
        *newLatencyLevel = VERY_HIGH_LATENCY;
        return true;
    } 
    if (strcmp(latencyStr, "EXTREMELY_HIGH_LATENCY") == 0) {
        *newLatencyLevel = EXTREMELY_HIGH_LATENCY;
        return true;
    } 
    return false;
}


bool ExchangingMessages::parseOperationMode(const char* operationModeStr, OperationMode_t * newOperationMode) {
    if (strcmp(operationModeStr, "NORMAL_OPERATION_MODE") == 0) {
        *newOperationMode = NORMAL_OPERATION_MODE;
        return true;
    } 
    if (strcmp(operationModeStr, "PURSUIT_OPERATION_MODE") == 0) {
        *newOperationMode = PURSUIT_OPERATION_MODE;
        return true;
    } 
    if (strcmp(operationModeStr, "SILENT_OPERATION_MODE") == 0) {
        *newOperationMode = SILENT_OPERATION_MODE;
        return true;
    } 
    return false;
}
