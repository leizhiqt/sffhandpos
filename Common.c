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

char systime[20];

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








