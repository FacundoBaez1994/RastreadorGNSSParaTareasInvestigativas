//=====[Libraries]=============================================================

#include "LoadingMessage.h"
#include "SavingMessage.h"
#include "Tracker.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "GoingToSleep.h"
#include "FormattingMessage.h"

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
LoadingMessage::LoadingMessage  (Tracker * tracker) {
    this->tracker = tracker;
    //this->poppedString = new char [this->sizeOfPoppedString];
}

/** 
* @brief
* 
* @param 
*/
LoadingMessage::~LoadingMessage  () {
    this->tracker = nullptr;
    //delete [] this->poppedString;
    //this->poppedString = nullptr;
}

void LoadingMessage::loadMessage (EEPROMManager * memory, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    IMUData_t * imuData,  BatteryData  * batteryStatus) {
    static char  poppedString [2048] = {0};
    char  log [50];
    static bool decryptionProcessFinished = false;
    static bool popProcessFinished = false;
    static bool init = false;

    if (init == false) {
        memset(poppedString, 0, sizeof(poppedString));
        init = true;
    }
    
    
    EEPROMStatus state;
    if (popProcessFinished  == false) {
        //state = memory->popStringFromEEPROM( this->poppedString, this->sizeOfPoppedString);
        state = memory->popStringFromEEPROM( poppedString, sizeof (poppedString));
        if (state == EEPROMStatus::POPPEDSTRINGOK) {
            snprintf(log, sizeof(log), "popped string From Memory\n\r");
            uartUSB.write(log, strlen(log));
            uartUSB.write(poppedString, strlen(poppedString));
            uartUSB.write("\n\r", strlen("\n\r"));

            popProcessFinished = true;
        } else if (state ==  EEPROMStatus::EMPTY) {
            snprintf(log, sizeof(log), "EEPROM empty\n\r");
            uartUSB.write(log, strlen(log));
            decryptionProcessFinished = false;
            popProcessFinished = false;
            init = false;
            this->tracker->changeState  (new GoingToSleep (this->tracker));
            return;
        }
    } else {
        if (this->tracker->decryptMessage(poppedString, sizeof (poppedString)) == true) {
            snprintf(log, sizeof(log), "\n\rultimo string descifrado:\n\r");
            uartUSB.write(log, strlen(log));
            uartUSB.write(poppedString, strlen(poppedString));
            uartUSB.write("\n\r", strlen("\n\r"));
            trackerStatus_t currentStatus;
            currentStatus = this->parseDecryptedMessage(poppedString, aCellInfo, GNSSInfo, 
            neighborsCellInformation, imuData, batteryStatus);
            if ( currentStatus == TRACKER_STATUS_PARSE_ERROR) {
                snprintf(log, sizeof(log), "\n\rparse error:\n\r");
                uartUSB.write(log, strlen(log));
                decryptionProcessFinished = false;
                popProcessFinished = false;
                init = false;
                this->tracker->changeState  (new GoingToSleep (this->tracker));
                return;
            }
            decryptionProcessFinished = false;
            popProcessFinished = false;
            init = false;

            //this->tracker->changeState  (new SavingMessage (this->tracker));
            //this->tracker->changeState  (new GoingToSleep (this->tracker));
            this->tracker->changeState  (new FormattingMessage (this->tracker, currentStatus));
            return;
        }
    }
    return;
 }

//=====[Implementations of private methods]==================================

trackerStatus_t LoadingMessage::parseDecryptedMessage(const char* decryptedString, 
    CellInformation* aCellInfo,
    GNSSData* GNSSInfo,
    std::vector<CellInformation*>& neighborsCellInformation,
    IMUData_t* imuData,
    BatteryData* batteryStatus) {

    if (strncmp(decryptedString, "MNMN", 4) == 0) {
        parseMNMN(decryptedString, aCellInfo, neighborsCellInformation, imuData, batteryStatus);
        return TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK;
    } else if (strncmp(decryptedString, "MNGNSS", 6) == 0) {
        parseMNGNSS(decryptedString, aCellInfo, GNSSInfo, imuData, batteryStatus);
        return TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK; 
    }
    return TRACKER_STATUS_PARSE_ERROR;
}

