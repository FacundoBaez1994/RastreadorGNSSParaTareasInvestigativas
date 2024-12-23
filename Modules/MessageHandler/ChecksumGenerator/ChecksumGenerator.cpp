//=====[Libraries]=============================================================

#include "ChecksumGenerator.h"
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
ChecksumGenerator::ChecksumGenerator () {
    this->nextHandler = nullptr;
}


/** 
* @brief 
* 
* 
* @returns 
*/
ChecksumGenerator::~ChecksumGenerator () {
    this->nextHandler = nullptr;
}

MessageHandlerStatus_t ChecksumGenerator::handleMessage(char *message) {
    // Crear una instancia de MbedCRC con el polinomio 32-bit ANSI
    MbedCRC<POLY_32BIT_ANSI, 32> crc32;
    uint32_t crc; // Variable para almacenar el resultado del CRC de 32 bits

    // Calcular el CRC32 del mensaje original (sin el CRC agregado)
    size_t messageLength = strlen(message);  // Longitud del mensaje original
    if (crc32.compute(reinterpret_cast<uint8_t*>(message), messageLength, &crc) == 0) {
        char crcStr[9]; // Espacio para 8 dígitos hexadecimales + terminador nulo
        sprintf(crcStr, "%08X", crc); // Convertir a formato hexadecimal con 8 caracteres

        // Depuración: imprimir el CRC generado
        uartUSB.write("Generated CRC32:\r\n", strlen("Generated CRC32:\r\n"));
        uartUSB.write(crcStr, strlen(crcStr));
        uartUSB.write("\r\n", 3);

        // Calcular la longitud total del mensaje (incluyendo el CRC)
        size_t totalLength = messageLength + strlen(crcStr) + 1; // +1 para el terminador nulo
        if (totalLength >= 256) { // Asegurarse de que el mensaje no supere el límite de 255 caracteres
            uartUSB.write("Message too long to append CRC\r\n", strlen("Message too long to append CRC\r\n"));
            return MESSAGE_HANDLER_STATUS_FAIL_TO_COMPUTE_CHECKSUM;
        }

        // Anexar el CRC al final del mensaje
        strcat(message, crcStr);

        // Depuración: imprimir el mensaje completo con CRC
        uartUSB.write("Complete message with CRC:\r\n", strlen("Complete message with CRC:\r\n"));
        uartUSB.write(message, strlen(message));
        uartUSB.write("\r\n", 3);

    } else {
        uartUSB.write("Failed to compute CRC\r\n", strlen("Failed to compute CRC\r\n"));
        return MESSAGE_HANDLER_STATUS_FAIL_TO_COMPUTE_CHECKSUM;
    }

    // Pasar el mensaje al siguiente manejador si existe
    if (this->nextHandler == nullptr) {
        return MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage(message);
    }
}


