#include "ServerCommunication/Src/messages_proto.h"
#include "FatfsToSpifs/Src/files_copier.h"
#include "Spiffs/Src/appl_spiffs.h"
#include "messages_sender.h"
#include "ExternalFlash/Src/appl_ext_flash.h"



#define DEVICE_ID 1
#define SERVER_ID 0xFFFF



volatile uint32_t countMessages = 0;
void OnMessageCopyFolderReceived(ProtoTransportP pr, MsgCopyFolder_t* msg){
	countMessages++;
	uint8_t dstLen;
	uint8_t srcLen = strlen((char*)msg->data.ptr);
	if(srcLen < msg->data.len){
		dstLen = strlen((char*)(msg->data.ptr + srcLen + 1));
	}
	if(srcLen + dstLen + 2 == msg->data.len){
		if(!ExtFlash_Initialise()){ 
			MS_SendMsgToMaster_Error(GLOB_ERR_FLASH_NOT_ANSWER);
		}
		else {
			int32_t r = FC_CopyFolderFatfsToSpiffs((char*)msg->data.ptr, (char*)(msg->data.ptr + srcLen + 1));
			MS_SendMsgToMaster_Error((GlobalErrorCode_t)r);
		}
	}
}

void OnMessageSpiffsSettingsReceived(ProtoTransportP pr, MsgSpiffsSettings_t* msg){
	if(!ExtFlash_Initialise()){
			MS_SendMsgToMaster_Error(GLOB_ERR_FLASH_NOT_ANSWER);
	}
	else{
		int32_t r = ApplSpiffs_Initialise(msg->flashSize, msg->spiffsAddr, msg->eraseSize, msg->pageSize, msg->logBlockSize, msg->allowFormating);
		MS_SendMsgToMaster_Error((GlobalErrorCode_t)r);
	}
}

void OnMessagePingReceived(ProtoTransportP pr, MsgPing_t* msg){
	if(msg->senderId == SERVER_ID)
		MS_SendMsgToMaster_Ping(DEVICE_ID);
}

void OnMessageCreateImageReceived(ProtoTransportP pr, MsgCreateImage_t* msg){
	if(!ExtFlash_Initialise()){
			MS_SendMsgToMaster_Error(GLOB_ERR_FLASH_NOT_ANSWER);
	}
	else{
		int32_t r = FC_CreateImageReceived((char*)msg->fileName.ptr);
		MS_SendMsgToMaster_Error((GlobalErrorCode_t)r);
	}
}


