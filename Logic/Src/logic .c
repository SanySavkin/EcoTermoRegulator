#include "logic.h"
#include "main.h"
#include "Developer/Src/dev_helper.h"
#include "ProtoCommunication\Src\messages_proto.h"
#include "Developer/Src/m_timer.h"



//зависит от требуемого сигнала на входе управления двигателем и используется для установки минимальной и максимальной скважности.
#define MIN_VALUE_ARR 230		
#define MAX_VALUE_ARR 550 //999	//must be equal ARR * n, где 0 < n <= 1


#define MOTOR_DIAPASONE 180	//Градусов

#define MIN_VALUE_POSITION 0						//минуты
#define MAX_VALUE_POSITION (MOTOR_DIAPASONE * 60)	//минуты


//typedef enum{
//	WAIT_START,
//	STARTING,
//	RUN,
//	
//}RegulatorState_t;


extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

static Regulator_t r = {0};
static Timer_t timerDurationOn;
static Timer_t timerPeriod;
static Timer_t timerActiveTime;
static Timer_t timerValveProcess;
static Timer_t timerCorrection;
volatile static uint32_t milliseconds = 0;
volatile int32_t correctValue = 0;

static uint8_t valveCurPos = 0; //100% - full open, 0% - full closed
static uint8_t valveSetPos = 0; //100% - full open, 0% - full closed


static uint32_t GetMilliseconds(void){
	return milliseconds;
}

static void SetPositionByMinutes(uint16_t _minutes){
	if(_minutes > MAX_VALUE_POSITION) _minutes = MAX_VALUE_POSITION;
	uint32_t precent = GetPrecentFromValue(_minutes, MAX_VALUE_POSITION);
	uint32_t val = MIN_VALUE_ARR + GetValueFromPrecent(precent, MAX_VALUE_ARR - MIN_VALUE_ARR);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, val);
}

//100% - full open, 0% - full closed
static void SetPositionByPrecent(uint8_t _prec){
	uint16_t min = r.set.angleOff > r.set.angleOn ? r.set.angleOn : r.set.angleOff;
	uint16_t max = r.set.angleOff > r.set.angleOn ? r.set.angleOff : r.set.angleOn;
	
	uint16_t maxArr = MIN_VALUE_ARR + (max * (MAX_VALUE_ARR - MIN_VALUE_ARR))/MAX_VALUE_POSITION;
	uint16_t minArr = MIN_VALUE_ARR + (min * (MAX_VALUE_ARR - MIN_VALUE_ARR))/MAX_VALUE_POSITION;
	
	uint32_t val = minArr + GetValueFromPrecent(_prec, maxArr - minArr);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, val);
}


//установка позиции клапана в процентах
//100% - full open, 0% - full closed
static void SetValve(uint8_t _precent){
	if(_precent > 100) _precent = 100;
	valveSetPos = _precent;
}
//полностью открыть
static void OpenValve(void){
	//SetValve(100);
}
//полностью закрыть
static void CloseValve(void){
	//SetValve(0);
}

static void Work(void){
	if(Timer_IsElapsed(&timerActiveTime)) {
		CloseValve();
		r.isActive = false;
		return;
	}
	
	if(Timer_IsElapsed(&timerDurationOn)){
		CloseValve();
	}
	
	if(Timer_IsElapsed(&timerPeriod)) {
		OpenValve();
		Timer_Reset(&timerDurationOn);
		Timer_Restart(&timerPeriod);
	}
}

static void ValvePositionProcess(void){
	if(valveCurPos != valveSetPos && Timer_IsElapsed(&timerValveProcess)){
		if(valveCurPos > valveSetPos) 
			valveCurPos--; 
		else valveCurPos++;
		Timer_Reset(&timerValveProcess);		
		SetPositionByPrecent(valveCurPos);
		
//		uint16_t range = r.set.angleOff > r.set.angleOn ? r.set.angleOff - r.set.angleOn : r.set.angleOn - r.set.angleOff;
//		uint16_t minutes = GetValueFromPrecent(valveCurPos, range);
//		uint16_t minimum = r.set.angleOff > r.set.angleOn ? r.set.angleOn : r.set.angleOff;
//		SetPosition(minimum + minutes);
	}
}

static void CorrectTime(void){
	if(Timer_IsElapsed(&timerCorrection)){
		correctValue = r.set.correctValue;
		Timer_Restart(&timerCorrection);
	}
}

void Logic_Process(void){
	ValvePositionProcess();	
	if(!r.isActive) {
		if(r.set.isOpenWhenStopped) OpenValve();
		else CloseValve();
		return;
	};	
	Work();
	CorrectTime();
}

void Logic_Initialise(void){
	Timer_Initialise(&timerDurationOn, GetMilliseconds);
	Timer_Initialise(&timerPeriod, GetMilliseconds);
	Timer_Initialise(&timerActiveTime, GetMilliseconds);
	Timer_Initialise(&timerValveProcess, GetMilliseconds);
	Timer_Initialise(&timerCorrection, GetMilliseconds);
	Timer_Start(&timerValveProcess, 2);
	
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

void Logic_IncrementTime(void){
	milliseconds++;
}

int32_t Logic_GetCorrectValue(void){
	return correctValue;
}

void Logic_CorrectTimeCmpltClbk(void){
	correctValue = 0;
}

static void RestartTimers(void){
	HAL_TIM_Base_Stop_IT(&htim3);
	__HAL_TIM_SetCounter(&htim3, 0);
	HAL_TIM_Base_Start_IT(&htim3);
}

void OnMessageStartReceived(ProtoTransportP pr, MsgStart_t* msg){
	r.isActive = msg->isStart;
	if(r.isActive){
		Timer_Start(&timerActiveTime, r.set.activeTime);
		Timer_Start(&timerCorrection, r.set.correctPer);
	}
}

void OnMessageSettingsReceived(ProtoTransportP pr, MsgSettings_t* msg){
	if(msg->correctValue > -900 && msg->correctValue < 900)
		r.set.correctValue = msg->correctValue;
	else r.set.correctValue = 0;
	
	r.set.activeTime = msg->activeTime;
	r.set.cycleCount = msg->cycleCount;
	r.set.delayAfter = msg->delayAfter;
	r.set.delayBefore = msg->delayBefore;
	r.set.durationOn = msg->durationOn;
	r.set.period = msg->period;
	r.set.angleOn = msg->angleOn;
	r.set.angleOff = msg->angleOff;
	r.set.valveStepPer = msg->valveStepPer;
	r.set.correctPer = msg->correctPer;
	r.set.isOpenWhenStopped = msg->isOpenWhenStopped;
	
	Timer_Start(&timerValveProcess, r.set.valveStepPer);
}

void OnMessageSynchronizeReceived(ProtoTransportP pr, MsgSynchronize_t* msg){
	RestartTimers();

	OpenValve();
	
	Timer_Start(&timerDurationOn, r.set.durationOn);
	Timer_Start(&timerPeriod, r.set.period);
	
	
	MsgSynchronize_t m = {.id = msg->id};
	SendMes_Synchronize(pr, &m);
}

void OnMessageSetPositionReceived(ProtoTransportP pr, MsgSetPosition_t* msg){
	SetPositionByMinutes(msg->position);
}

