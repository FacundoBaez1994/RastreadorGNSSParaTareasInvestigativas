//=====[Libraries]=============================================================

#include "Decrypter.h"
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
Decrypter::Decrypter () {
    this->aes = new AES ();
    this->nextHandler = nullptr;
}


/** 
* @brief 
* 
* 
* @returns 
*/
Decrypter::~Decrypter () {
    delete this->aes;
    this->aes = nullptr;
    this->nextHandler = nullptr;
}

MessageHandlerStatus_t  Decrypter::handleMessage (char * message, unsigned int sizeOfMessage) {
    static char base64_decoded [2564] = {0};
    static char log [120];
    int ret;
    size_t decoded_len = 0;
    static bool initialization = false;

    if (initialization  == false) {
        memset(base64_decoded , 0, sizeof(base64_decoded ));
        initialization = true;
    }

    ret = mbedtls_base64_decode((unsigned char*)base64_decoded, sizeof(base64_decoded), &decoded_len,
                                (unsigned char*)message, strlen (message));

    if (ret != 0) {
        snprintf(log, sizeof(log), "\r\nError decodificando en base64: %d\r\n", ret);
        uartUSB.write (log, strlen (log));  // debug only
        base64_decoded[decoded_len] = '\0';
    }

    uartUSB.write ("\r\nencrypted message:\r\n", strlen ("encrypted message:\r\n"));  // debug only
    uartUSB.write (base64_decoded, strlen ( base64_decoded));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only
    
    this->aes->setup(this->key, AES::KEY_256, AES::MODE_CBC, this->iv);
    this->aes->decrypt(base64_decoded , sizeof (base64_decoded));
    this->aes->clear();

    strcpy (message, base64_decoded);

    uartUSB.write ( "\r\n",  3 );  // debug only
    uartUSB.write ("\r\ndecrypted message:\r\n", strlen ("decrypted message:\r\n"));  // debug only
    uartUSB.write ( message, strlen (  message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    if (this->nextHandler == nullptr) {
        initialization = false;
        return  MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        initialization = false;
        return this->nextHandler->handleMessage ( message, sizeOfMessage);
    }
    
}

/*
MessageHandlerStatus_t  Decrypter::handleMessage (char * message) {

    uartUSB.write ("encrypted message:\r\n", strlen ("encrypted message:\r\n"));  // debug only
    uartUSB.write ( message, strlen ( message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only
    
    aes->setup(this->key, AES::KEY_256, AES::MODE_CBC, this->iv);
    aes->decrypt(message , strlen(message));
    aes->clear();

    uartUSB.write ( "\r\n",  3 );  // debug only
    uartUSB.write ("decrypted message:\r\n", strlen ("decrypted message:\r\n"));  // debug only
    uartUSB.write ( message, strlen (  message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    // Eliminar caracteres '@' al final de message
    size_t len = strlen(message);
    while (len > 0 && message[len - 1] == '@') {
        message[len - 1] = '\0';  // Reemplazar '@' por terminador nulo
        len--;
    }

    uartUSB.write("message without padding:\r\n", strlen("message without padding:\r\n"));  // debug only
    uartUSB.write(message, strlen(message));  // debug only
    uartUSB.write("\r\n", 3);  // debug only

    if (this->nextHandler == nullptr) {
        return  MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage ( message);
    }
    
}
*/

//=====[Implementations of private methods]===================================









