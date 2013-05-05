/****************************************************************************
*
*文件(File):         Common.h
*
*修改(Modify):       2012-7-16 8:35:30
*
*作者(Author):       廈$g)B
*
*编绎(Compile):      智能平台(Smart Platform)
*
*描述(Description):
*
*
*
----------------------------------------------------------------------------
|
| Version | Datetime             |   Author    | Description
| --------+----------------------+-------------+----------------------------
|
| V1.00  2012-7-16 8:35:30           廈$g)B 
----------------------------------------------------------------------------
****************************************************************************/
#include "API.h"

#ifndef COMMON

#define COMMON

#define USERNAME_LEN 17 //max 8 chinese

#define ANTIFAKECODE_LEN 17

#define QUERYTIME_LEN 21

char systime[20];

void WarningBeep(int type);

void GetSysTime(unsigned char* time);

#endif
