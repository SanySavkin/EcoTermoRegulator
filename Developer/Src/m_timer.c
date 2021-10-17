#include "m_timer.h"



void Timer_Initialise(PtrTimer_t _tm, Timer_GetMillis _getMillis){
	_tm->getMillis = _getMillis;
	_tm->period = 0;
	_tm->startTime = 0;
}

void Timer_Start(Timer_t* _tm, uint32_t _periodMillisec){
	_tm->startTime = _tm->getMillis();
	_tm->period = _periodMillisec;
}

void Timer_Reset(Timer_t* _tm){
	_tm->startTime = _tm->getMillis();
}

//Для правильной работы эта функция должна вызываться только если Timer_IsElapsed() == true; 
void Timer_Restart(PtrTimer_t _tm){
	_tm->startTime += _tm->period;
}

bool Timer_IsElapsed(Timer_t* _tm){
	uint32_t current = _tm->getMillis();
	
	if(current >= _tm->startTime){
		return current - _tm->startTime >_tm->period;
	}
	else{
		return UINT32_MAX - _tm->startTime + current > _tm->period;
	}
}
