/****************************************************************************
*
*文件(File):         Common.c
*
*修改(Modify):       2012-7-16 8:35:46
*
*作者(Author):       
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
| V1.00  2012-7-16 8:35:46            
----------------------------------------------------------------------------
****************************************************************************/
#include "Common.h"

char systime[40];

/*
//大字体
int  g_LINESpacing = 2;
int g_DisplyLine = 9;
*/

//存放系统参数
SysPara SysObj;

int g_LINESpacing = 1;
int g_DisplyLine = 19;
int AutoPowerOff = 0;

//======================================================================
//函数名：WaringBeep 
//功能  ：提示音 
//参数  ：int 
//返回值：无 
//======================================================================
void	WarningBeep(int type){
	int	count = 0;
	int	j = 0;
	
	if(type)
	{		
		for(count=0;count<3;count++)
		{	
			for(j=0;j<20;j++)
			{
				Beep_Sound(BEEP_ON,6,50);
			}
			
			Sys_Delay_MS(200);
			Beep_Sound(BEEP_OFF,6,50);	
			Sys_Delay_MS(50);
		}		
	}
	else
	{	
		for(j=0;j<1000;j++)
		{
			Beep_Sound(BEEP_ON,6,50);
		}
		Sys_Delay_MS(200);
		Beep_Sound(BEEP_OFF,6,50);
	}
}

void GetSysTime(char* time){
	typ_DATE_TIME sysnow;
	RTC_Get_DateTime(&sysnow);
	sprintf(time,"%d-%d-%d %d:%d:%d",sysnow.year,sysnow.month,sysnow.day,sysnow.hour,sysnow.min,sysnow.sec);
}

//======================================================================
// Name：Modify_Date
// Function：RTC Demo
// Parameters：None	
// Return：0		success
//	  other		failure
//======================================================================
/*
 int 	Modify_Date(void)
{
	typ_DATE	tdate;
	int ret = 0;
	int year = 0;
	int	month = 0;
	int	day = 0;
	int day_max = 0;
	char dBuf[40];
	char str1[20];
	char	str2[20];
	char	str3[20];
	
	RTC_Get_Date(&tdate);
	year = tdate.year;
	month = tdate.month;
	day = tdate.day;
		
	strcpy(str1,"年:");
	strcpy(str2,"月:");
	strcpy(str3,"日:");
		
	while(1)
	{
		sprintf(dBuf,"%04d/%02d/%02d",year,month,day);
		DispStr_CE(0,0,dBuf,DISP_CENTER|DISP_CLRSCR);
		DispStr_CE(0,2,str1,DISP_POSITION);
		DispStr_CE(0,4,str2,DISP_POSITION);
		DispStr_CE(0,6,str3,DISP_POSITION);
		
		ret = input_date_time(2,4,4,1980,2100,str1,&year);
		if(ret == -1)
		{
			return ret;
		}
		
		ret = input_date_time(4,2,1,1,12,str2,&month);	
		if(ret == -1)
		{
			continue;
		}
		
		//判断每个月的天数
		//Set the maximum days of every month
		switch(month)
		{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				day_max = 31;
				break;
				
			case 4:
			case 6:
			case 9:
			case 11:
				day_max = 30;
				break;
			
			case 2:
				if(  ( ((year%4)==0)&&(year%100!=0) )
				   ||( (year%400)==0 )
				  )
				{
					day_max = 29;
				}
				else
				{
					day_max = 28;
				}
				
				break;
				
			default:
				return -1;
		}
		
		ret = input_date_time(6,2,1,1,day_max,str3,&day);	
		if(ret != -1)
		{
			tdate.year = year;
			tdate.month = month;
			tdate.day = day;
			tdate.week = RTC_get_week(year,month,day);
			RTC_Set_Date(&tdate);
			
			sprintf(dBuf,"%04d/%02d/%02d",year,month,day);
			DispStr_CE(0,2,dBuf,DISP_CENTER|DISP_CLRSCR);
			
			delay_and_wait_key( 2, EXIT_KEY_ALL, 0 );
			
			return 0;
		}
	}
}


//======================================================================
// Name：Modify_Time
// Function：RTC Demo
// Parameters：None	
// Return：0		success
//	  other		failure
//======================================================================
 int Modify_Time(void)
{
	typ_TIME	ttime;
  int	ret = 0;
	int hour = 0;
	int	minute = 0;
	int	second = 0;
	char	dBuf[40];	
	char str1[20];
	char str2[20];
	char str3[20];
	
	
	RTC_Get_Time(&ttime);
	hour = ttime.hour;
	minute = ttime.min;
	second = ttime.sec;
		
	strcpy(str1,"时:");
	strcpy(str2,"分:");
	strcpy(str3,"秒:");
		
	while(1)
	{
		sprintf(dBuf,"%02d:%02d:%02d",ttime.hour,ttime.min,ttime.sec);
		DispStr_CE(0,0,dBuf,DISP_CENTER|DISP_CLRSCR);
		DispStr_CE(0,2,str1,DISP_POSITION);
		DispStr_CE(0,4,str2,DISP_POSITION);
		DispStr_CE(0,6,str3,DISP_POSITION);
		
		ret = input_date_time(2,2,1,0,23,str1,&hour);
		if(ret == -1)
		{
			return ret;
		}
		
		ret = input_date_time(4,2,1,0,59,str2,&minute);	
		if(ret == -1)
		{
			continue;
		}
		
		ret = input_date_time(6,2,1,0,59,str3,&second);	
		if(ret != -1)
		{
			ttime.hour = hour;
			ttime.min = minute;
			ttime.sec = second;			
			RTC_Set_Time(&ttime);
			
			sprintf(dBuf,"%02d:%02d:%02d",hour,minute,second);						
			DispStr_CE(0,2,dBuf,DISP_CENTER|DISP_CLRSCR);
			
			delay_and_wait_key( 2, EXIT_KEY_ALL, 0 );
			
			return 0;
		}
	}
}

//======================================================================
// Name：input_date_time
// Function：input date time value
// Parameters：None	
// Return：0		success
//	  other		failure
//======================================================================
int	input_date_time(int line,int maxNum,int minNum,int minValue,int maxValue,
	                char *pPrompt,int *retValue)
{
	GETSTR_PARAM 	gs;
	char	dBuf[40];
	int ret = 0;
	
	sprintf(dBuf,"%d",*retValue);
	gs.qx = 0;
	gs.sFont = 0;
	gs.alpha = 0;
	gs.nullEnable = 1;
	gs.csChar = '_';
	gs.pwChar = 0;
	gs.spChar = 0;
	gs.pStr = dBuf;
	gs.qEvent = 0;

	gs.qy = line;
	gs.maxNum = maxNum;
	gs.minNum = minNum;
	gs.minValue = minValue;
	gs.maxValue = maxValue;
	gs.pPrompt = pPrompt;
	gs.retNum = strlen((char*)dBuf);
	gs.qEvent = EXIT_KEY_F1|EXIT_KEY_CANCEL;
	
	ret = GetStr_E(&gs);
	if(ret==0)
	{
		*retValue = gs.retValue;
	}
	
	return ret;
}
*/

