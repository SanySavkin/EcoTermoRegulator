#include "dev_helper.h"

uint32_t GetValueFromPrecent(uint8_t _precent, uint32_t _maxValue){
	return (_precent * _maxValue + 50)/100;
}

uint32_t GetPrecentFromValue(uint32_t _value, uint32_t _maxValue){
	if(_value > _maxValue)
		return 100;
	return (_value * 100) / _maxValue;
}
