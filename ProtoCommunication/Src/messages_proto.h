#ifndef MESSAGES_PROTO_H_
#define MESSAGES_PROTO_H_

#include "string.h"
#include "stdint.h"
#include "stdbool.h"
#include "transport.h"


#ifdef __cplusplus
extern "C" {
#endif


#define FILE_NAME_LEN_MAX 32

typedef enum{
	GLOB_ERR_NONE = 0,
	GLOB_ERR_ALLOCATE_MEMORY = 1,
	GLOB_ERR_SPIFFS_MOUNTING = 2,
	GLOB_ERR_FLASH_PARAMETERS = 3,
	GLOB_ERR_SPIFFS_FORMATING = 4,
	GLOB_ERR_LONG_FILE_NAME = 5,
	GLOB_ERR_FILE_READ = 6,
	GLOB_ERR_FILE_WRITE = 7,
	GLOB_ERR_NOT_A_FILES = 8,
	GLOB_ERR_HASH = 9,
	GLOB_ERR_FLASH_NOT_ANSWER = 10,
	GLOB_ERROR_OPEN_FILE = 11,
	GLOB_ERR_IMAGE_NOT_CORRECT = 12,
	GLOB_ERR_FILES_NOT_FOUNDED = 13,
}GlobalErrorCode_t;


//#pragma pack(push,1)

typedef struct{
	uint8_t* ptr;
	uint32_t len;
}MsgDataStream;

typedef struct{
	uint32_t fileSize;
	MsgDataStream fileName;
}MsgWriteStart_t; //Msg_WriteStartReply_t too

typedef struct{
	uint32_t ofset;
	MsgDataStream data;
}MsgWrite_t;

typedef struct{
	MsgDataStream data;
}MsgCopyFolder_t;

typedef struct{
	uint32_t len;
	uint32_t ofset;
}MsgWriteReply_t;

typedef struct{
	MsgDataStream hashSum;
}MsgWriteEnd_t; //Msg_WriteEndReply_t too

typedef struct{
	uint32_t flashSize;
	uint32_t spiffsAddr;
	uint32_t eraseSize;
	uint32_t pageSize;
	uint32_t logBlockSize;
	bool allowFormating;
}MsgSpiffsSettings_t;

typedef struct{
	uint32_t errorCode;
}MsgError_t;

typedef struct{
	uint32_t senderId;
}MsgPing_t;

typedef struct{
	MsgDataStream fileName;
}MsgCreateImage_t;

//#pragma pack(pop)


void MessagesProtoInitialise(void);
void OnWriteReceived(char* _data);

bool SendMes_WriteFileStartReply(ProtoTransportP pr, MsgWriteStart_t* msg);
bool SendMes_WriteFileReply(ProtoTransportP pr, MsgWriteReply_t* msg);
bool SendMes_WriteFileEndReply(ProtoTransportP pr, MsgWriteEnd_t* msg);
bool SendMes_Error(ProtoTransportP pr, MsgError_t* msg);
bool SendMes_Ping(ProtoTransportP pr, MsgPing_t* msg);

void OnMessageWriteReceived(ProtoTransportP pr, MsgWrite_t* msg);
void OnMessageWriteStartReceived(ProtoTransportP pr, MsgWriteStart_t* msg);
void OnMessageCopyFolderReceived(ProtoTransportP pr, MsgCopyFolder_t* msg);
void OnMessageWriteEndReceived(ProtoTransportP pr, MsgWriteEnd_t* msg);
void OnMessageSpiffsSettingsReceived(ProtoTransportP pr, MsgSpiffsSettings_t* msg);
void OnMessagePingReceived(ProtoTransportP pr, MsgPing_t* msg);
void OnMessageCreateImageReceived(ProtoTransportP pr, MsgCreateImage_t* msg);



#ifdef __cplusplus
}
#endif

#endif //MESSAGES_PROTO_H_
