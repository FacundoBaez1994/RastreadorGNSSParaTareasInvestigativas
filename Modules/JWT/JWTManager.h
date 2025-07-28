//=====[#include guards - begin]===============================================

#ifndef _JWT_MANAGER_H_
#define _JWT_MANAGER_H_

#include "arm_book_lib.h"
#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "string.h"
#include <CustomJWT.h>


//=====[Declaration of public defines]=========================================



//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================

class JWTManager {
public:
    JWTManager ();
    virtual ~JWTManager ();

    void encodeJWT(char * payloadToJWT, char * jwtEncoded);
    bool decodeJWT (char * jwtToDecode, char * payloadRetrived);

private:
    CustomJWT * jwt;
    char JWTKey [40] = "a-string-secret-at-least-256-bits-long";

};


//=====[#include guards - end]=================================================

#endif // _JWT_MANAGER_H_
