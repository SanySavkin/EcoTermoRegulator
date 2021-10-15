#include "messages_sender.h"


extern ProtoTransport_t protoUsb;


bool MS_SendMsgToMaster_Error(GlobalErrorCode_t _errorCode){
	MsgError_t msg = {.errorCode = _errorCode};
	return SendMes_Error(&protoUsb, &msg);
}

bool MS_SendMsgToMaster_Ping(uint32_t senderId){
	MsgPing_t msg = {senderId};
	return SendMes_Ping(&protoUsb, &msg);
}

