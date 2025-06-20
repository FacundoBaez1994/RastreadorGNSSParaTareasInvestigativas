//=====[Libraries]=============================================================

#include "FormattingMessage.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "ExchangingMessages.h"
#include "SavingMessage.h"

//=====[Declaration of private defines]========================================

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
FormattingMessage::FormattingMessage (Tracker * tracker, trackerStatus_t trackerStatus) {
    this->tracker = tracker;
    this->currentStatus = trackerStatus;
}

/** 
* @brief
* 
* @param 
*/
FormattingMessage::~FormattingMessage () {
    this->tracker = NULL;
}

void FormattingMessage::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }


void FormattingMessage::formatMessage (char * formattedMessage, const CellInformation* aCellInfo,
    const GNSSData* GNSSInfo, const std::vector<CellInformation*> &neighborsCellInformation,
    const IMUData_t * imuData, const BatteryData  * batteryStatus) {
    char StringToSendUSB [50];

    switch (this->currentStatus ) {
        ///////////// MN Messages //////////////////////////////
        case TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK:
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),  "Formating MN,GNSS message\r\n");
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->formatMessage(formattedMessage, aCellInfo, GNSSInfo, imuData, batteryStatus);
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),"Switching State to ExchangingMessages"); 
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->tracker->changeState (new ExchangingMessages (this->tracker, this->currentStatus));
            break;
        case TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK:

            snprintf(StringToSendUSB, sizeof(StringToSendUSB),  "Formating MN,MN message:\r\n");
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->formatMessage(formattedMessage, aCellInfo, 
            neighborsCellInformation, imuData, batteryStatus);
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),"Switching State to ExchangingMessages"); 
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->tracker->changeState (new ExchangingMessages (this->tracker, this->currentStatus));
            break;
        ///////////// Lora Messages //////////////////////////////
        case TRACKER_STATUS_GNSS_OBTAIN_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_TRYING_LORA:

            snprintf(StringToSendUSB, sizeof(StringToSendUSB),  "Formating LORA,GNSS message:\r\n");
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->formatLoRaMessage(formattedMessage, aCellInfo, GNSSInfo, imuData, batteryStatus);
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),"Switching State to ExchangingMessages"); 
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->tracker->changeState (new ExchangingMessages (this->tracker, this->currentStatus));
            break;

        case TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTION_TO_MOBILE_NETWORK_UNAVAILABLE_TRYING_LORA:

            snprintf(StringToSendUSB, sizeof(StringToSendUSB),  "Formating LORA,LORA message:\r\n");
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->formatLoRaMessage(formattedMessage, aCellInfo, imuData, batteryStatus);
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),"Switching State to ExchangingMessages"); 
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->tracker->changeState (new ExchangingMessages (this->tracker, this->currentStatus));
            break;


        /////// Saving cases //////////////////////////////
        case TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK_SAVING_MESSAGE:
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),  "Formating MN,GNSS message to be saved\r\n");
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->formatMemoryMessage(formattedMessage, aCellInfo, GNSSInfo, imuData, batteryStatus);
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),"Switching State to SavingMessage"); 
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->tracker->changeState (new SavingMessage (this->tracker));
            break;
        case TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK_SAVING_MESSAGE:
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),  "Formating MN,MN message to be saved:\r\n");
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}

            this->formatMNMNMemoryMessage(formattedMessage, aCellInfo, 
            neighborsCellInformation, imuData, batteryStatus);
            
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),"Switching State to SavingMessage"); 
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->tracker->changeState (new SavingMessage (this->tracker));
            break;
        default:
            return;
    }

 
    return;
}




