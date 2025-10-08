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
#define LOG_MESSAGE_BUFFER_SIZE 100  
#define JSON_HEADER_MAX_BUFFER 30

#define LOG_MESSAGE_SEND_OK                         "The message was sent with success\r\n"
#define LOG_MESSAGE_SEND_OK_LEN                     (sizeof(LOG_MESSAGE_SEND_OK) - 1)

#define LOG_MESSAGE_ERROR_DECODING_JWT              "error decoding JWT\r\n"
#define LOG_MESSAGE_ERROR_DECODING_JWT_LEN          (sizeof(LOG_MESSAGE_ERROR_DECODING_JWT) - 1)

#define LOG_MESSAGE_CORRUPTED_SERVER_MESSAGE        "Corrupted Server Message\r\n"
#define LOG_MESSAGE_CORRUPTED_SERVER_MESSAGE_LEN    (sizeof(LOG_MESSAGE_CORRUPTED_SERVER_MESSAGE) - 1)

#define LOG_MESSAGE_SERVER_RETURN_ERROR             "Server returns error\r\n"
#define LOG_MESSAGE_SERVER_RETURN_ERROR_LEN         (sizeof(LOG_MESSAGE_SERVER_RETURN_ERROR) - 1)

#define LOG_MESSAGE_NO_MESSAGES_RECEIVED            "No Messages received\r\n"
#define LOG_MESSAGE_NO_MESSAGES_RECEIVED_LEN        (sizeof(LOG_MESSAGE_NO_MESSAGES_RECEIVED) - 1)

#define LOG_MESSAGE_MESSAGE_COULD_NOT_BE_SENT       "The message couldn't be sent\r\n"
#define LOG_MESSAGE_MESSAGE_COULD_NOT_BE_SENT_LEN   (sizeof(LOG_MESSAGE_MESSAGE_COULD_NOT_BE_SENT) - 1)

#define LOG_MESSAGE_REDIRECT_TO_LORA                "Redirect to LoRa Sending\r\n"
#define LOG_MESSAGE_REDIRECT_TO_LORA_LEN            (sizeof(LOG_MESSAGE_REDIRECT_TO_LORA) - 1)

#define LOG_MESSAGE_LATENCY_PREFIX                  "LATENCY: "
#define LOG_MESSAGE_LATENCY_PREFIX_LEN              (sizeof(LOG_MESSAGE_LATENCY_PREFIX) - 1)

#define LOG_MESSAGE_SUCCESS_PREFIX                  "SUCCESS: "
#define LOG_MESSAGE_SUCCESS_PREFIX_LEN              (sizeof(LOG_MESSAGE_SUCCESS_PREFIX) - 1)

#define LOG_MESSAGE_MODE_PREFIX                     "MODE: "
#define LOG_MESSAGE_MODE_PREFIX_LEN                 (sizeof(LOG_MESSAGE_MODE_PREFIX) - 1)

#define LOG_MESSAGE_SILENT_MODE_PREFIX              "Setted Silent mode of "
#define LOG_MESSAGE_SILENT_MODE_PREFIX_LEN          (sizeof(LOG_MESSAGE_SILENT_MODE_PREFIX) - 1)
#define LOG_MESSAGE_SILENT_MODE_SUFFIX              " hours\r\n"
#define LOG_MESSAGE_SILENT_MODE_SUFFIX_LEN          (sizeof(LOG_MESSAGE_SILENT_MODE_SUFFIX) - 1)

#define LOG_MESSAGE_LATENCY_EXTREMELY_LOW           "\r\nLatency level: EXTREMELY_LOW_LATENCY\r\n"
#define LOG_MESSAGE_LATENCY_EXTREMELY_LOW_LEN       (sizeof(LOG_MESSAGE_LATENCY_EXTREMELY_LOW) - 1)

#define LOG_MESSAGE_LATENCY_VERY_LOW                "\r\nLatency level: VERY_LOW_LATENCY\r\n"
#define LOG_MESSAGE_LATENCY_VERY_LOW_LEN            (sizeof(LOG_MESSAGE_LATENCY_VERY_LOW) - 1)

