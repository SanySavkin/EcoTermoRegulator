#include "cbuffer.h"
#include "string.h"
#include "stdint.h"

size_t minsize(size_t a, size_t b)
{
	return (a < b) ? a : b;
}

void CBufInit(CyclicBuffer* buf, void* buffer, size_t size) 
{
	buf->data = buffer;
	buf->rpos = 0;
	buf->wpos = 0;
	buf->size = size;
}

void CBufClear(CyclicBuffer* buf) 
{
	buf->rpos = 0;
	buf->wpos = 0;
}

size_t CBufGetDataSize(CyclicBuffer* buf) 
{
	if (buf->wpos < buf->rpos)
		return buf->size - buf->rpos + buf->wpos;
	else
		return buf->wpos - buf->rpos;
}

size_t CBufGetFreeSize(CyclicBuffer* buf) 
{
	if (buf->wpos < buf->rpos)
		return buf->rpos - 1 - buf->wpos;
	else
		return buf->rpos - 1 - buf->wpos + buf->size;
}

size_t CBufGetFreeSizeNonViolate(CyclicBuffer* buf, size_t wpos)
{
	if (buf->wpos < buf->rpos)
		return buf->rpos - 1 - buf->wpos;
	else
		return buf->rpos - 1 - buf->wpos + buf->size;
}

bool CBufIsEmpty(CyclicBuffer* buf) 
{
	return buf->wpos == buf->rpos;
}

bool CBufIsFull(CyclicBuffer* buf) 
{
	return (buf->wpos + 1 == buf->rpos) || (buf->rpos == 0 && buf->wpos + 1 == buf->size);
}


inline size_t GetWriteLimit(CyclicBuffer* buf){
	return (buf->rpos > 0)?buf->rpos - 1:buf->size - 1;
}

bool CBufWriteByte(CyclicBuffer* buf, char value)
{	
	size_t wpos = buf->wpos;
	size_t next = CBufWrapIfEnd(buf, wpos + 1);
	if (next == buf->rpos)
		return false;
	buf->data[wpos] = value;
	buf->wpos = next;
	return true;
}

int CBufReadByte(CyclicBuffer* buf)
{
	size_t rpos = buf->rpos;
	if (rpos == buf->wpos)
		return -1;
	int res = (unsigned char)buf->data[rpos];
	buf->rpos = CBufWrapIfEnd(buf, rpos + 1);
	return res;
}

size_t CBufWrite(CyclicBuffer* buf, const void* data, size_t size)
{
	const char* loc_data = (const char*)data;
	size_t wpos = buf->wpos;// fix wpos
	size = minsize(size, CBufGetFreeSizeNonViolate(buf, wpos));
	size_t res = size;
	size_t cont_size = buf->size - wpos;
	if (size >= cont_size) 
	{
		memcpy((char*)buf->data + wpos, loc_data, cont_size);
		loc_data += cont_size;
		size -= cont_size;
		wpos = 0;
	}
	if (size > 0) {
		memcpy((char*)buf->data + wpos, loc_data, size);
		wpos += size;
	}
	buf->wpos = wpos;
	return res;
}

size_t CBufRead(CyclicBuffer* buf, void* data, size_t size)
{
	char* loc_data = (char*)data;
	size = minsize(size, CBufGetDataSize(buf));
	size_t res = size;
	size_t cont_size = buf->size - buf->rpos;
	if (size >= cont_size) 
	{
		memcpy(loc_data, buf->data + buf->rpos, cont_size);
		loc_data += cont_size;
		size -= cont_size;
		buf->rpos = 0;
	}
	if (size > 0) {
		memcpy(loc_data, buf->data + buf->rpos, size);
		buf->rpos += size;
	}
	return res;
}

//bool CBufReserveAtomic(CyclicBuffer* buf, size_t size, CBufReserveMem* mem)
//{
//	size_t new_wpos;
//	size_t wpos;
//	bool bOk;
//	do {
//#if (__CORTEX_M >= 0x03U)
//		wpos = __LDREXW(&buf->wpos);
//#else
//		wpos = buf->wpos;
//#endif
//		size_t free_size = (wpos < buf->rpos)? buf->rpos - 1 - wpos: buf->size - wpos + buf->rpos - 1;

//		bOk = size <= free_size;
//		if (bOk)
//			new_wpos =  CBufWrapIfOverflow(buf, wpos + size);
//		else
//			new_wpos = wpos;
//	}
//#if (__CORTEX_M >= 0x03U)
//	while (__STREXW(new_wpos, &buf->wpos) && bOk);
//#elif defined(_WIN64)
//	while (InterlockedCompareExchange64(&buf->wpos, new_wpos, wpos) != wpos && bOk);
//#elif defined(_WIN32)
//	while (InterlockedCompareExchange(&buf->wpos, new_wpos, wpos) != wpos && bOk);
//#else
//	while (buf->wpos = new_wpos, false);
//	buf->wpos = new_wpos;
//#endif
//	if (bOk)
//	{
//		mem->data = buf->data;
//		mem->pos = wpos;
//		mem->end = new_wpos;
//		mem->size = buf->size;
//	}
//	return bOk;
//}


