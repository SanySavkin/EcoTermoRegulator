#ifndef MESSAGES_PROTO_H_
#define MESSAGES_PROTO_H_

#include "string.h"
#include "stdint.h"
#include "stdbool.h"
#include "transport.h"


#ifdef __cplusplus
extern "C" {
#endif


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

//s2c
typedef struct{
	uint32_t delayBefore; 	//опережение закрытия клапана перед выходом счетчика из энергосберегающего режима.
	uint32_t delayAfter;	//задержка открытия клапана после входа счетчика в энергосберегающий режим.
	uint32_t durationOn;	//продолжительность открытого клапана.
	uint32_t cycleCount;	//число пропуска периодов (если 0, то открытие клапана каждый период).
	uint32_t period;		//периоде цикла, мсек;
	uint32_t activeTime;	//максимальное время работы от синхронизации, ms.
	uint32_t correctPer;	//период коррекции времени, мс;
	int16_t correctValue;	//значение коррекции времени, мкс; MIN -900, MAX +900;
	uint16_t angleOn;		//угол, при котором клапан полностью открыт (минуты).
	uint16_t angleOff;		//угол, при котором клапан полностью закрыт (минуты).
	uint8_t valveStepPer;	//период (в милисек) одного шага (в процентах) клапана; Необходимо для плавного нажатия и отжатия клапана;
	
	bool isOpenWhenStopped;	//положение клапана после истечения activeTime. if(isOpenWhenStopped) клапан открыт; else клапан закрыт.
}MsgSettings_t;

//c2s
typedef struct{
	uint32_t closePosition; 	//значение в минутах, когда клапан закрыт полностью.
	uint16_t currentPosition;	//угол в минутах.
	bool isActive;				//основной цикл работы активен?
}MsgStatus_t;

//s2c
typedef struct{
	uint16_t position;	//угол в минутах.
}MsgSetPosition_t;

//s2c c2s (s2c - во время события "измерение" + delayAfter; c2s - во время открытия клапана)
typedef struct{
	uint32_t id; 	//identifier
}MsgSynchronize_t;

//s2c
typedef struct{
	bool isStart;	//if(isStart) start; else stop;
}MsgStart_t;

//c2s
typedef struct{
	uint32_t errorCode;
}MsgError_t;

//s2c c2s
typedef struct{
	uint32_t senderId;
}MsgPing_t;

//#pragma pack(pop)


bool SendMes_Settings(ProtoTransportP pr, MsgSettings_t* msg);
bool SendMes_Start(ProtoTransportP pr, MsgStart_t* msg);
bool SendMes_SetPosition(ProtoTransportP pr, MsgSetPosition_t* msg);
bool SendMes_Synchronize(ProtoTransportP pr, MsgSynchronize_t* msg);

bool SendMes_Error(ProtoTransportP pr, MsgError_t* msg);
bool SendMes_Ping(ProtoTransportP pr, MsgPing_t* msg);


void OnMessageStartReceived(ProtoTransportP pr, MsgStart_t* msg);
void OnMessageSettingsReceived(ProtoTransportP pr, MsgSettings_t* msg);
void OnMessageSynchronizeReceived(ProtoTransportP pr, MsgSynchronize_t* msg);
void OnMessageSetPositionReceived(ProtoTransportP pr, MsgSetPosition_t* msg);
void OnMessagePingReceived(ProtoTransportP pr, MsgPing_t* msg);


#ifdef __cplusplus
}
#endif

#endif //MESSAGES_PROTO_H_