void LoadingMessage::parseMNGNSS(const char* message,
    CellInformation* aCellInfo,
    GNSSData* GNSSInfo,
    IMUData_t* imuData,
    BatteryData* batteryStatus) {
    
    static bool init = false;
    char * buffer;
    size_t sizeOfBuffer = 2048;

    if (init == false) {
        init = true;
        buffer = new char [sizeOfBuffer];
    }

    strncpy(buffer, message, sizeOfBuffer);
    buffer[sizeOfBuffer -1] = '\0';

    char* token = strtok(buffer, ",");
    if (!token || strcmp(token, "MNGNSS") != 0) {
        init = false;
        delete [] buffer;
        buffer = nullptr;
        return;
    }
     

    token = strtok(NULL, ",");
    GNSSInfo->latitude = atof(token);
    token = strtok(NULL, ",");
    GNSSInfo->longitude = atof(token);
    token = strtok(NULL, ",");
    GNSSInfo->hdop = atof(token);
    token = strtok(NULL, ",");
    GNSSInfo->altitude = atof(token);
    token = strtok(NULL, ",");
    GNSSInfo->cog = atof(token);
    token = strtok(NULL, ",");
    GNSSInfo->spkm = atof(token);

    token = strtok(NULL, ",");
    aCellInfo->mnc = atoi(token);
    token = strtok(NULL, ",");
    aCellInfo->mcc = atoi(token);
    token = strtok(NULL, ",");
    sscanf(token, "%X", &aCellInfo->lac);
    token = strtok(NULL, ",");
    sscanf(token, "%X", &aCellInfo->cellId);
    token = strtok(NULL, ",");
    aCellInfo->signalLevel = atof(token);
    token = strtok(NULL, ",");
    aCellInfo->accessTechnology = atoi(token);
    token = strtok(NULL, ",");
    aCellInfo->registrationStatus = atoi(token);
    token = strtok(NULL, ",");
    aCellInfo->channel = atoi(token);
    token = strtok(NULL, ",");
    aCellInfo->band = token;
    strcpy (aCellInfo->band, token);
    //strncpy(aCellInfo->band, token, sizeof(aCellInfo->band) - 1);
    //aCellInfo->band[sizeof(aCellInfo->band) - 1] = '\0';
    token = strtok(NULL, ",");
    strcpy (GNSSInfo->timestamp, token);
    //strncpy(GNSSInfo->timestamp, token, sizeof(GNSSInfo->timestamp) - 1);
    //GNSSInfo->timestamp[sizeof(GNSSInfo->timestamp) - 1] = '\0';

    token = strtok(NULL, ",");
    batteryStatus->batteryChargeStatus = atoi(token);
    token = strtok(NULL, ",");
    batteryStatus->chargeLevel = atoi(token);

    token = strtok(NULL, ",");
    imuData->status = atoi(token);
    token = strtok(NULL, ",");
    imuData->acceleration.ax = atof(token);
    token = strtok(NULL, ",");
    imuData->acceleration.ay = atof(token);
    token = strtok(NULL, ",");
    imuData->acceleration.az = atof(token);
    token = strtok(NULL, ",");
    imuData->angles.yaw = atof(token);
    token = strtok(NULL, ",");
    imuData->angles.roll = atof(token);
    token = strtok(NULL, ",");
    imuData->angles.pitch = atof(token);

    init = false;
    delete [] buffer;
    buffer = nullptr;
}