/*
MessageHandlerStatus_t ChecksumGenerator::handleMessage(char *message) {
    // Crear una instancia de MbedCRC con el polinomio 32-bit ANSI
    MbedCRC<POLY_32BIT_ANSI, 32> crc32;
    uint32_t crc; // Variable para almacenar el resultado del CRC
    //char crcMessage[5]; // 4 bytes de CRC + terminador nulo

    if (crc32.compute(reinterpret_cast<uint8_t*>(message), strlen(message), &crc) == 0) {
        char crcStr[9]; // Espacio suficiente para almacenar un uint32_t en hexadecimal
        sprintf(crcStr, "0x%08X", crc); // Convertir a formato hexadecimal
        uartUSB.write("Generated CRC32:\r\n", strlen("Generated CRC32:\r\n")); // Imprimir etiqueta
        uartUSB.write(crcStr, strlen(crcStr)); // Imprimir el CRC en formato texto
        uartUSB.write("\r\n", 3); // Nueva línea

        // Preparar los bytes del CRC en `crcMessage`
        crcStr[0] = (crc >> 24) & 0xFF;
        crcStr[1] = (crc >> 16) & 0xFF;
        crcStr[2] = (crc >> 8) & 0xFF;
        crcStr[3] = crc & 0xFF;
        crcStr[4] = '\0'; // Terminador nulo por seguridad

        uartUSB.write("CRC:\r\n", strlen(" CRC:\r\n"));
        uartUSB.write(crcStr, strlen (crcStr)); // Imprimir mensaje con CRC


        // Calcular la longitud actual del mensaje
        size_t messageLength = strlen(message);

        // Usar memcpy para copiar los 4 bytes de CRC al final de `message`
        strcat(message, crcStr);

        // Actualizar el terminador nulo de `message`
        message[messageLength + 4] = '\0';

        // Depuración: imprimir el mensaje completo con el CRC
        uartUSB.write("Complete message with CRC:\r\n", strlen("Complete message with CRC:\r\n"));
        uartUSB.write(message, messageLength + 4); // Imprimir mensaje con CRC
        uartUSB.write("\r\n", 3); // Nueva línea
    } else {
        uartUSB.write("Failed to compute CRC\r\n", strlen("Failed to compute CRC\r\n")); // Depuración
        return MESSAGE_HANDLER_STATUS_FAIL_TO_COMPUTE_CHECKSUM;
    }

    // Pasar el mensaje al siguiente manejador si existe
    if (this->nextHandler == nullptr) {
        return MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage(message);
    }
}




MessageHandlerStatus_t  ChecksumGenerator::handleMessage (char * message) {
  
    // Crear una instancia de MbedCRC con el polinomio 32-bit ANSI
    MbedCRC<POLY_32BIT_ANSI, 32> crc32;
    // Variable para almacenar el resultado del CRC
    uint32_t crc;
    char crcMessage [5];

    if (crc32.compute(reinterpret_cast<uint8_t*>(message), strlen(message), &crc) == 0) {
        char crcStr[12]; // Espacio suficiente para almacenar un uint32_t en hexadecimal
        sprintf(crcStr, "0x%08X", crc); // Convertir a formato hexadecimal
        uartUSB.write("generated CRC32:\r\n", strlen("generated CRC32:\r\n")); // Imprimir etiqueta
        uartUSB.write(crcStr, strlen(crcStr)); // Imprimir el CRC en formato texto
        uartUSB.write("\r\n", 3); // Nueva línea


    
        size_t messageLength = strlen(message);

        // Añadir el CRC al final del buffer
        message[messageLength]     = (crc >> 24) & 0xFF;
        message[messageLength + 1] = (crc >> 16) & 0xFF;
        message[messageLength + 2] = (crc >> 8) & 0xFF;
        message[messageLength + 3] = crc & 0xFF;

        // Asegurarte de ajustar el tamaño total
        messageLength += 4;
        message[messageLength] = '\0'; 
       
        crcMessage[0]     = (crc >> 24) & 0xFF;
        crcMessage[1] = (crc >> 16) & 0xFF;
        crcMessage[2] = (crc >> 8) & 0xFF;
        crcMessage[3] = crc & 0xFF;
        crcMessage [4] = '\0'; 
        
        uartUSB.write ("CRC:\r\n", strlen ("CRC:\r\n"));  // debug only
        uartUSB.write (crcMessage, strlen (crcMessage));  // debug only


        // Calcular la longitud actual del mensaje
        size_t messageLength = strlen(message);
        // Usar memcpy para copiar los 4 bytes de CRC al final de `message`
        memcpy(&message[messageLength], crcMessage, 4); // Copiar solo los 4 bytes del CRC
        // Actualizar el terminador nulo de `message`
        message[messageLength + 4] = '\0';



        uartUSB.write ("Complete message with CRC:\r\n", strlen ("Complete message with CRC:\r\n"));  // debug only
        uartUSB.write (message, messageLength);  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    } else {
        uartUSB.write ("Failed to compute CRC\r\n", strlen ("Failed to compute CRC\r\n"));  // debug only
        return MESSAGE_HANDLER_STATUS_FAIL_TO_COMPUTE_CHECKSUM;
    }

    
    if (this->nextHandler == nullptr) {
        return  MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage (message);
    }
}
 */










