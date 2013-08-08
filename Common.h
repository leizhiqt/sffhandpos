#ifndef COMMON

#define COMMON

#include "api.h"

#define USERNAME_LEN 17 //max 8 chinese

#define ANTIFAKECODE_LEN 17

#define QUERYTIME_LEN 21

char systime[20];

void WarningBeep(int type);

void GetSysTime(char* time);

char* filter(char *c1,char c2);

int input_date_time(int line,int maxNum,int minNum,int minValue,int maxValue, char *pPrompt,int *retValue);

int Modify_Time(void);
 
int Modify_Date(void);

#endif
