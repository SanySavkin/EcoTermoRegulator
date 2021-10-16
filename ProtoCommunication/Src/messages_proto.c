#include "ProtoCommunication\Src\messages_proto.h"
#include "ProtoCommunication\Src\transport.h"
#include "main.h"


#define MSG_ID_BASE 0x1U


typedef enum{
	MSG_ID_SETTINGS = MSG_ID_BASE,
	MSG_ID_START_STOP,
	MSG_ID_SET_POSITION,
	MSG_ID_SYNCHRONIZE,
	MSG_ID_ERROR,
	MSG_ID_PING,
}MesagesId_t;


static void DispatchMessage(ProtoTransportP, uint8_t*, uint32_t);


bool SendMes_Settings(ProtoTransportP pr, MsgSettings_t* msg){
	return ProtoTransp_Send(pr, MSG_ID_SETTINGS, (uint8_t*)msg, sizeof(MsgSettings_t), NULL, 0);
}

bool SendMes_Start(ProtoTransportP pr, MsgStart_t* msg){
	return ProtoTransp_Send(pr, MSG_ID_START_STOP, (uint8_t*)msg, sizeof(MsgStart_t), NULL, 0);
}

bool SendMes_SetPosition(ProtoTransportP pr, MsgSetPosition_t* msg){
	return ProtoTransp_Send(pr, MSG_ID_SET_POSITION, (uint8_t*)msg, sizeof(MsgSetPosition_t), NULL, 0);
}

bool SendMes_Synchronize(ProtoTransportP pr, MsgSynchronize_t* msg){
	return ProtoTransp_Send(pr, MSG_ID_SYNCHRONIZE, (uint8_t*)msg,  sizeof(MsgSynchronize_t), NULL, 0);
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
	uint8_t* msgBegin = _data + MESSAGE_ID_LENGTH;
	uint32_t messageLen = _len - MESSAGE_ID_LENGTH; //without msgId
	switch (*(uint32_t*)_data)
    {
    	case MSG_ID_START_STOP:
			{
				MsgStart_t msg;
				if(Parse(&msg, sizeof(MsgStart_t), false, msgBegin, messageLen)){
					OnMessageStartReceived(pr, &msg);
				}
			}
    		break;
    	case MSG_ID_SETTINGS:
			{
				MsgSettings_t msg;
				if(Parse(&msg, sizeof(MsgSettings_t), false, msgBegin, messageLen)){
					OnMessageSettingsReceived(pr, &msg);
				}
			}
    		break;
		case MSG_ID_SET_POSITION:
			{
				MsgSetPosition_t msg;
				if(Parse(&msg, sizeof(MsgSetPosition_t), false, msgBegin, messageLen)){
					OnMessageSetPositionReceived(pr, &msg);
				}
			}
			break;
		case MSG_ID_SYNCHRONIZE:
			{
				MsgSynchronize_t msg;
				if(Parse(&msg, sizeof(MsgSynchronize_t), false, msgBegin, messageLen)){
					OnMessageSynchronizeReceived(pr, &msg);
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
    	default:
    		break;
    }
}

__weak void OnMessageStartReceived(ProtoTransportP pr, MsgStart_t* msg){UNUSED(pr);UNUSED(msg);}
__weak void OnMessageSettingsReceived(ProtoTransportP pr, MsgSettings_t* msg){UNUSED(pr);UNUSED(msg);}
__weak void OnMessageSynchronizeReceived(ProtoTransportP pr, MsgSynchronize_t* msg){UNUSED(pr);UNUSED(msg);}
__weak void OnMessageSetPositionReceived(ProtoTransportP pr, MsgSetPosition_t* msg){UNUSED(pr);UNUSED(msg);}
__weak void OnMessagePingReceived(ProtoTransportP pr, MsgPing_t* msg){UNUSED(pr);UNUSED(msg);}