//=====[Implementations of private methods]==================================
//////////////////////// MN messages /// 
void FormattingMessage::formatMessage(char * formattedMessage, const CellInformation* aCellInfo, 
    const std::vector<CellInformation*> &neighborsCellInformation, const IMUData_t * imuData, const BatteryData  * batteryStatus) {

    static char message[2048];
    static char tempBuffer[250]; // buffer auxiliar para formateo
    size_t currentLen = 0;

    // Encabezado principal del mensaje JSON con los datos de la celda principal
    currentLen = snprintf(message, sizeof(message),
        "{\"Type\":\"MNMN\","
        "\"IMEI\":%lld,"
        "\"MCC\":%d,"
        "\"MNC\":%d,"
        "\"LAC\":\"%X\","
        "\"CID\":\"%X\","
        "\"SLVL\":%.2f,"
        "\"TECH\":%d,"
        "\"REGS\":%d,"
        "\"CHNL\":%d,"
        "\"BAND\":\"%s\","
        "\"TIME\":\"%s\","
        "\"BSTA\":%d,"
        "\"BLVL\":%d,"
        "\"SIMU\":%d,"
        "\"AX\":%.2f,"
        "\"AY\":%.2f,"
        "\"AZ\":%.2f,"
        "\"YAW\":%.2f,"
        "\"ROLL\":%.2f,"
        "\"PTCH\":%.2f",
        aCellInfo->IMEI,               // 0
        aCellInfo->mcc,               // 1
        aCellInfo->mnc,               // 2
        aCellInfo->lac,               // 3
        aCellInfo->cellId,            // 4
        aCellInfo->signalLevel,       // 5
        aCellInfo->accessTechnology,  // 6
        aCellInfo->registrationStatus,// 7
        aCellInfo->channel,           // 8
        aCellInfo->band,              // 9
        aCellInfo->timestamp,              // 10
        batteryStatus->batteryChargeStatus, // 11
        batteryStatus->chargeLevel,          // 12
        imuData->status,                //13
        imuData->acceleration.ax,       // 14
        imuData->acceleration.ay,       //15
        imuData->acceleration.az,       //16
        imuData->angles.yaw,            // 17
        imuData->angles.roll,           // 18
        imuData->angles.pitch           // 19
    );

    // Agregar array de celdas vecinas si existen
    if (!neighborsCellInformation.empty()) {
        currentLen += snprintf(message + currentLen, sizeof(message) - currentLen, ",\"Neighbors\":[");
        
        for (size_t i = 0; i < neighborsCellInformation.size(); ++i) {
            CellInformation* neighbor = neighborsCellInformation[i];
            snprintf(tempBuffer, sizeof(tempBuffer),
                "{\"TECH\":%d,\"MCC\":%d,\"MNC\":%d,\"LAC\":\"%X\",\"CID\":\"%X\",\"SLVL\":%.2f}",
                neighbor->tech,
                neighbor->mcc,
                neighbor->mnc,
                neighbor->lac,
                neighbor->cellId,
                neighbor->signalLevel
            );
            strncat(message, tempBuffer, sizeof(message) - strlen(message) - 1);

            // Si no es el último, agregamos coma
            if (i < neighborsCellInformation.size() - 1) {
                strncat(message, ",", sizeof(message) - strlen(message) - 1);
            }

            //delete neighborsCellInformation[i];
            //neighborsCellInformation[i] = NULL;
        }
        //neighborsCellInformation.clear();
        strncat(message, "]", sizeof(message) - strlen(message) - 1);
    }

    // Cierre del JSON
    strncat(message, "}\n", sizeof(message) - strlen(message) - 1);

    message[sizeof(message) - 1] = '\0';

    //strcpy(formattedMessage, message);
    this->tracker->encodeJWT (message, formattedMessage);

     strcat(formattedMessage, "\n");
}

void FormattingMessage::formatMessage(char * formattedMessage, const CellInformation* aCellInfo,
 const GNSSData* GNSSInfo,  const IMUData_t * imuData, const BatteryData  * batteryStatus) {

    static char message[2048];
    size_t currentLen = 0;

    currentLen = snprintf(message, sizeof(message),
        "{" 
        "\"Type\":\"MNGNSS\","
        "\"IMEI\":%lld,"
        "\"LAT\":%.6f,"
        "\"LONG\":%.6f,"
        "\"HDOP\":%.2f,"
        "\"ALT\":%.2f,"
        "\"COG\":%.2f,"
        "\"SPD\":%.2f,"
        "\"MNC\":%d,"
        "\"MCC\":%d,"
        "\"LAC\":\"%X\","
        "\"CID\":\"%X\","
        "\"SLVL\":%.2f,"
        "\"TECH\":%d,"
        "\"REGS\":%d,"
        "\"CHNL\":%d,"
        "\"BAND\":\"%s\","
        "\"TIME\":\"%s\","
        "\"BSTA\":%d,"
        "\"BLVL\":%d,"
        "\"SIMU\":%d,"
        "\"AX\":%.2f,"
        "\"AY\":%.2f,"
        "\"AZ\":%.2f,"
        "\"YAW\":%.2f,"
        "\"ROLL\":%.2f,"
        "\"PTCH\":%.2f"
        "}",
        aCellInfo->IMEI,                // 0
        GNSSInfo->latitude,            // 1
        GNSSInfo->longitude,           // 2
        GNSSInfo->hdop,                // 3
        GNSSInfo->altitude,            // 4
        GNSSInfo->cog,                 // 5
        GNSSInfo->spkm,                // 6
        aCellInfo->mnc,                // 7
        aCellInfo->mcc,                // 8
        aCellInfo->lac,                // 9
        aCellInfo->cellId,             // 10
        aCellInfo->signalLevel,        // 11
        aCellInfo->accessTechnology,   // 12
        aCellInfo->registrationStatus, // 13
        aCellInfo->channel,            // 14
        aCellInfo->band,               // 15
        GNSSInfo->timestamp,                // 16
        batteryStatus->batteryChargeStatus, // 19
        batteryStatus->chargeLevel,          // 20
        imuData->status,
        imuData->acceleration.ax,
        imuData->acceleration.ay,
        imuData->acceleration.az,
        imuData->angles.yaw,
        imuData->angles.roll,
        imuData->angles.pitch
    );
    message[sizeof(message) - 1] = '\0';

    //strcpy(formattedMessage, message);
    this->tracker->encodeJWT (message, formattedMessage);

    strcat(formattedMessage, "\n");

}

