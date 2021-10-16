#include "transport.h"



#define FRAME_SERVICE_LENGTH MESSAGE_ID_LENGTH

#define LOCK
#define UNLOCK

static uint8_t bufRx[256];


//private functions declaration begin 
static bool WriteToSendBuf(ProtoTransportP pr, uint8_t* _data, uint32_t _len);
static void Send(ProtoTransportP pr);
static void Receive(ProtoTransportP pr);
static void CheckTimeout(ProtoTransportP pr);
//private functions declaration end

void ProtoTransp_Initialise(ProtoTransportP pr, PrTrConfig_t* cnfg){
	pr->cnfg.numberResendAttempts = cnfg->numberResendAttempts;
	pr->cnfg.recTimeout = cnfg->recTimeout;
	pr->cnfg.sendTimeout = cnfg->sendTimeout;
}

void ProtoTransp_Process(ProtoTransportP pr){
	Send(pr);
	Receive(pr);
	CheckTimeout(pr);
}

bool ProtoTransp_Send(ProtoTransportP pr, uint32_t _idMsg, uint8_t* _msg, uint32_t _sizeMsg, void* _streamData, uint32_t _streamLen){
	uint32_t frameLen = FRAME_SERVICE_LENGTH + _sizeMsg + _streamLen;
	uint32_t idMsg = _idMsg;
	if(WriteToSendBuf(pr, (uint8_t*)&frameLen, sizeof(frameLen))){
		if(WriteToSendBuf(pr, (uint8_t*)&idMsg, sizeof(idMsg))){
			if(WriteToSendBuf(pr, _msg, _sizeMsg)){
				if(_streamData != NULL){
					if(WriteToSendBuf(pr, _streamData, _streamLen)){ 
						return true;
					}
				}
				else return true;
			}
		}
	}
	pr->itf.clearTx(pr);
	return false;
}

void ProtoTransp_ReceiveData(ProtoTransportP pr, uint8_t* _data, uint32_t _len){
	pr->tmt.prevRecTime = pr->itf.getMillis();
	pr->itf.writeRx(pr, _data, _len);
}

static bool WriteToSendBuf(ProtoTransportP pr, uint8_t* _data, uint32_t _len){
	if(pr->itf.getFreeTx(pr) >= _len){
		if(pr->itf.writeTx(pr, _data, _len) == _len)
			return true;
	}
	ProtoTransp_Error(pr, TRP_SEND_DATA_ERROR);
	return false;
}

static void Send(ProtoTransportP pr){
	if(pr->ctrl.isActiveTx || pr->itf.isEmptyTx(pr)) return;
	
	if(pr->itf.getDataForReadTx(pr, &pr->data, &pr->len)){
		pr->tmt.prevSendTime = pr->itf.getMillis();
		pr->ctrl.isActiveTx = true;
		pr->ctrl.resendCounter = 0;
		if(pr->data != NULL) pr->itf.send(pr, pr->data, pr->len);
	}
	else{
		pr->itf.clearTx(pr);
		ProtoTransp_Error(pr, TRP_READ_TX_BUFFER_ERROR);
	}
//	else{
//		if(pr->ctrl.resendCounter++ > pr->cnfg.numberResendAttempts){
//			pr->itf.clearTx(pr);
//			ProtoTransp_Error(pr, TRP_SEND_ABORTED);
//			SendAbort(pr);
//		}
//		else{
//			ProtoTransp_Error(pr, TRP_TX_PERIPHERY_ERROR);
//			pr->tmt.prevSendTime = pr->itf.getMillis();
//			if(pr->data != NULL) pr->itf.send(pr, pr->data, pr->len);
//		}
//	}
}

static void Receive(ProtoTransportP pr){
	uint32_t len;
	if(pr->itf.readRxNotDel(pr, (uint8_t*)&len, sizeof(len)) == sizeof(len)){
		if(len > sizeof(bufRx)){
			ProtoTransp_Error(pr, TRP_RX_BUF_SIZE_DEFICIENT);
			pr->itf.clearRx(pr);
			return;
		}
		if(pr->itf.getDataSizeRx(pr) >= len + sizeof(len)){
			pr->tmt.prevRecTime = pr->itf.getMillis();
			pr->itf.readRx(pr, bufRx, sizeof(len));
			if(pr->itf.readRx(pr, bufRx, len) == len){
				ProtoTransp_ReceiveCmpltClbk(pr, bufRx, len);
			}
			else {
				ProtoTransp_Error(pr, TRP_READ_RX_BUFFER_ERROR);
			}
		}
	}
}

static void CheckTimeout(ProtoTransportP pr){
	if(!pr->itf.isEmptyRx(pr) && pr->itf.getMillis() - pr->tmt.prevRecTime > pr->cnfg.recTimeout){
		pr->itf.clearRx(pr);
		ProtoTransp_Error(pr, TRP_RX_TIMEOUT);
	}
	if(pr->ctrl.isActiveTx && pr->itf.getMillis() - pr->tmt.prevSendTime > pr->cnfg.sendTimeout){
		pr->ctrl.isActiveTx = false;
		ProtoTransp_Error(pr, TRP_TX_TIMEOUT);
	}
}

//static void SendAbort(ProtoTransportP pr){
//	pr->ctrl.isActiveTx = false;
//	pr->ctrl.resendCounter = 0;
//}

volatile uint32_t debugKByteRate = 0;
static void CalculateKByteRate(ProtoTransportP pr, uint32_t _len){
	static uint32_t time = 0;
	static uint32_t lenSumPerSec = 0;
	if(pr->itf.getMillis() - time > 1000){
		debugKByteRate = lenSumPerSec >> 10;
		time = pr->itf.getMillis();
		lenSumPerSec = 0;
	}
	else {
		lenSumPerSec += _len;
	}
}

void ProtoTransp_TxCompleteClbk(ProtoTransportP pr, uint8_t* _buf, uint32_t* _len){
	CalculateKByteRate(pr, *_len);
	if(pr->data != NULL && pr->data == _buf){
		pr->ctrl.isActiveTx = false;
		pr->itf.markAsReadTx(pr, pr->len);
		pr->data = NULL;
	}
}

volatile uint32_t debugCountError = 0;
volatile uint32_t debugErrorCode = 0;
__weak void ProtoTransp_Error(ProtoTransportP pr, int32_t _errorCode){
	debugCountError++;
	debugErrorCode = _errorCode;
}

__weak void ProtoTransp_ReceiveCmpltClbk(ProtoTransportP pr, uint8_t* data, uint32_t _len){
	
}
