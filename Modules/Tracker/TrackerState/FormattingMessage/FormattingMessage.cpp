//=====[Libraries]=============================================================

#include "FormattingMessage.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "ExchangingMessages.h"

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


void FormattingMessage::formatMessage (char * formattedMessage, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    IMUData_t * imuData, BatteryData  * batteryStatus) {
    /// Cifrado iria aca tambien..

    if (this->currentStatus == TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK) {
            char StringToSendUSB [50] = "Formating MN,GNSS message\r\n";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->formatMessage(formattedMessage, aCellInfo, GNSSInfo, imuData, batteryStatus);
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),"Switching State to ExchangingMessages"); 
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->tracker->changeState (new ExchangingMessages (this->tracker, 
            TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK));
            return;
    }
    if (this->currentStatus == TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK) {
            char StringToSendUSB [50] = "Formating MN,MN message:\r\n";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->formatMessage(formattedMessage, aCellInfo, 
            neighborsCellInformation, imuData, batteryStatus);
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),"Switching State to ExchangingMessages"); 
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->tracker->changeState (new ExchangingMessages (this->tracker, 
            TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK));
            return;
        }
        
    return;
}




//=====[Implementations of private methods]==================================
void FormattingMessage::formatMessage(char * formattedMessage, CellInformation* aCellInfo, 
    std::vector<CellInformation*> &neighborsCellInformation, IMUData_t * imuData, BatteryData  * batteryStatus) {

    static char message[2024];
    static char tempBuffer[250]; // buffer auxiliar para formateo
    size_t currentLen = 0;

    // Encabezado principal del mensaje JSON con los datos de la celda principal
    currentLen = snprintf(message, sizeof(message),
        "{\"Type\":\"MNMN\","
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
    // inertialData,  //13 status, 13 ax, 14 ay, 15 az, 16 yaw, 17 roll, 18 pitch

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
    strncat(message, "}", sizeof(message) - strlen(message) - 1);

    message[sizeof(message) - 1] = '\0';

    //strcpy(formattedMessage, message);
    this->tracker->encodeJWT (message, formattedMessage);

    strcat(formattedMessage, "\n");
}

/*
void FormattingMessage::formatMessage(char * formattedMessage, CellInformation* aCellInfo, 
std::vector<CellInformation*> &neighborsCellInformation, char * inertialData, BatteryData  * batteryStatus) {
    static char message[500];
    char neighbors[50];
    char closeJWTString [3] = ",}"; 
    int lac;
    int idCell;
    int tech;
    int mcc;
    int mnc;
    float prx;
    snprintf(message, sizeof(message), 
            "{\"Type\":\"MNMN\",\"MCC\":%d,\"MNC\":%d,\"LAC\":%X,\"CID\":%X,\"SLVL\":%.2f,\"TECH\":%d,\"REGS\":%d,\"CHNL\":%d,\"BAND\":\"%s\",\"DATE\":\"%s\",\"TIME\":\"%s\",\"BSTA\":%d,\"BLVL\":%d", 
            aCellInfo->mcc,  // 1
            aCellInfo->mnc,  // 2
            aCellInfo->lac,  // 3
            aCellInfo->cellId,  // 4
            aCellInfo->signalLevel, // 5
            aCellInfo->accessTechnology, // 6
            aCellInfo->registrationStatus, // 7
            aCellInfo->channel, // 8
            aCellInfo->band, // 9
            aCellInfo->date, // 10  // deberia ser un solo campo
            aCellInfo->time, // 11  // deberia ser un solo campo
            batteryStatus->batteryChargeStatus, //19
            batteryStatus->chargeLevel //20
            );   // inertialData,  //12 temp, 13 ax, 14 ay, 15 az, 16 yaw, 17 roll, 18 pitch
    snprintf(neighbors, sizeof(neighbors),"size of vector %d", neighborsCellInformation.size()); 
    uartUSB.write (neighbors , strlen (neighbors ));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only 
    if ( neighborsCellInformation.size() == 0) {
        strncat (message, closeJWTString, sizeof(message) - strlen(message) - 1); // completar aca CHAT GPT
        strcpy (formattedMessage, message);
        return;
    }
       
    for (size_t i = 0; i < neighborsCellInformation.size(); ++i) {
        tech = neighborsCellInformation[i]->tech;
        mcc = neighborsCellInformation[i]->mcc;
        mnc = neighborsCellInformation[i]->mnc;
        lac = neighborsCellInformation[i]->lac;
        idCell = neighborsCellInformation[i]->cellId;
        prx = neighborsCellInformation[i]->signalLevel;
        snprintf(neighbors, sizeof(neighbors),",%dG,%d,%d,%X,%X,%.2f",tech,mcc,mnc,lac,idCell,prx); 
        uartUSB.write (neighbors , strlen (neighbors ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        // Concatenar el mensaje de la celda vecina al mensaje principal
        strncat(message, neighbors, sizeof(message) - strlen(message) - 1);
        delete neighborsCellInformation[i];
        neighborsCellInformation[i] = NULL;
    }
    strncat (message, closeJWTString, sizeof(message) - strlen(message) - 1); // completar aca CHAT GPT
    strcpy (formattedMessage, message);
    neighborsCellInformation.clear();
}
*/

void FormattingMessage::formatMessage(char * formattedMessage, CellInformation* aCellInfo,
 GNSSData* GNSSInfo,  IMUData_t * imuData, BatteryData  * batteryStatus) {

    static char message[512];
    size_t currentLen = 0;

    currentLen = snprintf(message, sizeof(message),
        "{"
        "\"Type\":\"MNGNSS\","
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

/*
void FormattingMessage::formatMessage(char * formattedMessage, CellInformation* aCellInfo,
 GNSSData* GNSSInfo,  char * inertialData, BatteryData  * batteryStatus) {
    static char message[200]; 
    snprintf(message, sizeof(message), 
             "MN,GNSS,%.6f,%.6f,%.2f,%.2f,%.2f,%.2f,%d,%d,%X,%X,%.2f,%d,%d,%d,%s,%s,%s,%s,%d,%d\r\n", 
            GNSSInfo->latitude, // 1
            GNSSInfo->longitude,  // 2
            GNSSInfo->hdop,  // 3
            GNSSInfo->altitude,  // 4
            GNSSInfo->cog,  // 5
            GNSSInfo->spkm,  // 6
            aCellInfo->mnc,  // 7
            aCellInfo->mcc,  // 8
            aCellInfo->lac,  // 9
            aCellInfo->cellId,  // 10
            aCellInfo->signalLevel, //11
            aCellInfo->accessTechnology, //12
            aCellInfo->registrationStatus, // 13
            aCellInfo->channel, // 14
            aCellInfo->band, // 15
            GNSSInfo->date, // 16
            GNSSInfo->utc, // 17
            inertialData, //18 temp, 19 ax, 20 ay, 21 az, 22 yaw, 23 roll, 24 pitch
            batteryStatus->batteryChargeStatus, // 25
            batteryStatus->chargeLevel // 26
            );
    strcpy (formattedMessage, message);
}
*/

