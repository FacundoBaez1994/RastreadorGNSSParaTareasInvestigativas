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

//=====[Implementations of public methods]===================================
LoadingMessage::LoadingMessage  (Tracker * tracker) {
    this->tracker = tracker;
    //this->poppedString = new char [this->sizeOfPoppedString];
}

LoadingMessage::~LoadingMessage  () {
    this->tracker = nullptr;
    //delete [] this->poppedString;
    //this->poppedString = nullptr;
}

void LoadingMessage::loadMessage (EEPROMManager * memory, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    IMUData_t * imuData, std::vector<IMUData_t*> &IMUDataSamples,  BatteryData  * batteryStatus) {
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
            neighborsCellInformation, imuData, IMUDataSamples, batteryStatus);
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
    std::vector<IMUData_t*>& IMUDataSamples,
    BatteryData* batteryStatus) {

    if (strncmp(decryptedString, "MNMN", 4) == 0) {
        uartUSB.write("parseMNMN IN\n\r", strlen("parseMNMN IN\n\r")); // ELIMINIAR
        parseMNMN(decryptedString, aCellInfo, neighborsCellInformation, imuData, batteryStatus);
        return TRACKER_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK;
    } else if (strncmp(decryptedString, "MNGNSS", 6) == 0) {
        parseMNGNSS(decryptedString, aCellInfo, GNSSInfo, imuData, batteryStatus);
        return TRACKER_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK; 
    }  else if (strncmp(decryptedString, "GNSS", 4) == 0) {
        parseGNSS(decryptedString, GNSSInfo, imuData, batteryStatus);
        return TRACKER_STATUS_GNSS_LOADED_MESSAGE;
    } else if (strncmp(decryptedString, "IMU", 3) == 0) {
        parseIMU(decryptedString, imuData, IMUDataSamples, batteryStatus);
        return TRACKER_STATUS_IMU_LOADED_MESSAGE;
    }
    return TRACKER_STATUS_PARSE_ERROR;
}

void LoadingMessage::parseMNGNSS(const char* message,
    CellInformation* aCellInfo,
    GNSSData* GNSSInfo,
    IMUData_t* imuData,
    BatteryData* batteryStatus) {
    
    char * buffer;
    size_t sizeOfBuffer = 1024;

    buffer = new char [sizeOfBuffer];

    if (!buffer) {
        delete [] buffer;
        buffer = nullptr;
        return;
    }

    strncpy(buffer, message, sizeOfBuffer);
    buffer[sizeOfBuffer -1] = '\0';

    char* token = strtok(buffer, ",");
    if (!token || strcmp(token, "MNGNSS") != 0) {
        delete [] buffer;
        buffer = nullptr;
        return;
    }
     
    token = strtok(NULL, ","); 
    char event [25];
    strcpy (event, token);
    this->tracker->setMovementEvent (event);
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

    delete [] buffer;
    buffer = nullptr;
}


void LoadingMessage::parseMNMN(const char* message,
    CellInformation* aCellInfo,
    std::vector<CellInformation*>& neighborsCellInformation,
    IMUData_t* imuData,
    BatteryData* batteryStatus) {
    
    for (auto cell : neighborsCellInformation) {
        delete cell;
        cell = nullptr;
    }
    neighborsCellInformation.clear();

    if (!message || strncmp(message, "MNMN,", 5) != 0) return;

    size_t sizeOfBuffer = 1024;
    char* buffer = new char[sizeOfBuffer];
    char* neighborsBuffer = new char[sizeOfBuffer];

    if (!buffer || !neighborsBuffer) {
        delete[] buffer;
        delete[] neighborsBuffer;
        buffer = nullptr;
        neighborsBuffer = nullptr;
        return;
    }

    strncpy(buffer, message, sizeOfBuffer);
    buffer[sizeOfBuffer - 1] = '\0';

    char* mainPart = strtok(buffer, "|");
    if (!mainPart) {
        delete[] buffer;
        delete[] neighborsBuffer;
        return;
    }

    char* token = strtok(mainPart, ",");
    int fieldIndex = 0;
    while (token != nullptr) {
        switch (fieldIndex) {
            case 1:
                char event [25];
                strcpy (event, token);
                this->tracker->setMovementEvent (event);
                break;
            case 2: 
                aCellInfo->mcc = atoi(token); 
                break;
            case 3: 
                aCellInfo->mnc = atoi(token); 
                break;
            case 4: 
                aCellInfo->lac = strtol(token, nullptr, 16); 
                break;
            case 5: 
                aCellInfo->cellId = strtol(token, nullptr, 16); 
                break;
            case 6: 
                aCellInfo->signalLevel = atof(token); 
                break;
            case 7: 
                aCellInfo->accessTechnology = atoi(token); 
                break;
            case 8: 
                aCellInfo->registrationStatus = atoi(token); 
                break;
            case 9: 
                aCellInfo->channel = atoi(token);
                break;
            case 10: 
                strcpy(aCellInfo->band, token); 
                break;
            case 11: 
                strcpy(aCellInfo->timestamp, token); 
                break;
            case 12: 
                batteryStatus->batteryChargeStatus = atoi(token); 
                break;
            case 13: 
                batteryStatus->chargeLevel = atoi(token); 
                break;
            case 14: 
                imuData->status = atoi(token); 
                break;
            case 15: 
                imuData->acceleration.ax = atof(token); 
                break;
            case 16: 
                imuData->acceleration.ay = atof(token); 
                break;
            case 17: 
                imuData->acceleration.az = atof(token); 
                break;
            case 18: 
                imuData->angles.yaw = atof(token); 
                break;
            case 19: 
                imuData->angles.roll = atof(token); 
                break;
            case 20: 
                imuData->angles.pitch = atof(token); 
                break;
        }
        token = strtok(nullptr, ",");
        fieldIndex++;
    }

    strncpy(neighborsBuffer, message, sizeOfBuffer);
    neighborsBuffer[sizeOfBuffer - 1] = '\0';
    strtok(neighborsBuffer, "|"); 
    char* neighborPart = strtok(nullptr, "|");

    while (neighborPart != nullptr) {
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
        } else {
            delete neighbor;
            neighbor = nullptr;
        }

        neighborPart = strtok(nullptr, "|");
    }

    delete[] buffer;
    buffer = nullptr;
    delete[] neighborsBuffer;
    neighborsBuffer = nullptr;
}