#define LOG_MESSAGE_LATENCY_LOW                     "\r\nLatency level: LOW_LATENCY\r\n"
#define LOG_MESSAGE_LATENCY_LOW_LEN                 (sizeof(LOG_MESSAGE_LATENCY_LOW) - 1)

#define LOG_MESSAGE_LATENCY_MEDIUM                  "\r\nLatency level: MEDIUM_LATENCY\r\n"
#define LOG_MESSAGE_LATENCY_MEDIUM_LEN              (sizeof(LOG_MESSAGE_LATENCY_MEDIUM) - 1)

#define LOG_MESSAGE_LATENCY_HIGH                    "\r\nLatency level: HIGH_LATENCY\r\n"
#define LOG_MESSAGE_LATENCY_HIGH_LEN                (sizeof(LOG_MESSAGE_LATENCY_HIGH) - 1)

#define LOG_MESSAGE_LATENCY_VERY_HIGH               "\r\nLatency level: VERY_HIGH_LATENCY\r\n"
#define LOG_MESSAGE_LATENCY_VERY_HIGH_LEN           (sizeof(LOG_MESSAGE_LATENCY_VERY_HIGH) - 1)

#define LOG_MESSAGE_LATENCY_EXTREMELY_HIGH          "\r\nLatency level: EXTREMELY_HIGH_LATENCY\r\n"
#define LOG_MESSAGE_LATENCY_EXTREMELY_HIGH_LEN      (sizeof(LOG_MESSAGE_LATENCY_EXTREMELY_HIGH) - 1)

#define LOG_MESSAGE_OPERATION_MODE_NORMAL           "\r\nOperation Mode: NORMAL_OPERATION_MODE\r\n"
#define LOG_MESSAGE_OPERATION_MODE_NORMAL_LEN       (sizeof(LOG_MESSAGE_OPERATION_MODE_NORMAL) - 1)

#define LOG_MESSAGE_OPERATION_MODE_PURSUIT          "\r\nOperation Mode: PURSUIT_OPERATION_MODE\r\n"
#define LOG_MESSAGE_OPERATION_MODE_PURSUIT_LEN      (sizeof(LOG_MESSAGE_OPERATION_MODE_PURSUIT) - 1)

#define LOG_MESSAGE_OPERATION_MODE_SILENT           "\r\nOperation Mode: SILENT_OPERATION_MODE\r\n"
#define LOG_MESSAGE_OPERATION_MODE_SILENT_LEN       (sizeof(LOG_MESSAGE_OPERATION_MODE_SILENT) - 1)


#define JSON_MESSAGE_FIELD_EXCHANGE_SUCCESS "\"SUCS\""
#define JSON_MESSAGE_FIELD_LATENCY "\"LTCY\""
#define JSON_MESSAGE_FIELD_OPERATION_MODE "\"MODE\""
#define JSON_MESSAGE_FIELD_TIME_SILENT "\"TSOP\""

#define JSON_SUCCESS_OK_1               "true"
#define JSON_SUCCESS_OK_2               "TRUE"

#define JSON_LATENCY_EXTREMELY_LOW                  "ELL"
#define JSON_LATENCY_VERY_LOW                       "VLL"
#define JSON_LATENCY_LOW                            "LL"
#define JSON_LATENCY_MEDIUM                         "ML"
#define JSON_LATENCY_HIGH                           "HL"
#define JSON_LATENCY_VERY_HIGH                      "VHL"
#define JSON_LATENCY_EXTREMELY_HIGH                 "EHL"

#define JSON_OPERATION_MODE_NORMAL_OPERATION        "NOPM"
#define JSON_OPERATION_MODE_PURSUIT_OPERATION       "POPM"
#define JSON_OPERATION_MODE_SILENT_OPERATION        "SOPM"
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
ExchangingMessages::ExchangingMessages (Tracker * tracker, trackerStatus_t trackerStatus) {
    this->tracker = tracker;
    this->currentStatus = trackerStatus;
}