void LoadingMessage::parseMNMN(const char* message,
    CellInformation* aCellInfo,
    std::vector<CellInformation*>& neighborsCellInformation,
    IMUData_t* imuData,
    BatteryData* batteryStatus) {

    // Limpiar celdas viejas
    for (auto cell : neighborsCellInformation) {
        delete cell;
    }
    neighborsCellInformation.clear();

    if (!message || strncmp(message, "MNMN,", 5) != 0) return;

    // Hacer una copia del mensaje para procesar
    static bool init = false;
    char * buffer;
    char * neighborsBuffer;
    size_t sizeOfBuffer = 2048;

    if (init == false) {
        init = true;
        buffer = new char [sizeOfBuffer];
        neighborsBuffer = new char [sizeOfBuffer];
        strncpy(buffer, message, sizeOfBuffer);
        buffer[sizeOfBuffer-1] = '\0';

    }


    // Procesar parte principal
    char* mainPart = strtok(buffer, "|");
    if (!mainPart) {
        delete [] buffer;
        buffer = nullptr;
        delete [] neighborsBuffer;
        neighborsBuffer = nullptr;
        init = false;
        return;
    } 

    // Extraer campos principales
    char* token = strtok(mainPart, ",");
    int fieldIndex = 0;

    while (token != nullptr) {
        switch (fieldIndex) {
            case 1: aCellInfo->mcc = atoi(token); break;
            case 2: aCellInfo->mnc = atoi(token); break;
            case 3: aCellInfo->lac = strtol(token, nullptr, 16); break;
            case 4: aCellInfo->cellId = strtol(token, nullptr, 16); break;
            case 5: aCellInfo->signalLevel = atof(token); break;
            case 6: aCellInfo->accessTechnology = atoi(token); break;
            case 7: aCellInfo->registrationStatus = atoi(token); break;
            case 8: aCellInfo->channel = atoi(token); break;
            case 9: 
                strcpy (aCellInfo->band, token);
                //strncpy(aCellInfo->band, token, sizeof(aCellInfo->band));
                //aCellInfo->band[sizeof(aCellInfo->band) - 1] = '\0';
                break;
            case 10:
            strcpy (aCellInfo->timestamp, token);
                //strncpy(aCellInfo->timestamp, token, sizeof(aCellInfo->timestamp));
                //aCellInfo->timestamp[sizeof(aCellInfo->timestamp) - 1] = '\0';
                break;
            case 11: batteryStatus->batteryChargeStatus = atoi(token); break;
            case 12: batteryStatus->chargeLevel = atoi(token); break;
            case 13: imuData->status = atoi(token); break;
            case 14: imuData->acceleration.ax = atof(token); break;
            case 15: imuData->acceleration.ay = atof(token); break;
            case 16: imuData->acceleration.az = atof(token); break;
            case 17: imuData->angles.yaw = atof(token); break;
            case 18: imuData->angles.roll = atof(token); break;
            case 19: imuData->angles.pitch = atof(token); break;
        }
        token = strtok(nullptr, ",");
        fieldIndex++;
    }

    // Procesar celdas vecinas
    //  trabajar con una copia nueva - strtok modifica el buffer
    //char neighborsBuffer[2048];
    strncpy(neighborsBuffer, message, sizeOfBuffer);
    neighborsBuffer[sizeOfBuffer - 1] = '\0';

    // Avanzar hasta la primera parte de vecinos
    strtok(neighborsBuffer, "|"); // Saltar la parte principal
    char* neighborPart = strtok(nullptr, "|");

    char log[50];
    while (neighborPart != nullptr) {
        snprintf(log, sizeof(log), "Celda Vecina Recargada: %s\r\n", neighborPart);
        uartUSB.write(log, strlen(log));

        CellInformation* neighbor = new CellInformation();
        int tech, mcc, mnc;
        unsigned int lac, cellId;
        float signal;

        if (sscanf(neighborPart, "%d,%d,%d,%X,%X,%f", &tech, &mcc, &mnc, &lac, &cellId, &signal) == 6) {
            neighbor->tech = tech;
            neighbor->mcc = mcc;
            neighbor->mnc = mnc;
            neighbor->lac = lac;
            neighbor->cellId = cellId;
            neighbor->signalLevel = signal;

            neighborsCellInformation.push_back(neighbor);
            uartUSB.write("Cell added to vector\r\n", strlen("Cell added to vector\r\n"));
            snprintf(log, sizeof(log), "Vector size: %zu\r\n", neighborsCellInformation.size());
            uartUSB.write(log, strlen(log));
        } else {
            snprintf(log, sizeof(log), "Error parsing neighbor: %s\r\n", neighborPart);
            uartUSB.write(log, strlen(log));
            delete neighbor;
            neighbor = nullptr;
        }

        neighborPart = strtok(nullptr, "|");
    }
    delete [] buffer;
    buffer = nullptr;
    delete [] neighborsBuffer;
    neighborsBuffer = nullptr;
    init = false;
}

