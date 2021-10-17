#ifndef LOGIC_H_
#define LOGIC_H_

#include "string.h"
#include "stdint.h"
#include "stdbool.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct{
	uint32_t delayBefore; 	//опережение закрытия клапана перед выходом счетчика из энергосберегающего режима.
	uint32_t delayAfter;	//задержка открытия клапана после входа счетчика в энергосберегающий режим.
	uint32_t durationOn;	//продолжительность открытого клапана.
	uint32_t cycleCount;	//число пропуска периодов (если 0, то открытие клапана каждый период).
	uint32_t period;		//периоде цикла, мсек;
	uint32_t activeTime;	//время работы, ms.
	uint16_t angleOn;		//угол, при котором клапан полностью открыт (минуты).
	uint16_t angleOff;		//угол, при котором клапан полностью закрыт (минуты).
	uint32_t correctPer;	//период коррекции времени, мс;
	int16_t correctValue;	//значение коррекции времени, мкс; MIN -900, MAX +900;
	uint8_t valveStepPer;	//период (в милисек) одного шага (в процентах) клапана; Необходимо для плавного нажатия и отжатия клапана;
	bool isOpenWhenStopped;	//положение клапана после истечения activeTime. if(isOpenWhenStopped) клапан открыт; else клапан закрыт.
}RegulatorSettings_t;

typedef struct{
	uint16_t position;
	bool isActive;
	uint32_t nextSwitchOnTime;
	RegulatorSettings_t set;
}Regulator_t;



void Logic_Process(void);
void Logic_Initialise(void);
void Logic_IncrementTime(void);
int32_t Logic_GetCorrectValue(void);
void Logic_CorrectTimeCmpltClbk(void);



#ifdef __cplusplus
}
#endif
	
	
#endif //LOGIC_H_