/////////////////////////// LoRa Messages //////////////////////////////
void FormattingMessage::formatLoRaMessage(char * formattedMessage, const CellInformation* aCellInfo, 
  const GNSSData* GNSSInfo, const IMUData_t * imuData, const BatteryData  * batteryStatus) {
  static char message[2048]; 
    snprintf(message, sizeof(message), 
    "LORAGNSS,%lld,%.6f,%.6f,%.2f,%.2f,%.2f,%.2f,%s,%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f", 
        aCellInfo->IMEI,                    // 0 %lld         
        GNSSInfo->latitude,                 // 1 %.6f
        GNSSInfo->longitude,                // 2 %.6f
        GNSSInfo->hdop,                     // 3 %.2f
        GNSSInfo->altitude,                 // 4 %.2f
        GNSSInfo->cog,                      // 5 %.2f
        GNSSInfo->spkm,                     // 6 %.2f
        GNSSInfo->timestamp,                // 7 %s
        batteryStatus->batteryChargeStatus, // 8 %d
        batteryStatus->chargeLevel,         // 9 %d
        imuData->status,                    // 10 %d
        imuData->acceleration.ax,           // 11 %.2f
        imuData->acceleration.ay,           // 12 %.2f
        imuData->acceleration.az,           // 13 %.2f
        imuData->angles.yaw,                // 14 %.2f
        imuData->angles.roll,               // 15 %.2f
        imuData->angles.pitch               // 16 %.2f
            );
    message[sizeof(message) - 1] = '\0';       
    strcpy(formattedMessage, message);
    //formattedMessage[sizeof(formattedMessage) - 1] = '\0';
}


void FormattingMessage::formatLoRaMessage (char * formattedMessage, const CellInformation* aCellInfo, 
 const IMUData_t * imuData, const BatteryData  * batteryStatus) {
  static char message[2048]; 
    snprintf(message, sizeof(message), 
    "LORALORA,%lld,%d,%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f", 
        aCellInfo->IMEI,                     // 0 %lld         
        batteryStatus->batteryChargeStatus, // 1 %d
        batteryStatus->chargeLevel,         // 2 %d
        imuData->status,                    // 3 %d
        imuData->acceleration.ax,           // 4 %.2f
        imuData->acceleration.ay,           // 5 %.2f
        imuData->acceleration.az,           // 6 %.2f
        imuData->angles.yaw,                // 7 %.2f
        imuData->angles.roll,               // 8 %.2f
        imuData->angles.pitch               // 9 %.2f
            );
    message[sizeof(message) - 1] = '\0';       
    strcpy(formattedMessage, message);
    //formattedMessage[sizeof(formattedMessage) - 1] = '\0';
}