ExchangingMessages::~ExchangingMessages () {
    this->tracker = nullptr;
}

void ExchangingMessages::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }


void ExchangingMessages::exchangeMessages (CellularModule* cellularTransceiver,
    char* message,  RemoteServerInformation* serverTargetted, char* receivedMessage ) {

    static CellularTransceiverStatus_t currentTransmitionStatus;
    static bool newDataAvailable = false;
    static bool enableTransceiver = false;
    char logMessage [LOG_MESSAGE_BUFFER_SIZE];
    char payloadRetrieved [RECEPTION_MN_BUFFER_SIZE];
    
    // if conected to mobile network send the message throght LTE Modem
    if (this->currentStatus == TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK
     || this->currentStatus == TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK 
     || this->currentStatus == TRACKER_STATUS_GNSS_LOADED_MESSAGE
     || this->currentStatus ==  TRACKER_STATUS_IMU_LOADED_MESSAGE) {
        if (enableTransceiver == false) {
            cellularTransceiver->enableTransceiver();
            enableTransceiver = true; 
        }
       currentTransmitionStatus = cellularTransceiver->exchangeMessages (message, serverTargetted,
       receivedMessage, &newDataAvailable);

        if (currentTransmitionStatus == CELLULAR_TRANSCEIVER_STATUS_SEND_OK) {
            uartUSB.write (LOG_MESSAGE_SEND_OK , LOG_MESSAGE_SEND_OK_LEN);  // debug only

        if (newDataAvailable == true) {

            //////////////////   MESSAGE INTERPRETATION ////////////////
            ////////////////////////////////////////////////////////////////
            if (this->tracker->decodeJWT(receivedMessage, payloadRetrieved) == false) {
                uartUSB.write(LOG_MESSAGE_ERROR_DECODING_JWT, LOG_MESSAGE_ERROR_DECODING_JWT_LEN);

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
            strcpy (receivedMessage, payloadRetrieved);

            newDataAvailable = false;
            enableTransceiver = false;

            char success[JSON_HEADER_MAX_BUFFER];
            char latency[JSON_HEADER_MAX_BUFFER];
            char mode[JSON_HEADER_MAX_BUFFER];
            char timeSilentString [JSON_HEADER_MAX_BUFFER];
            int timeSilent;

            if (extractField(receivedMessage, JSON_MESSAGE_FIELD_EXCHANGE_SUCCESS, success, sizeof(success)) == false) {
                uartUSB.write(LOG_MESSAGE_CORRUPTED_SERVER_MESSAGE, LOG_MESSAGE_CORRUPTED_SERVER_MESSAGE_LEN);

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
            snprintf(logMessage, sizeof(logMessage), "%s %s\r\n",LOG_MESSAGE_SUCCESS_PREFIX, success);
            uartUSB.write(logMessage, strlen(logMessage));


            if (strcmp (success, JSON_SUCCESS_OK_1) != 0 && strcmp (success, JSON_SUCCESS_OK_2) != 0) {
                uartUSB.write(LOG_MESSAGE_SERVER_RETURN_ERROR, LOG_MESSAGE_SERVER_RETURN_ERROR_LEN);

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

            
            this->tracker->progressOnHashChain ();
            this->tracker->increaseSequenceNumber ();

            if (extractField(receivedMessage, JSON_MESSAGE_FIELD_LATENCY, latency, sizeof(latency)) == false) {    
                newDataAvailable = false;
                enableTransceiver = false;
                this->tracker->changeState (new LoadingMessage (this->tracker));
                return;
            }
            snprintf(logMessage, sizeof(logMessage), "%s %s\r\n",LOG_MESSAGE_LATENCY_PREFIX, latency);
            uartUSB.write(logMessage, strlen(logMessage));
            LatencyLevel_t newLatencyLevel;
            if (this->parseLatencyLevel(latency, &newLatencyLevel)) {
                this->tracker->setLatency(newLatencyLevel);
            }
            

            if (extractField(receivedMessage, JSON_MESSAGE_FIELD_OPERATION_MODE, mode, sizeof(mode)) == false) {
                newDataAvailable = false;
                enableTransceiver = false;
                this->tracker->changeState (new LoadingMessage (this->tracker));
                return;
            }
            OperationMode_t newOperationMode;
            int hoursSilentMode;
            if (this->parseOperationMode(mode, &newOperationMode)) {
                if (newOperationMode == SILENT_OPERATION_MODE) {
                    if (extractField(receivedMessage, JSON_MESSAGE_FIELD_TIME_SILENT, timeSilentString, sizeof(timeSilentString)) == false) {
                        newDataAvailable = false;
                        enableTransceiver = false;
                        this->tracker->changeState (new LoadingMessage (this->tracker));
                        return;
                    }
                    timeSilent = std::atoi(timeSilentString);
                    if ( timeSilent < 0) {
                        newDataAvailable = false;
                        enableTransceiver = false;
                        this->tracker->changeState (new LoadingMessage (this->tracker));
                        return;
                    }
                    snprintf(logMessage, sizeof(logMessage), "%s %i %s",LOG_MESSAGE_SILENT_MODE_PREFIX, timeSilent, LOG_MESSAGE_SILENT_MODE_SUFFIX );
                    uartUSB.write(logMessage, strlen(logMessage));
                    this->tracker->setOperationMode(newOperationMode);
                    this->tracker->setSilentTimer(timeSilent);
                } else {
                    this->tracker->setOperationMode(newOperationMode);
                }

            }

            snprintf(logMessage, sizeof(logMessage), "%s %s\r\n",LOG_MESSAGE_MODE_PREFIX, mode);
            uartUSB.write(logMessage, strlen(logMessage));
            

            newDataAvailable = false;
            enableTransceiver = false;
            this->tracker->changeState (new LoadingMessage (this->tracker));
            return;
            //////////////////   MESSAGE INTERPRETATION ////////////////
            ////////////////////////////////////////////////////////////////
            
            } else {
                uartUSB.write (LOG_MESSAGE_NO_MESSAGES_RECEIVED , LOG_MESSAGE_NO_MESSAGES_RECEIVED_LEN);  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only}
                newDataAvailable = false;
                enableTransceiver = false;
                this->tracker->changeState (new LoadingMessage (this->tracker));
                return;
            }    
        }  else if (currentTransmitionStatus != CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND
        && currentTransmitionStatus != CELLULAR_TRANSCEIVER_STATUS_UNAVAIBLE) {

            uartUSB.write (LOG_MESSAGE_MESSAGE_COULD_NOT_BE_SENT , LOG_MESSAGE_MESSAGE_COULD_NOT_BE_SENT_LEN);  // debug only
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
            //this->tracker->changeState (new LoadingMessage (this->tracker)); // ELIMINAR
            this->tracker->changeState (new FormattingMessage (this->tracker, this->currentStatus));
            return;
        }
    } else if (this->currentStatus == TRACKER_STATUS_GNSS_OBTAIN_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_TRYING_LORA || 
    this->currentStatus == TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_TRYING_LORA) {
        uartUSB.write (LOG_MESSAGE_REDIRECT_TO_LORA , LOG_MESSAGE_REDIRECT_TO_LORA_LEN);  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only}

        if (this->currentStatus == TRACKER_STATUS_GNSS_OBTAIN_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_TRYING_LORA ){
            this->tracker->changeState (new FormattingMessage (this->tracker, TRACKER_STATUS_GNSS_OBTAIN_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_LORA_UNAVAILABLE_SAVING_MESSAGE));
            return;
        }
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

    found++; 
    while (*found == ' ' || *found == '\"') found++; // salteamos espacios o comillas

    size_t i = 0;
    while (*found && *found != '\"' && *found != ',' && i < maxLen - 1) {
        output[i++] = *found++;
    }
    output[i] = '\0';
    return true;
}

bool ExchangingMessages::parseLatencyLevel(const char* latencyStr, LatencyLevel_t * newLatencyLevel) {

    if (strcmp(latencyStr, JSON_LATENCY_EXTREMELY_LOW) == 0) {
        uartUSB.write(LOG_MESSAGE_LATENCY_EXTREMELY_LOW, LOG_MESSAGE_LATENCY_EXTREMELY_LOW_LEN);
        *newLatencyLevel = EXTREMELY_LOW_LATENCY;
        return true;
    }
    if (strcmp(latencyStr, JSON_LATENCY_VERY_LOW) == 0) {
        uartUSB.write(LOG_MESSAGE_LATENCY_VERY_LOW, LOG_MESSAGE_LATENCY_VERY_LOW_LEN);
        *newLatencyLevel = VERY_LOW_LATENCY;
        return true;
    }
    if (strcmp(latencyStr, JSON_LATENCY_LOW) == 0) {
        uartUSB.write(LOG_MESSAGE_LATENCY_LOW, LOG_MESSAGE_LATENCY_LOW_LEN);
        *newLatencyLevel = LOW_LATENCY;
        return true;
    }
    if (strcmp(latencyStr, JSON_LATENCY_MEDIUM) == 0) {
        uartUSB.write(LOG_MESSAGE_LATENCY_MEDIUM, LOG_MESSAGE_LATENCY_MEDIUM_LEN);
        *newLatencyLevel = MEDIUM_LATENCY;
        return true;
    }
    if (strcmp(latencyStr, JSON_LATENCY_HIGH) == 0) {
        uartUSB.write(LOG_MESSAGE_LATENCY_HIGH, LOG_MESSAGE_LATENCY_HIGH_LEN);
        *newLatencyLevel = HIGH_LATENCY;
        return true;
    }
    if (strcmp(latencyStr, JSON_LATENCY_VERY_HIGH) == 0) {
        uartUSB.write(LOG_MESSAGE_LATENCY_VERY_HIGH, LOG_MESSAGE_LATENCY_VERY_HIGH_LEN);
        *newLatencyLevel = VERY_HIGH_LATENCY;
        return true;
    }
    if (strcmp(latencyStr, JSON_LATENCY_EXTREMELY_HIGH) == 0) {
        uartUSB.write(LOG_MESSAGE_LATENCY_EXTREMELY_HIGH, LOG_MESSAGE_LATENCY_EXTREMELY_HIGH_LEN);
        *newLatencyLevel = EXTREMELY_HIGH_LATENCY;
        return true;
    }

    return false;
}

bool ExchangingMessages::parseOperationMode(const char* operationModeStr, OperationMode_t * newOperationMode) {

    if (strcmp(operationModeStr, JSON_OPERATION_MODE_NORMAL_OPERATION) == 0) {
        uartUSB.write(LOG_MESSAGE_OPERATION_MODE_NORMAL, LOG_MESSAGE_OPERATION_MODE_NORMAL_LEN);
        *newOperationMode = NORMAL_OPERATION_MODE;
        return true;
    }
    if (strcmp(operationModeStr, JSON_OPERATION_MODE_PURSUIT_OPERATION) == 0) {
        uartUSB.write(LOG_MESSAGE_OPERATION_MODE_PURSUIT, LOG_MESSAGE_OPERATION_MODE_PURSUIT_LEN);
        *newOperationMode = PURSUIT_OPERATION_MODE;
        return true;
    }
    if (strcmp(operationModeStr, JSON_OPERATION_MODE_SILENT_OPERATION) == 0) {
        uartUSB.write(LOG_MESSAGE_OPERATION_MODE_SILENT, LOG_MESSAGE_OPERATION_MODE_SILENT_LEN);
        *newOperationMode = SILENT_OPERATION_MODE;
        return true;
    }

    return false;
}
