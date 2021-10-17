#ifndef M_TIMER_H_
#define M_TIMER_H_


#include "stdint.h"
#include "stdbool.h"
#include "string.h"


#ifdef __cplusplus 
extern "C" {
#endif

typedef uint32_t (*Timer_GetMillis)(void);

typedef struct{
	Timer_GetMillis getMillis;
	uint32_t startTime;
	uint32_t period;
}Timer_t;

typedef Timer_t* PtrTimer_t;

void Timer_Initialise(PtrTimer_t, Timer_GetMillis);
void Timer_Start(PtrTimer_t, uint32_t _periodMillisec);
void Timer_Reset(PtrTimer_t);
void Timer_Restart(PtrTimer_t); // перезапуск без люфта
bool Timer_IsElapsed(PtrTimer_t);


#ifdef __cplusplus 
}
#endif



#endif //M_TIMER_H_