void LoadingMessage::parseGNSS(const char* message,
    GNSSData* GNSSInfo,
    IMUData_t* imuData,
    BatteryData* batteryStatus) {

    size_t sizeOfBuffer = 1024;
    char* buffer = new char[sizeOfBuffer];

    if (!buffer) {
        delete[] buffer;
        buffer = nullptr;
        return;
    }

    strncpy(buffer, message, sizeOfBuffer);
    buffer[sizeOfBuffer - 1] = '\0';

    char* token = strtok(buffer, ",");
    if (!token || strcmp(token, "GNSS") != 0) {
        delete[] buffer;
        buffer = nullptr;
        return;
    }

    token = strtok(NULL, ","); 
    char event [25];
    strcpy (event, token);
    this->tracker->setMovementEvent (event);
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
    strcpy(GNSSInfo->timestamp, token);
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

    delete[] buffer;
    buffer = nullptr;
}


void LoadingMessage::parseIMU(const char* message,
    IMUData_t* imuData,
    std::vector<IMUData_t*>& IMUDataSamples,
    BatteryData* batteryStatus) {

    char log[100];

    for (auto sample : IMUDataSamples) {
        delete sample;
        sample = nullptr;
    }
    IMUDataSamples.clear();

    // Copia del mensaje original
    char* fullCopy = new char[1024];
    strncpy(fullCopy, message, 1024);
    fullCopy[1023] = '\0';

    // Separar la parte principal del resto usando strchr
    char* separator = strchr(fullCopy, '|');
    if (!separator) {
        delete[] fullCopy;
        fullCopy = nullptr;
        return;
    }

    *separator = '\0';
    char* mainPart = fullCopy;
    char* sampleStart = separator + 1;

    char* token = strtok(mainPart, ",");
    int index = 0;
    while (token != nullptr) {
        switch (index) {
            case 1: 
                char event [25];
                strcpy (event, token);
                this->tracker->setMovementEvent (event);
                break;
            case 2: 
                strcpy(imuData->timestamp, token); 
                break;
            case 3: 
                imuData->timeBetweenSamples = atoi(token); 
                break;
            case 4: 
                batteryStatus->batteryChargeStatus = atoi(token);
                break;
            case 5: 
                batteryStatus->chargeLevel = atoi(token); 
                break;
            case 6: 
                imuData->status = atoi(token);
                 break;
            case 7: 
                imuData->acceleration.ax = atof(token); 
                break;
            case 8: 
                imuData->acceleration.ay = atof(token); 
                break;
            case 9: 
                imuData->acceleration.az = atof(token); 
                break;
            case 10: 
                imuData->angles.yaw = atof(token); 
                break;
            case 11: 
                imuData->angles.roll = atof(token); 
                break;
            case 12: 
                imuData->angles.pitch = atof(token); 
                break;
        }
        token = strtok(nullptr, ",");
        index++;
    }

    char* sampleToken = strtok(sampleStart, "|");
    while (sampleToken != nullptr) {
        IMUData_t* sample = new IMUData_t();
        if (sscanf(sampleToken, "%f,%f,%f,%f,%f,%f",
            &sample->acceleration.ax,
            &sample->acceleration.ay,
            &sample->acceleration.az,
            &sample->angles.yaw,
            &sample->angles.roll,
            &sample->angles.pitch) == 6) {
            
            IMUDataSamples.push_back(sample);

            snprintf(log, sizeof(log), "Muestra IMU añadida: %.2f,%.2f,%.2f,%.2f,%.2f,%.2f\r\n",
                sample->acceleration.ax, sample->acceleration.ay, sample->acceleration.az,
                sample->angles.yaw, sample->angles.roll, sample->angles.pitch);
            uartUSB.write(log, strlen(log));
        } else {
            delete sample;
            sample = nullptr;
        }
        sampleToken = strtok(nullptr, "|");
    }

    snprintf(log, sizeof(log), "Total muestras IMU: %zu\r\n", IMUDataSamples.size());
    uartUSB.write(log, strlen(log));

    delete[] fullCopy;
    fullCopy = nullptr;
}