char* filter(char *c1,char c2){

	int i = 0, a = 0, s = 0;
	char c[1024];

	a=strlen(c1);
	if(!(a<1024))
		a=1023;

	for(i=0;i<a;i++)
		if(c2!=c1[i])	{
			c[s]=c1[i];
			s++;
	}

	c[s]='\0'; 
	for(i=0;i<=s;i++)
		c1[i]=c[i];

	return c1;
}

//函数功能:
//	显示函数
//输入函数:
//	unsigned int x, 
//	unsigned int y
//		信息显示开始信息
//	char	*str,
//		要显示的字符串  
//	unsigned int  bgc,
//		背景色
//	unsigned int  fc
//		前景色
//	int xyFlag
//		对齐方式
//函数返回:
//	void
void DispStr_CEEX(unsigned int x, unsigned int y, 
												char	*str, unsigned int  bgc, unsigned int  fc, int xyFlag)
{
	int x1 = 0;
	int Len = 0;
	
	x = x;
	if(xyFlag == DISP_CENTER)
	{
		Len = strlen(str);
		x1 = ((30 - Len) / 2) * (240 / 30);
	}
	Disp_Goto_XY(x1, y);
	Disp_Write_Str_Col((unsigned char *)str, bgc, fc);
	return;
}


/******************************************************************************************
Description:Browse menus used for EH0518-A
Parameter:	Input:	s_line -- starting line of the screen 
					p_menu -- menus will display
					p_cPtr -- current selected line 
					p_lPtr -- starting line of the menus
Return:		None
*******************************************************************************************/
int browse_menu(int s_line, char *p_menu,int *p_cPtr,int *p_lPtr, int lineMax, int DisplyLine)
{
	BROWINFO	bi;
	int ret;
	
	//if(s_line>3) s_line = 0;
	bi.startLine = s_line;       					// 屏幕显示的起始行
	bi.dispLines = DisplyLine;						// 屏幕显示的行数
	bi.iStr = p_menu;
	bi.mInt = strlen(p_menu)/lineMax;
	bi.lineMax = lineMax;
	bi.sFont = 0;
	bi.numEnable = 0;								// 允许键盘‘2’‘8’代替拨轮上下
	bi.qEvent = EXIT_KEY_ALL|EXIT_AUTO_QUIT|EXIT_KEY_OK;		// 导致函数退出的事件标志
	bi.autoexit = 30;								// 自动退出时间
	bi.cPtr = *p_cPtr;             		// 当前行 相对于画面来说的,画面的第一行为0
	bi.lPtr = *p_lPtr;								// 菜单显示的起始行
	ret = brow_select(&bi);
	*p_cPtr = bi.cPtr;
	*p_lPtr = bi.lPtr;
	if(ret < 0)
	{
		if(bi.qEvent==EXIT_KEY_POWER) return -1;
		if((bi.qEvent==EXIT_KEY_F1) || (bi.qEvent==(long)EXIT_KEY_CANCEL)) return -2;
		if(bi.qEvent==EXIT_KEY_F2) return -3;
		if(bi.qEvent==EXIT_KEY_F3) return -4;
		if(bi.qEvent==EXIT_KEY_CANCEL) return -4;
		if(bi.qEvent==EXIT_KEY_COMM) return -5;
		if(bi.qEvent==EXIT_AUTO_QUIT) return -6;
		if(bi.qEvent==EXIT_KEY_F4) return -7;				
		if(bi.qEvent==EXIT_KEY_ENTER)return -8;				
		if(bi.qEvent==EXIT_KEY_0) return 0;		
		if(bi.qEvent==EXIT_KEY_1) return 1;
		if(bi.qEvent==EXIT_KEY_2) return 2;
		if(bi.qEvent==EXIT_KEY_3) return 3;
		if(bi.qEvent==EXIT_KEY_4) return 4;
		if(bi.qEvent==EXIT_KEY_5) return 5;
		if(bi.qEvent==EXIT_KEY_6) return 6;
		if(bi.qEvent==EXIT_KEY_7) return 7;
		if(bi.qEvent==EXIT_KEY_8) return 8;
		if(bi.qEvent==EXIT_KEY_9) return 9;
		if(bi.qEvent==EXIT_AUTO_QUIT) Sys_Power_Sleep(3);
	}
	return ret;
}


