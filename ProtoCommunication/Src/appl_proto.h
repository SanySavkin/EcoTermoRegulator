#ifndef APPL_PROTO_H_
#define APPL_PROTO_H_

#include "string.h"
#include "stdint.h"
#include "stdbool.h"
#include "ProtoCommunication/Src/transport.h"


#ifdef __cplusplus
extern "C" {
#endif


void ApplProto_Initialise(void);
void ApplProto_Process(void);
void ApplProto_TxCmpltClbk(ProtoTransportP pr);
void ApplProto_RxCmpltClbk(ProtoTransportP pr);
void ApplProto_ErrorClbk(ProtoTransportP pr);


#ifdef __cplusplus
}
#endif
	
	
#endif //APPL_PROTO_H_
