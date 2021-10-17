#ifndef _DEV_HELPER_H_
#define _DEV_HELPER_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */


uint32_t GetValueFromPrecent(uint8_t _precent, uint32_t _maxValue);
uint32_t GetPrecentFromValue(uint32_t _value, uint32_t _maxValue);


#if defined(__cplusplus)
}
#endif /* __cplusplus */


#endif //_DEV_HELPER_H_
