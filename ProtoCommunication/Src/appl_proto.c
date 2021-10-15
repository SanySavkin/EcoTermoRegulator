/**
В этом файле находится реализация интерфейсов ProtoTransport_t обьектов.
Определите в этом файле обьекты типа ProtoTransport_t и реализуйте для каждого из них свой интерфейс.
**/

#include "appl_proto.h"
#include "Developer/Src/cbuffer.h"
#include "main.h"


extern UART_HandleTypeDef huart1;
extern ProtoTransport_t protoUart;

static uint8_t tempBufUartReceive;
static CyclicBuffer cBufUartRx;
static uint8_t cBufBufferUsbRx[512];

static CyclicBuffer cBufUartTx;
static uint8_t cBufBufferUsbTx[512];

//private functions declaration begin
static void ProtoUsbInit(void);

static bool GetDataForRead(ProtoTransportP, uint8_t** data, uint32_t* len);
static int32_t WriteRx(ProtoTransportP pr, uint8_t* data, uint32_t len);
static int32_t WriteTx(ProtoTransportP pr, uint8_t* data, uint32_t len);
static int32_t ReadRxNotDel(ProtoTransportP pr, uint8_t* data, uint32_t len);
static int32_t ReadTxNotDel(ProtoTransportP pr, uint8_t* data, uint32_t len);
static int32_t ReadRx(ProtoTransportP, uint8_t* data, uint32_t len);
static uint32_t GetDataSizeRx(ProtoTransportP pr);
static uint32_t GetDataSizeTx(ProtoTransportP pr);
static uint32_t GetFreeTx(ProtoTransportP pr);
static uint32_t GetFreeRx(ProtoTransportP pr);
static bool IsEmptyRx(ProtoTransportP pr);
static bool IsEmptyTx(ProtoTransportP pr);
static bool Send(ProtoTransportP, uint8_t* data, uint32_t len);
static void MarkAsRead(ProtoTransportP, uint32_t len);
static void ClearRx(ProtoTransportP);
static void ClearTx(ProtoTransportP);
static uint32_t GetMillis(void);
static bool Mutex(ProtoTransportP, bool);
//private functions declaration end




void ApplProto_Initialise(void){
	ProtoUsbInit();
}
uint32_t data[10] = {0x12340, 0x37, 0x2C, 0x1};
void ApplProto_Process(void){
	ProtoTransp_Process(&protoUart);
}
volatile uint32_t debugDataCount = 0;
void ApplProto_OnUsbReceivedData(uint8_t* _data, uint32_t _len){
	debugDataCount += _len;
	ProtoTransp_ReceiveData(&protoUart, _data, _len);
}

void ApplProto_TxCmpltClbk(ProtoTransportP pr){
	ProtoTransp_TxCompleteClbk(pr, pr->data, &pr->len);
}

static void ProtoUsbInit(void){
	CBufInit(&cBufUartRx, cBufBufferUsbRx, sizeof(cBufBufferUsbRx));
	CBufInit(&cBufUartTx, cBufBufferUsbTx, sizeof(cBufBufferUsbTx));
	
	protoUart.itf.send = Send;
	protoUart.itf.getMillis = GetMillis;
	protoUart.itf.readRx = ReadRx;
	protoUart.itf.writeRx = WriteRx;
	protoUart.itf.markAsReadTx = MarkAsRead;
	protoUart.itf.writeTx = WriteTx;
	protoUart.itf.getDataForReadTx = GetDataForRead;
	protoUart.itf.readRxNotDel = ReadRxNotDel;
	protoUart.itf.readTxNotDel = ReadTxNotDel;
	protoUart.itf.getDataSizeRx = GetDataSizeRx;
	protoUart.itf.getDataSizeTx = GetDataSizeTx;
	protoUart.itf.getFreeRx = GetFreeRx;
	protoUart.itf.getFreeTx = GetFreeTx;
	protoUart.itf.isEmptyRx = IsEmptyRx;
	protoUart.itf.isEmptyTx = IsEmptyTx;
	protoUart.itf.clearRx = ClearRx;
	protoUart.itf.clearTx = ClearTx;
	protoUart.itf.locker = Mutex;
	
	PrTrConfig_t cnfg = {0};
	cnfg.numberResendAttempts = 3;
	cnfg.recTimeout = 1000;
	cnfg.sendTimeout = 2000;
	
	ProtoTransp_Initialise(&protoUart, &cnfg);
	
	HAL_UART_Receive_IT(&huart1, &tempBufUartReceive, 1);
}

//PrTrInterface_t interface implementation begin
static uint32_t GetMillis(void){
	return HAL_GetTick();
}

static int32_t ReadRx(ProtoTransportP pr, uint8_t* data, uint32_t len){
	CyclicBuffer* cBuf = NULL;
	if(pr == &protoUart) cBuf = &cBufUartRx;
	
	if(cBuf != NULL){
		return CBufRead(cBuf, data, len);
	}
	return 0;
}

static int32_t WriteRx(ProtoTransportP pr, uint8_t* data, uint32_t len){
	CyclicBuffer* cBuf = NULL;
	if(pr == &protoUart) cBuf = &cBufUartRx;
	
	if(cBuf != NULL){
		return CBufWrite(cBuf, data, len);
	}
	return 0;
}

