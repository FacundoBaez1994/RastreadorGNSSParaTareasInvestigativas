//=====[Libraries]=============================================================

#include "DecrypterBase64.h"
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
DecrypterBase64::DecrypterBase64 () {
    this->aes = new AES ();
    this->nextHandler = nullptr;
}


/** 
* @brief 
* 
* 
* @returns 
*/
DecrypterBase64::~DecrypterBase64 () {
    delete this->aes;
    this->aes = nullptr;
    this->nextHandler = nullptr;
}

MessageHandlerStatus_t  DecrypterBase64::handleMessage (char * message, unsigned int sizeOfMessage) {
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


//=====[Implementations of private methods]===================================









