//=====[Libraries]=============================================================

#include "AuthenticationVerifier.h"
#include "Debugger.h" // due to global usbUart


//=====[Declaration of private defines]========================================
    const char key[32] = {
        0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE,
        0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81,
        0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7,
        0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4
    };
    const char iv[16] = {
        0x74, 0x11, 0xF0, 0x45, 0xD6, 0xA4, 0x3F, 0x69,
        0x18, 0xC6, 0x75, 0x42, 0xDF, 0x4C, 0xA7, 0x84
    };
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
AuthenticationVerifier::AuthenticationVerifier () {
    this->nextHandler = nullptr;
}


/** 
* @brief 
* 
* 
* @returns 
*/
AuthenticationVerifier::~AuthenticationVerifier () {
    this->nextHandler = nullptr;
}


MessageHandlerStatus_t  AuthenticationVerifier::handleMessage (char * message) {
     unsigned char keyhmac [] = "KURRRVWWWWAAAAA";


    // Extraer el HMAC del final del mensaje (antes del CRC)
    size_t packetSize = strlen (message);
    size_t hmacOffset = packetSize - 32; // Últimos 32 bytes
    //size_t hmacOffset = strlen (messageWithoutCRC)- 32; // Últimos 32 bytes
    unsigned char receivedHMAC[32] = {0};
    //char encryptedMessageReceived [200];

    uartUSB.write ("message in AuthenticationVerifier:\r\n", strlen ("message in AuthenticationVerifier:\r\n"));  // debug only
    uartUSB.write (message, strlen (message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    memcpy(receivedHMAC, &message[hmacOffset], 32);

    // Imprimir el mensaje cifrado
    uartUSB.write ("Hmac received:\r\n", strlen ("Hmac received:\r\n"));  // debug only
    uartUSB.write (receivedHMAC, sizeof (receivedHMAC));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    // Verificar HMAC
    unsigned char calculatedHMAC[32];
    
    message [hmacOffset] = '\0';
    
    uartUSB.write ("message received without HMAC\r\n", strlen ("message received without HMAC\r\n"));  // debug only
    uartUSB.write ( message , strlen(message ));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only


    this->generate_hmac(keyhmac, strlen((char *)keyhmac), (unsigned char *) message, strlen (message), calculatedHMAC);


    uartUSB.write ("Hmac calculated:\r\n", strlen ("Hmac calculated:\r\n"));  // debug only
    uartUSB.write (calculatedHMAC, strlen((char *)calculatedHMAC));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only
    int nullCount = 0;
    // Recorrer el arreglo y contar cuántos valores son '\0'
    for (int i = 0; i < 32; i++) {
        if (calculatedHMAC[i] == 0) { // 0 en caracter es el '\0'
            nullCount++;
            calculatedHMAC[i] = '0';
        }
    }

    // Comparar el HMAC recibido con el calculado
    if (memcmp(receivedHMAC, calculatedHMAC, sizeof(receivedHMAC)) == 0) {
        uartUSB.write("HMAC OK!\r\n", strlen("HMAC OK!\r\n")); // HMAC es válido
    } else {
        uartUSB.write("HMAC Error\r\n", strlen("HMAC Error\r\n")); // HMAC no coincide
        return MESSAGE_HANDLER_STATUS_AUTHENTIFICATION_MISSMATCH;
    }

    // Imprimir el mensaje cifrado
    uartUSB.write ("encrypted Message Received Without HMAC:\r\n", strlen ("encrypted Message Received Without HMAC:\r\n"));  // debug only
    uartUSB.write (message, strlen (message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    
    if (this->nextHandler == nullptr) {
        return  MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage (message);
    }
    
}

//=====[Implementations of private methods]===================================
void AuthenticationVerifier::generate_hmac(const unsigned char *key, size_t key_len, const unsigned char *message, size_t message_len, unsigned char *hmac_out)
{
    mbedtls_md_context_t ctx;
    const mbedtls_md_info_t *md_info;

    // Inicializar el contexto de HMAC
    mbedtls_md_init(&ctx);

    // Seleccionar el tipo de hash (SHA-256, por ejemplo)
    md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

    // Inicializar el contexto con la información del hash
    mbedtls_md_setup(&ctx, md_info, 1);

    // Procesar el mensaje con la clave para generar el HMAC
    mbedtls_md_hmac_starts(&ctx, key, key_len);
    mbedtls_md_hmac_update(&ctx, message, message_len);
    mbedtls_md_hmac_finish(&ctx, hmac_out);

    // Limpiar el contexto
    mbedtls_md_free(&ctx);
}









