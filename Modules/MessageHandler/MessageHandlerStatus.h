#ifndef _MESSAGE_HANDLER_STATUS_H_
#define _MESSAGE_HANDLER_STATUS_H_

typedef enum {
    MESSAGE_HANDLER_STATUS_PROCESSING_MESSAGE,
    MESSAGE_HANDLER_STATUS_PROCESSED,
    MESSAGE_HANDLER_STATUS_ERROR_PACKET_TOO_SMALL,
    MESSAGE_HANDLER_STATUS_CHECKSUM_ERROR,
    MESSAGE_HANDLER_STATUS_FAIL_TO_COMPUTE_CHECKSUM,
    MESSAGE_HANDLER_STATUS_AUTHENTIFICATION_MISSMATCH,
} MessageHandlerStatus_t;

#endif // _MESSAGE_HANDLER_STATUS_H_