short browse_info(int startline,char *p_menu,int *p_cPtr,int *p_lPtr,short flag ){
	BROWINFO	bi;
	short	ret = 0;

	if(flag ==0){
		Disp_Clear();
		DispStr_CE(0,0,"本批次提交记录信息如下:",DISP_CENTER);
		KEY_Flush_FIFO();
		DispStr_CE(0,36,"【F1退出提交】【F3确认提交】",DISP_CLRLINE|DISP_CENTER);
	}else if(flag ==1){
		Disp_Clear();
		DispStr_CE(0,0,"错误记录信息如下",DISP_CENTER);
		DispStr_CE(0,36,"【F3退出浏览】",DISP_RIGHT | DISP_CLRLINE);
	}

	if(startline>18) startline = 2;

	bi.startLine = startline;
	bi.dispLines = 18 - startline;
	bi.iStr = p_menu;
	bi.mInt = strlen(p_menu)/28;
	bi.lineMax = 28;
	bi.sFont = 0;
	bi.numEnable = 1;

	if(flag ==1){
		bi.qEvent = EXIT_KEY_F3;
	}else if(flag ==0){
		bi.qEvent = EXIT_KEY_F1|EXIT_KEY_F3;
	}

	bi.autoexit = 0;
	bi.cPtr = *p_cPtr;
	bi.lPtr = *p_lPtr;
	ret = brow_info( &bi );
	*p_cPtr = bi.cPtr;
	*p_lPtr = bi.lPtr;

	if(flag ==0) {
		if(bi.qEvent==EXIT_KEY_F1)
			return KEY_F1;

		if(bi.qEvent==EXIT_KEY_F3)
			return KEY_F3;
	}else if(flag ==1){
		if(bi.qEvent==EXIT_KEY_F3)
			return KEY_F3;
	}
	return 0;
}

void PackUpMenuData(char menu[], int MenuCount, int LineLen){
	int i = 0;
	menu[MenuCount * LineLen + 2] = '\0';
	for(i = 0; i < (MenuCount * LineLen + 1); ++i){
		if(menu[i] == '\0'){
			menu[i] = 0x20;
		}
	}
}

