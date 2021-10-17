#ifndef TRANSPORT_H_
#define TRANSPORT_H_

#include "string.h"
#include "stdint.h"
#include "stdbool.h"


#ifdef __cplusplus
extern "C" {
#endif

#define DATA_LEN_LENGTH 4
#define MESSAGE_ID_LENGTH 4

typedef enum{
	TRP_READ_RX_BUFFER_ERROR = 1,
	TRP_READ_TX_BUFFER_ERROR = 2,
	TRP_SEND_DATA_ERROR = 3,
	TRP_RX_TIMEOUT = 4,
	TRP_TX_TIMEOUT = 5,
	TRP_TX_PERIPHERY_ERROR = 6,
	TRP_RX_BUF_SIZE_DEFICIENT = 7,
	TRP_SEND_ABORTED = 8,
}TranspErrorCode_t;

struct ProtoTransport;
typedef struct ProtoTransport* ProtoTransportP;

typedef bool (*PrTr_Send)(ProtoTransportP, uint8_t* data, uint32_t _len);
typedef uint32_t (*PrTr_GetMillis)(void);
typedef int32_t (*PrTr_BufferRW)(ProtoTransportP, uint8_t* data, uint32_t _len); //function for reading and writing intermediate buffers. Must return the number of reading/writing bytes.
typedef bool (*PrTr_BufferGetData)(ProtoTransportP, uint8_t** data, uint32_t* _len);
typedef void (*PrTr_BufferMarkAsRead)(ProtoTransportP, uint32_t _len);
typedef uint32_t (*PrTr_GetBuferInfo)(ProtoTransportP);
typedef bool (*PrTr_BuferIsEmpty)(ProtoTransportP);
typedef void (*PrTr_ClearBuffer)(ProtoTransportP);
typedef bool (*PrTr_Mutex)(ProtoTransportP, bool _lock);


typedef struct {
	uint32_t prevRecTime;
	uint32_t prevSendTime;
}PrTrTimeout_t;

typedef struct{
	bool isActiveTx;
	uint32_t resendCounter;
}PrTrController_t;

typedef struct{
	uint32_t recTimeout;
	uint32_t sendTimeout;
	uint32_t numberResendAttempts;
}PrTrConfig_t;

typedef struct{
	PrTr_Send send; //Send data to periphery
	PrTr_GetMillis getMillis; // get milliseconds
	PrTr_BufferRW readRx; //read Rx buffer
	PrTr_BufferRW writeRx; //write Rx buffer
	PrTr_BufferMarkAsRead markAsReadTx; //fake read Tx buffer (mark as read)
	PrTr_BufferRW writeTx; //write Tx buffer
	PrTr_BufferGetData getDataForReadTx; //data to be read are sequentially located in the tx buffer
	PrTr_BufferRW readRxNotDel; //read Rx buffer without deleting data
	PrTr_BufferRW readTxNotDel; //read Tx buffer without deleting data
	PrTr_GetBuferInfo getDataSizeRx; //data size RX
	PrTr_GetBuferInfo getDataSizeTx; //data size TX
	PrTr_GetBuferInfo getFreeRx; //free space RX
	PrTr_GetBuferInfo getFreeTx; //free space TX
	PrTr_BuferIsEmpty isEmptyRx; //is emty RX buffer
	PrTr_BuferIsEmpty isEmptyTx; //is emty TX buffer
	PrTr_ClearBuffer clearRx; //clear RX buffer
	PrTr_ClearBuffer clearTx; //clear TX buffer
	PrTr_Mutex locker;
}PrTrInterface_t;

typedef struct ProtoTransport{
	PrTrInterface_t itf;
	PrTrTimeout_t tmt;
	PrTrController_t ctrl;
	PrTrConfig_t cnfg;
	uint8_t* data;
	uint32_t len;
}ProtoTransport_t;


void ProtoTransp_Initialise(ProtoTransportP pr, PrTrConfig_t* cnfg);
void ProtoTransp_Process(ProtoTransportP);
bool ProtoTransp_Send(ProtoTransportP, uint32_t idMsg, uint8_t* msg, uint32_t sizeMsg, void* streamData, uint32_t streamLen);
void ProtoTransp_ReceiveCmpltClbk(ProtoTransportP, uint8_t* data, uint32_t len);
void ProtoTransp_Error(ProtoTransportP, int32_t errorCode);
void ProtoTransp_ReceiveData(ProtoTransportP, uint8_t* data, uint32_t len);
void ProtoTransp_TxCompleteClbk(ProtoTransportP pr, uint8_t* buf, uint32_t* len);

#ifdef __cplusplus
}
#endif

#endif //TRANSPORT_H_
