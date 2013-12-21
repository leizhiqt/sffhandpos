#ifndef COMMON
#define COMMON

#include "api.h"

//系统的参数结构
typedef struct 
{
	//sim卡类型
	// 0 移动GPRS卡 
	// 1 联通GPRS卡 
	//默认值 0
	int SimCard;
}SysPara;

extern SysPara SysObj;
extern int g_LINESpacing;
extern int g_DisplyLine;
extern int AutoPowerOff;
extern char systime[40];

//颜色定义

//纯黑
#define BLACK 0x0000

//深蓝
#define NAVY 0x0010

//深红
#define MAROON 0x8000

//浅蓝 
#define BLUE 0x001F

//大红
#define RED 0xF800

//品红
#define FUCHSIA 0xF81F

//亮白  
#define WHITE 0xFFFF   

void WarningBeep(int type);
void GetSysTime(char* time);
char* filter(char *c1,char c2);
//int input_date_time(int line,int maxNum,int minNum,int minValue,int maxValue, char *pPrompt,int *retValue);
//int Modify_Time(void);
//int Modify_Date(void);
void DispStr_CEEX(unsigned int x, unsigned int y, char	*str, unsigned int  bgc, unsigned int  fc, int xyFlag);
int browse_menu(int s_line, char *p_menu,int *p_cPtr,int *p_lPtr, int lineMax, int DisplyLine);
short browse_info(int startline,char *p_menu,int *p_cPtr,int *p_lPtr,short flag);
void PackUpMenuData(char menu[], int MenuCount, int LineLen);

#endif
