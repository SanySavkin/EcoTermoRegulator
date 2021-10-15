#ifndef MESSAGES_SENDER_H_
#define MESSAGES_SENDER_H_

#include "string.h"
#include "stdint.h"
#include "stdbool.h"
#include "ServerCommunication\Src\messages_proto.h"


#ifdef __cplusplus
extern "C" {
#endif

bool MS_SendMsgToMaster_Error(GlobalErrorCode_t);
bool MS_SendMsgToMaster_Ping(uint32_t senderId);


#ifdef __cplusplus
}
#endif
	
	
#endif //MESSAGES_SENDER_H_
