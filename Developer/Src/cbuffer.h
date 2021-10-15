#ifndef _FORPOST_CYCLIC_BUFFER
#define _FORPOST_CYCLIC_BUFFER

#include "stddef.h"
#include "stdbool.h"

typedef struct _CyclicBuffer {
   char* data;
   size_t rpos;
   volatile size_t wpos;
   size_t size;
} CyclicBuffer;

typedef struct _CBufReserveMem {
	char* data;
	size_t pos;
	size_t end;
	size_t size; // buffer size
}CBufReserveMem;

#define CBUF_INIT(p) {(char*)p, 0, 0, sizeof(p)}

#ifdef __cplusplus
extern "C" {
#endif 

void CBufInit(CyclicBuffer* buf, void* buffer, size_t size);

void CBufClear(CyclicBuffer* buf);

size_t CBufGetDataSize(CyclicBuffer* buf);
size_t CBufGetFreeSize(CyclicBuffer* buf);

bool CBufIsEmpty(CyclicBuffer* buf);
bool CBufIsFull(CyclicBuffer* buf);

bool CBufWriteByte(CyclicBuffer* buf, char value);
int CBufReadByte(CyclicBuffer* buf);

size_t CBufWrite(CyclicBuffer* buf, const void* data, size_t size);
size_t CBufRead(CyclicBuffer* buf, void* data, size_t size);

bool CBufReserve(CyclicBuffer* buf, size_t size, void* stream);
bool CBufReserveAtomic(CyclicBuffer* buf, size_t size, CBufReserveMem* mem); /// INTERRUPT SAFE ALLOCATOR
void FillStream(void* _stream, char* _data, size_t _pos, size_t _end, size_t _size, bool _has_error); 

bool CBufGetDataForRead(CyclicBuffer* buf, void** data, size_t* size);
bool CBufMarkAsRead(CyclicBuffer* buf, size_t size);
bool CBufGetBufForWrite(CyclicBuffer* buf, void** data, size_t* size);
bool CBufMarkAsWritten(CyclicBuffer* buf, size_t size);

size_t CBufWrapIfEnd(CyclicBuffer* buf, size_t pos);
size_t CBufWrapIfOverflow(CyclicBuffer* buf, size_t pos);
size_t CBufWrapIfOverflowNeg(CyclicBuffer* buf, size_t pos);

size_t CBufMoveAll(CyclicBuffer* dest, CyclicBuffer* src);

bool CBufSkipUntil(CyclicBuffer* buf, const char* data, size_t size);
bool CBufSkipUntilTxt(CyclicBuffer* buf, const char* data);


#ifdef __cplusplus
}
#endif 

#endif //_FORPOST_CYCLIC_BUFFER
