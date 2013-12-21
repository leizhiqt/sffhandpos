
#include "API.h"

#ifndef __SF_HEAD__
#define __SF_HEAD__

typedef struct 
{
	char year[5];
	char month[3];
	char day[3];
	char hour[3];
	char min[3];
	char sec[3];
	char RUF;
}Time;

short Display(short flag);
long Alert();
void InitSystem();
void Query();
short OpenGPRS();
void SubmitData();
void FormatDatabase();
void  SysSetMenu();
void GetInfo();
void MainMenu();

#endif
