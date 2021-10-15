#include "ProtoCommunication\Src\messages_proto.h"
#include "ProtoCommunication\Src\transport.h"
#include "main.h"


#define MSG_ID_BASE 0x12340U


typedef enum{
	MSG_ID_WRITE_START = MSG_ID_BASE,
	MSG_ID_WRITE_START_REPLY,
	MSG_ID_WRITE,
	MSG_ID_WRITE_REPLY,
	MSG_ID_WRITE_END,
	MSG_ID_WRITE_END_REPLY,
	MSG_ID_ERROR,
	MSG_ID_GET_FLASH_TYPE,
	MSG_ID_WRITE_FOLDER,
	MSG_ID_SPIFFS_SETTINGS,
	MSG_ID_PING,
	MSG_ID_CREATE_IMAGE,
}MesagesId_t;


static void DispatchMessage(ProtoTransportP, uint8_t*, uint32_t);


bool SendMes_WriteFileStartReply(ProtoTransportP pr, MsgWriteStart_t* msg){
	return ProtoTransp_Send(pr, MSG_ID_WRITE_START_REPLY, (uint8_t*)msg, sizeof(MsgWriteStart_t) - sizeof(MsgDataStream), msg->fileName.ptr, msg->fileName.len);
}

bool SendMes_WriteFileReply(ProtoTransportP pr, MsgWriteReply_t* msg){
	return ProtoTransp_Send(pr, MSG_ID_WRITE_REPLY, (uint8_t*)msg,  sizeof(MsgWriteReply_t), NULL, 0);
}

bool SendMes_WriteFileEndReply(ProtoTransportP pr, MsgWriteEnd_t* msg){
	return ProtoTransp_Send(pr, MSG_ID_WRITE_END_REPLY, (uint8_t*)msg->hashSum.ptr,  msg->hashSum.len, NULL, 0);
}

bool SendMes_Error(ProtoTransportP pr, MsgError_t* msg){
	return ProtoTransp_Send(pr, MSG_ID_ERROR, (uint8_t*)msg,  sizeof(MsgError_t), NULL, 0);
}

bool SendMes_Ping(ProtoTransportP pr, MsgPing_t* msg){
	return ProtoTransp_Send(pr, MSG_ID_PING, (uint8_t*)msg,  sizeof(MsgPing_t), NULL, 0);
}

void ProtoTransp_ReceiveCmpltClbk(ProtoTransportP pr, uint8_t* data, uint32_t len){
	DispatchMessage(pr, data, len);
}

static bool Parse(void* _msg, uint32_t _lenFixed, bool _isStream, uint8_t* _data, uint32_t _fullLen){
	if(_fullLen < _lenFixed) return false;
	if(_lenFixed != 0) memcpy(_msg, _data, _lenFixed);
	if(_isStream && _fullLen != _lenFixed){
		((MsgDataStream*)((uint8_t*)_msg + _lenFixed))->ptr = _data + _lenFixed;
		((MsgDataStream*)((uint8_t*)_msg + _lenFixed))->len = _fullLen - _lenFixed;
	}
	return true;
}

static void DispatchMessage(ProtoTransportP pr, uint8_t* _data, uint32_t _len){
	uint8_t* msgBegin = _data + 4;
	uint32_t messageLen = _len - 4; //without msgId
	switch (*(uint32_t*)_data)
    {
    	case MSG_ID_WRITE_START:
			{
				MsgWriteStart_t msg;
				if(Parse(&msg, sizeof(MsgWriteStart_t) - sizeof(MsgDataStream), true, msgBegin, messageLen)){
					OnMessageWriteStartReceived(pr, &msg);
				}
			}
    		break;
    	case MSG_ID_WRITE:
			{
				MsgWrite_t msg;
				if(Parse(&msg, sizeof(MsgWrite_t) - sizeof(MsgDataStream), true, msgBegin, messageLen)){
					OnMessageWriteReceived(pr, &msg);
				}
			}
    		break;
		case MSG_ID_WRITE_END:
			{
				MsgWriteEnd_t msg;
				if(Parse(&msg, sizeof(MsgWriteEnd_t) - sizeof(MsgDataStream), true, msgBegin, messageLen)){
					OnMessageWriteEndReceived(pr, &msg);
				}
			}
			break;
		case MSG_ID_WRITE_FOLDER:
			{
				MsgCopyFolder_t msg;
				if(Parse(&msg, sizeof(MsgCopyFolder_t) - sizeof(MsgDataStream), true, msgBegin, messageLen)){
					OnMessageCopyFolderReceived(pr, &msg);
				}
			}
			break;
		case MSG_ID_GET_FLASH_TYPE:
			break;
		case MSG_ID_SPIFFS_SETTINGS:
			{
				MsgSpiffsSettings_t msg;
				if(Parse(&msg, sizeof(MsgSpiffsSettings_t), false, msgBegin, messageLen)){
					OnMessageSpiffsSettingsReceived(pr, &msg);
				}
			}
			break;
		case MSG_ID_PING:
			{
				MsgPing_t msg;
				if(Parse(&msg, sizeof(MsgPing_t), false, msgBegin, messageLen)){
					OnMessagePingReceived(pr, &msg);
				}
			}
			break;
		case MSG_ID_CREATE_IMAGE:
			{
				MsgCreateImage_t msg;
				if(Parse(&msg, sizeof(MsgCreateImage_t) - sizeof(MsgDataStream), true, msgBegin, messageLen)){
					OnMessageCreateImageReceived(pr, &msg);
				}
			}
			break;
    	default:
    		break;
    }
}

__weak void OnMessageWriteStartReceived(ProtoTransportP pr, MsgWriteStart_t* msg){UNUSED(pr);UNUSED(msg);}
__weak void OnMessageWriteReceived(ProtoTransportP pr, MsgWrite_t* msg){UNUSED(pr);UNUSED(msg);}
__weak void OnMessageCopyFolderReceived(ProtoTransportP pr, MsgCopyFolder_t* msg){UNUSED(pr);UNUSED(msg);}
__weak void OnMessageWriteEndReceived(ProtoTransportP pr, MsgWriteEnd_t* msg){UNUSED(pr);UNUSED(msg);}
__weak void OnMessageSpiffsSettingsReceived(ProtoTransportP pr, MsgSpiffsSettings_t* msg){UNUSED(pr);UNUSED(msg);}
__weak void OnMessagePingReceived(ProtoTransportP pr, MsgPing_t* msg){UNUSED(pr);UNUSED(msg);}
__weak void OnMessageCreateImageReceived(ProtoTransportP pr, MsgCreateImage_t* msg){UNUSED(pr);UNUSED(msg);}