static void MarkAsRead(ProtoTransportP pr, uint32_t len){
	CyclicBuffer* cBuf = NULL;
	if(pr == &protoUart) cBuf = &cBufUartTx;
	
	if(cBuf != NULL){
		CBufMarkAsRead(cBuf, len);
	}
}

static int32_t WriteTx(ProtoTransportP pr, uint8_t* data, uint32_t len){
	CyclicBuffer* cBuf = NULL;
	if(pr == &protoUart) cBuf = &cBufUartTx;
	
	if(cBuf != NULL){
		return CBufWrite(cBuf, data, len);
	}
	return 0;
}

static bool GetDataForRead(ProtoTransportP pr, uint8_t** data, uint32_t* len){
	CyclicBuffer* cBuf = NULL;
	if(pr == &protoUart) cBuf = &cBufUartTx;
	
	if(cBuf != NULL){
		return CBufGetDataForRead(cBuf, (void**)data, len);
	}
	return false;
}

static int32_t ReadRxNotDel(ProtoTransportP pr, uint8_t* data, uint32_t len){
	CyclicBuffer* cBuf = NULL;
	if(pr == &protoUart) cBuf = &cBufUartRx;
	
	if(cBuf != NULL){
		CyclicBuffer cBufTemp = {.data = cBuf->data, .rpos = cBuf->rpos, .wpos = cBuf->wpos, .size = cBuf->size};
		return CBufRead(&cBufTemp, data, len);
	}
	return 0;
}

static int32_t ReadTxNotDel(ProtoTransportP pr, uint8_t* data, uint32_t len){
	CyclicBuffer* cBuf = NULL;
	if(pr == &protoUart) cBuf = &cBufUartTx;
	
	if(cBuf != NULL){
		CyclicBuffer cBufTemp = {.data = cBuf->data, .rpos = cBuf->rpos, .wpos = cBuf->wpos, .size = cBuf->size};
		return CBufRead(&cBufTemp, data, len);
	}
	return 0;
}

static uint32_t GetDataSizeRx(ProtoTransportP pr){
	CyclicBuffer* cBuf = NULL;
	if(pr == &protoUart) cBuf = &cBufUartRx;
	
	if(cBuf != NULL){
		return CBufGetDataSize(cBuf);
	}
	return 0;
}

static uint32_t GetDataSizeTx(ProtoTransportP pr){
	CyclicBuffer* cBuf = NULL;
	if(pr == &protoUart) cBuf = &cBufUartTx;
	
	if(cBuf != NULL){
		return CBufGetDataSize(cBuf);
	}
	return 0;
}

static uint32_t GetFreeTx(ProtoTransportP pr){
	CyclicBuffer* cBuf = NULL;
	if(pr == &protoUart) cBuf = &cBufUartTx;
	
	if(cBuf != NULL){
		return CBufGetFreeSize(cBuf);
	}
	return 0;
}

static uint32_t GetFreeRx(ProtoTransportP pr){
	CyclicBuffer* cBuf = NULL;
	if(pr == &protoUart) cBuf = &cBufUartRx;
	
	if(cBuf != NULL){
		return CBufGetFreeSize(cBuf);
	}
	return 0;
}

static bool IsEmptyRx(ProtoTransportP pr){
	CyclicBuffer* cBuf = NULL;
	if(pr == &protoUart) cBuf = &cBufUartRx;
	
	if(cBuf != NULL){
		return CBufIsEmpty(cBuf);
	}
	return false;
}

static bool IsEmptyTx(ProtoTransportP pr){
	CyclicBuffer* cBuf = NULL;
	if(pr == &protoUart) cBuf = &cBufUartTx;
	
	if(cBuf != NULL){
		return CBufIsEmpty(cBuf);
	}
	return false;
}

static void ClearRx(ProtoTransportP pr){
	CyclicBuffer* cBuf = NULL;
	if(pr == &protoUart) cBuf = &cBufUartRx;
	
	if(cBuf != NULL){
		CBufClear(cBuf);
	}
}

static void ClearTx(ProtoTransportP pr){
	CyclicBuffer* cBuf = NULL;
	if(pr == &protoUart) cBuf = &cBufUartTx;
	
	if(cBuf != NULL){
		CBufClear(cBuf);
	}
}

static bool Send(ProtoTransportP pr, uint8_t* data, uint32_t len){
	if(pr == &protoUart){
		return HAL_UART_Transmit_DMA(&huart1, data, len) == HAL_OK;
	}
	return false;
}

static bool Mutex(ProtoTransportP pr, bool _lock){
	if(pr == &protoUart){
		if(_lock){
			//osMutexAcquire();
		}
		//else //osMutexRelease();
	}
	return false;
}
//PrTrInterface_t interface implementation end

void ApplProto_RxCmpltClbk(ProtoTransportP pr){
	pr->itf.writeRx(pr, &tempBufUartReceive, 1);
	HAL_UART_Receive_IT(&huart1, &tempBufUartReceive, 1);
}

void ApplProto_ErrorClbk(ProtoTransportP pr){
	HAL_UART_Abort_IT(&huart1);
	pr->itf.clearRx(pr);
	pr->itf.clearTx(pr);
	HAL_UART_Receive_IT(&huart1, &tempBufUartReceive, 1);
}
