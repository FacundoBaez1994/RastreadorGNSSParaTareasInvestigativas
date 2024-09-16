//=====[Libraries]=============================================================

#include "ConnectedState.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

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
ConnectedState::ConnectedState () {
    this->mobileNetworkModule = NULL;
    this->enableTransmission = false;
}


/** 
* @brief
* 
* @param 
*/
ConnectedState::ConnectedState (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->enableTransmission = false;
}


/** 
* @brief 
* 
* 
* @returns 
*/
ConnectedState::~ConnectedState () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
void ConnectedState::enableConnection () {
    return;
}

CellularConnectionStatus_t ConnectedState::connect (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, CellInformation * currentCellInformation) {
        return CELLULAR_CONNECTION_STATUS_CONNECTED_TO_NETWORK;
    }

/** 
* @brief 
* 
* 
* @returns 
*/
bool ConnectedState::retrivNeighborCellsInformation(ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors) {
    
    static bool cellDataRetrived = false;
    static bool readyToSend = true;
    
    char StringToBeRead[150];
    char ExpectedResponse[15] = "OK";
    char buffer [60];
    
    char StringToSend[15] = "AT+QOPS";
    char StringToSendUSB[40] = "Retriving Neighboor Cells Data";

    static int mnc;
    static int mcc;

    char tech[5];
    int idCell;
    int lac;
    float prx;

    if (readyToSend == true && cellDataRetrived == false) {
        uartUSB.write(StringToSendUSB, strlen(StringToSendUSB));  // debug only
        uartUSB.write("\r\n", 3);  // debug only
        handler->sendATCommand(StringToSend);
        uartUSB.write(StringToSend, strlen(StringToSend));  // debug only
        uartUSB.write("\r\n", 3);  // debug only
        readyToSend = false;
    }
    if (handler->readATResponse(StringToBeRead) == true) {
        uartUSB.write(StringToBeRead, strlen(StringToBeRead));  // debug only
        uartUSB.write("\r\n", 3);  // debug only

        if (this->retrivOperatorsCodes(StringToBeRead, &mcc, &mnc) == false) {
            if (this->retrivCellData(StringToBeRead, tech, &idCell, &lac, &prx)) {
                cellDataRetrived = true;
                CellInformation* cellInfoTemp = new CellInformation;
                cellInfoTemp->tech = new char[5];
                cellInfoTemp->signalLevel = prx;
                cellInfoTemp->mcc = mcc;
                cellInfoTemp->mnc = mnc;
                strcpy(cellInfoTemp->tech, tech);
                cellInfoTemp->lac =  lac;
                cellInfoTemp->cellId = idCell;
                
                neighborsCellInformation.push_back(cellInfoTemp);
                uartUSB.write("Cell added to vector\r\n", strlen("Cell added to vector\r\n"));
                snprintf(buffer, sizeof(buffer), "Vector size: %d\r\n", neighborsCellInformation.size());
                uartUSB.write(buffer, strlen(buffer));
            }
        }
        if (strcmp (StringToBeRead, ExpectedResponse)  == 0) {
            return true;
        }
    }


    if (refreshTime->read()) {
        readyToSend = true;
    }

    return false;
}


//=====[Implementations of private functions]==================================
bool ConnectedState::retrivOperatorsCodes(const char *response, int *mcc, int *mnc) {
    char StringToCompare[7] = "+QOPS:";

    if (strncmp(response, StringToCompare, strlen(StringToCompare)) == 0) {
        // Variables para almacenar los códigos MCC y MNC
        char operatorCode[10] = {0};
        
        // Parsear la respuesta para obtener el código del operador
        int n = sscanf(response, "+QOPS: \"%*[^\"]\",\"%*[^\"]\",\"%[^\"]\"", operatorCode);

        if (n == 1) {
            // Extraer los primeros 3 dígitos para MCC
            char mccStr[4] = {0};
            strncpy(mccStr, operatorCode, 3);
            *mcc = atoi(mccStr);

            // Extraer los siguientes dígitos para MNC
            char mncStr[4] = {0};
            strncpy(mncStr, operatorCode + 3, strlen(operatorCode) - 3);
            *mnc = atoi(mncStr);

            
            // Mostrar los valores de MCC y MNC
            char buffer[50];  // Buffer temporal para la salida
           // snprintf(buffer, sizeof(buffer), "MCC: %d, MNC: %d\n", *mcc, *mnc);
           // uartUSB.write(buffer, strlen(buffer));  // Enviar por UART

            return true;
        }
    }
    return false;
}

bool ConnectedState::retrivCellData(const char *response, char *tec, int *idCell, int *lac, float *prx) {
    char accessTechnologyretrived[7];
    int index = 2;

    // Verificar que la respuesta comience con un número (ignorar si no es)
    if (isdigit(response[0]) == 0) {
        return false;
    }

    // Extraer la tecnología de acceso (RAT)
    sscanf(response, "%d,\"%6[^\"]\",", &index, accessTechnologyretrived);

    // Comparar la tecnología y extraer los valores correspondientes según el RAT
    if (strcmp(accessTechnologyretrived, "2G") == 0) {
        // Para 2G: index, RAT, freq, lac, ci, bsic, rxlev, c1, cba, is_gprs_support
        sscanf(response, "%*d,\"%*[^\"]\",%*d,%x,%x,%*d,%f", lac, idCell, prx);
        // Convertir los valores de lac y idCell de int a cadena de caracteres
    } else if (strcmp(accessTechnologyretrived, "3G") == 0) {
        // Para 3G: index, RAT, freq, psc, lac, ci, rscp, ecno, cba
        sscanf(response, "%*d,\"%*[^\"]\",%*d,%*d,%x,%x,%f", lac, idCell, prx);

    } else if (strcmp(accessTechnologyretrived, "4G") == 0) {
        // Para 4G: index, RAT, freq, pci, tac, ci, rsrp, rsrq, cba
        sscanf(response, "%*d,\"%*[^\"]\",%*d,%*d,%x,%x,%f", lac, idCell, prx);

    } else {
        return false;
    }

    // Copiar la tecnología de acceso recuperada al parámetro tec
    strcpy(tec, accessTechnologyretrived);

    return true;
}