/////////////////////////// saving cases //////////////////////////////
//// MNMN for save on memory
void FormattingMessage::formatMNMNMemoryMessage(char * formattedMessage, const CellInformation* aCellInfo, 
    const std::vector<CellInformation*> &neighborsCellInformation, const IMUData_t * imuData, const BatteryData  * batteryStatus) {

    static char message[2048];
    static char tempBuffer[250]; // buffer auxiliar para formateo
    size_t currentLen = 0;


    char StringToSendUSB [100];
    
    snprintf(StringToSendUSB, sizeof(StringToSendUSB),  "MN,MN save:\r\n");
    uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only}

    // Encabezado principal del mensaje JSON con los datos de la celda principal
    currentLen = snprintf(message, sizeof(message),
        "MNMN,%d,%d,%X,%X,%.2f,%d,%d,%d,%s,%s,%d,%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
        aCellInfo->mcc,               // 1 %d
        aCellInfo->mnc,               // 2 %d
        aCellInfo->lac,               // 3 %X
        aCellInfo->cellId,            // 4 %X
        aCellInfo->signalLevel,       // 5 %.2f
        aCellInfo->accessTechnology,  // 6 %d
        aCellInfo->registrationStatus,// 7 %d
        aCellInfo->channel,           // 8 %d
        aCellInfo->band,              // 9 %s
        aCellInfo->timestamp,              // 10 %s
        batteryStatus->batteryChargeStatus, // 11 %d
        batteryStatus->chargeLevel,          // 12 %d
        imuData->status,                //13 %d
        imuData->acceleration.ax,       // 14 %.2f
        imuData->acceleration.ay,       //15 %.2f
        imuData->acceleration.az,       //16 %.2f
        imuData->angles.yaw,            // 17 %.2f
        imuData->angles.roll,           // 18 %.2f
        imuData->angles.pitch           // 19 %.2f
    );
    // inertialData,  //13 status, 13 ax, 14 ay, 15 az, 16 yaw, 17 roll, 18 pitch

    // Agregar array de celdas vecinas si existen
    if (!neighborsCellInformation.empty()) {
        for (size_t i = 0; i < neighborsCellInformation.size(); ++i) {
            CellInformation* neighbor = neighborsCellInformation[i];
            snprintf(tempBuffer, sizeof(tempBuffer),
                "|%d,%d,%d,%X,%X,%.2f",
                neighbor->tech,
                neighbor->mcc,
                neighbor->mnc,
                neighbor->lac,
                neighbor->cellId,
                neighbor->signalLevel
            );
            strncat(message, tempBuffer, sizeof(message) - strlen(message) - 1);

            uartUSB.write ( tempBuffer,  strlen (tempBuffer) );  // debug only}
        }
    }
    message[sizeof(message) - 1] = '\0';
    strcpy(formattedMessage, message);
    //formattedMessage[sizeof(formattedMessage) - 1] = '\0';
}


//// MNGNSS for save on memory
void FormattingMessage::formatMemoryMessage(char * formattedMessage, const CellInformation* aCellInfo,
 const GNSSData* GNSSInfo,  const IMUData_t * imuData, const BatteryData  * batteryStatus) {
    static char message[2048]; 
    snprintf(message, sizeof(message), 
    "MNGNSS,%.6f,%.6f,%.2f,%.2f,%.2f,%.2f,%d,%d,%X,%X,%.2f,%d,%d,%d,%s,%s,%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f", 
        GNSSInfo->latitude,                 // 1 %.6f
        GNSSInfo->longitude,                // 2 %.6f
        GNSSInfo->hdop,                     // 3 %.2f
        GNSSInfo->altitude,                 // 4 %.2f
        GNSSInfo->cog,                      // 5 %.2f
        GNSSInfo->spkm,                     // 6 %.2f
        aCellInfo->mnc,                     // 7 %d
        aCellInfo->mcc,                     // 8 %d
        aCellInfo->lac,                     // 9 %X
        aCellInfo->cellId,                  // 10 %X
        aCellInfo->signalLevel,             // 11 %.2f
        aCellInfo->accessTechnology,        // 12 %d
        aCellInfo->registrationStatus,      // 13 %d
        aCellInfo->channel,                 // 14 %d
        aCellInfo->band,                    // 15 %s
        GNSSInfo->timestamp,                // 16 %s
        batteryStatus->batteryChargeStatus, // 17 %d
        batteryStatus->chargeLevel,         // 18 %d
        imuData->status,                    // 19 %d
        imuData->acceleration.ax,           // 20 %.2f
        imuData->acceleration.ay,           // %.2f
        imuData->acceleration.az,           // %.2f
        imuData->angles.yaw,                // %.2f
        imuData->angles.roll,               // %.2f
        imuData->angles.pitch               // %.2f
            );
    message[sizeof(message) - 1] = '\0';       
    strcpy(formattedMessage, message);
    //formattedMessage[sizeof(formattedMessage) - 1] = '\0';
}


