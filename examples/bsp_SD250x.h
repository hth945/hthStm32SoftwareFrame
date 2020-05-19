#ifndef _SD250x_H__
#define _SD250x_H__	 

#include "skySoft_sd250x.h"

void SD250X_Init(void);
int SD250X_RTC_WriteDate(Time_Def *psRTC);
int SD250X_RTC_ReadDate(Time_Def *psRTC);

#endif

