
#include "API.h"

#ifndef __SF_HEAD__
#define __SF_HEAD__

short browse_info(int startline,char *p_menu,int *p_cPtr,int *p_lPtr,short flag);
void PackUpMenuData(char menu[], int MenuCount, int LineLen);
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
