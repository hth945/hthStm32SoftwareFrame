#ifndef _BSP_BUSSD2506_H_
#define _BSP_BUSSD2506_H_

#include "stdint.h"
#include "Public_SoftCan.h"

void BusSD2506_Init(can_adapter *adap);
int busRunProcess2(void);

#endif