bool CBufGetDataForRead(CyclicBuffer* buf, void** data, size_t* size)
{
	if (buf->rpos == buf->wpos) 
		return false;
	
	*data = (void*)(buf->data + buf->rpos);
	if (buf->wpos < buf->rpos)
		*size = buf->size - buf->rpos;
	else
		*size = buf->wpos - buf->rpos;
	return true;
}

bool CBufMarkAsRead(CyclicBuffer* buf, size_t size)
{
	size_t data_size = CBufGetDataSize(buf);
	if (size > data_size)
			return false;
	buf->rpos = CBufWrapIfOverflow(buf, buf->rpos + size);
	return true;
}

bool CBufGetBufForWrite(CyclicBuffer* buf, void** data, size_t* size)
{
	size_t end_pos = GetWriteLimit(buf);
	if (buf->wpos == end_pos)
		return false;

	*data = buf->data + buf->wpos;
	if (end_pos < buf->wpos)
		*size = buf->size - buf->wpos;
	else
		*size = end_pos - buf->wpos;
	return true;
}

bool CBufMarkAsWritten(CyclicBuffer* buf, size_t size)
{
	size_t free_size = CBufGetFreeSize(buf);
	if (size > free_size)
		return false;
	buf->wpos = CBufWrapIfOverflow(buf, buf->wpos + size);
	return true;
}


size_t CBufMoveAll(CyclicBuffer* dest, CyclicBuffer* src)
{
	size_t bytesLeft = minsize(CBufGetFreeSize(dest), CBufGetDataSize(src));
	size_t result = bytesLeft;
	while (bytesLeft > 0) {
		size_t blockSize = minsize(bytesLeft, minsize(dest->size - dest->wpos, src->size - src->rpos));
		memcpy((char*)dest->data + dest->wpos, (char*)src->data + src->rpos, blockSize);
		dest->wpos = CBufWrapIfEnd(dest, dest->wpos + blockSize);
		src->rpos = CBufWrapIfEnd(src, src->rpos + blockSize);
		bytesLeft -= blockSize;
	}
	return result;
}

const char* FindDataBlock(const char* buf, const char* buf_end, const char* data, size_t size)
{
	while (buf < buf_end)
	{
		buf = memchr(buf, data[0], buf_end - buf);
		if (buf == NULL)
			return buf_end;
		size_t len = minsize(buf_end - buf, size);
		if (memcmp(buf, data, len) == 0)
			return buf;
		buf++;
	}
	return buf_end;
}

bool CBufSkipUntil(CyclicBuffer* buf, const char* data, size_t size)
{
	size_t wpos = buf->wpos;
	if (wpos < buf->rpos)
	{
		const char* buf_end = buf->data + buf->size;
		const char* p = FindDataBlock(buf->data + buf->rpos, buf_end, data, size);
		size_t buf_tail = buf_end - p;
		if (buf_tail >= size)
		{
			buf->rpos = CBufWrapIfEnd(buf, p - buf->data + size);
			return true;
		}

		while (p < buf_end)
		{
			size_t data_tail = size - buf_tail;
			size_t len = minsize(wpos, data_tail);
			if (memcmp(buf->data, data + buf_tail, len) == 0)
			{
				buf->rpos = len;
				return wpos >= data_tail;
			}
			p = FindDataBlock(p, buf_end, data, size);
		}
	}
	const char* buf_end = buf->data + wpos;
	const char* p = FindDataBlock(buf->data, buf_end, data, size);
	if (p + size <= buf_end)
	{
		buf->rpos = p + size - buf->data;
		return true;
	}
	buf->rpos = p - buf->data;
	return false;
}

bool CBufSkipUntilTxt(CyclicBuffer* buf, const char* data)
{
	return CBufSkipUntil(buf, data, strlen(data));
}

size_t CBufWrapIfEnd(CyclicBuffer* buf, size_t pos) {
	return (pos == buf->size) ? 0 : pos;
}

size_t CBufWrapIfOverflow(CyclicBuffer* buf, size_t pos) {
	return (pos < buf->size) ? pos : pos - buf->size;
}

size_t CBufWrapIfOverflowNeg(CyclicBuffer* buf, size_t pos) {
	return ((intptr_t)pos >= 0) ? pos : pos + buf->size;
}

