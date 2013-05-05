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
#include"Common.h"
//======================================================================
//函数名：WaringBeep 
//功能  ：提示音 
//参数  ：int 
//返回值：无 
//======================================================================
void	WarningBeep(int type)
{
	int	count;
	int	j;
	
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
void GetSysTime(unsigned char* time)
{
    typ_DATE_TIME systime;
     RTC_Get_DateTime(&systime);
     sprintf(time,"%d-%d-%d %d:%d:%d",systime.year,systime.month,systime.day,systime.hour,systime.min,systime.sec);
   /*  DispStr_CE(0,0,"时间",DISP_CLRSCR); 
     DispStr_CE(0,4,time,DISP_CENTER|DISP_CLRSCR);
     delay_and_wait_key(0,EXIT_KEY_F2,0);
     */
     
}
 /*
int input_info(int index,char* indata,char* returndata)
{
        GETCSTR_PARAM	gsc;
	gsc.qx =0;
	gsc.qy = 24;
	gsc.defaultMode = 0;
	gsc.eDisable = 0;								// 0:允许英文输入
	gsc.nullEnable = 0;
	gsc.pStr = indata;
	gsc.csChar ='_';
	switch(index)
	{
             case 1:
             {
              gsc.minNum =4;
              gsc.maxNum = 4; 
                 break;
             }
             case 2:
             case 3:
             case 4:
             case 5:
             case 6:
             {
                 gsc.minNum =1;
                 gsc.maxNum = 2; 
                 break;
             }
	}
	gsc.retNum = strlen(indata);    	
	gsc.autoexit = 0;
	
        int err = GetStr_CEX(&gsc);
        
        if(err ==0)
        {
         strcpy(returndata,gsc.pStr);
         return 0;
        }
        else
        {
           return -1;
        }
} 

short SetSysTime()
{
       typ_DATE_TIME systime;
        typ_DATE_TIME settime;
       
     RTC_Get_DateTime(&systime);
     char time[35];
     memset(time,0,35);
     sprintf(time,"%d-%d-%d %d:%d:%d",systime.year,systime.month,systime.day,systime.hour,systime.min,systime.sec);
     DispStr_CE(0,0,"当前时间为:",DISP_CENTER|DISP_CLRSCR);
     DispStr_CE(0,2,time,DISP_CENTER);

     //年 
     char year[5];
     memset(year,0,5);
     sprintf(year,"%d",systime.year);
     //月
     char month[5];
     memset(month,0,5);
     sprintf(month,"%d",systime.month);
     //日 
     char day[5];
     memset(day,0,5);
     sprintf(day,"%d",systime.day);
         //小时
         char hour[5];
         memset(hour,0,5);
         sprintf(hour,"%d",systime.hour);
         //分
         char minute[5];
         memset(minute,0,5);
         sprintf(minute,"%d",systime.min);
         //秒
         char second[5];
         memset(second,0,5);
         sprintf(second,"%d",systime.sec);
         
          char returndata[5];
          
         int index =1;
         int err;
         while(index <=6)
         {
           memset(returndata,0,5);
           err =1;
           switch(index)
           {
             case 1:
             {
                 
                 err= input_info(index,year,returndata);
                 if(err == 0)
                 {    
                     systime.year= (unsigned short)atoi(returndata);
                     memset(year,0,5);
                     sprintf(year,"%d",systime.year);
                     Disp_Goto_XY(0,6);
                     DispStr_CE(0,6,"年：",DISP_CURRENT);
                     DispStr_CE(6,6,year,DISP_CURRENT);
                     
                    // delay_and_wait_key(0,EXIT_KEY_F2,0) ;
                 }
                break;
                
             }
              case 2:
             {
                err= input_info(index,month,returndata);
                 if(err == 0)
                 {
                    
                    systime.month= (unsigned char)atoi(returndata);
                     memset(month,0,5);
                     sprintf(month,"%d",systime.month);
                     Disp_Goto_XY(0,8);
                     DispStr_CE(0,8,"月：",DISP_CURRENT);
                     DispStr_CE(8,6,month,DISP_CURRENT);
                    // delay_and_wait_key(0,EXIT_KEY_F2,0) ;
                 }
                break;
                
             }
              case 3:
             {
                 err= input_info(index,day,returndata);
                 if(err == 0)
                 {
                       systime.day= (unsigned char)atoi(returndata);
                     memset(day,0,5);
                     sprintf(day,"%d",systime.day);
                     Disp_Goto_XY(0,10);
                     DispStr_CE(0,10,"日：",DISP_CURRENT);
                     DispStr_CE(10,6,day,DISP_CURRENT);
                    // delay_and_wait_key(0,EXIT_KEY_F2,0)   ;
                    }
                break;
               
             }
              case 4:
             {
                 err= input_info(index,hour,returndata);
                 if(err == 0)
                 {
                       systime.hour= (unsigned char)atoi(returndata);
                     memset(hour,0,5);
                     sprintf(hour,"%d",systime.hour);
                     Disp_Goto_XY(0,12);
                     DispStr_CE(0,12,"时：",DISP_CURRENT);
                     DispStr_CE(12,6,hour,DISP_CURRENT);
                    // delay_and_wait_key(0,EXIT_KEY_F2,0) ;
                    }
                break;
                
             }
              case 5:
             {
                 err= input_info(index,minute,returndata);
                 if(err == 0)
                 {
                    systime.min=(unsigned char)atoi(returndata);
                     memset(minute,0,5);
                     sprintf(minute,"%d",systime.min);
                     Disp_Goto_XY(0,14);
                     DispStr_CE(0,14,"分：",DISP_CURRENT);
                     DispStr_CE(14,6,minute,DISP_CURRENT);
                    //delay_and_wait_key(0,EXIT_KEY_F2,0)  ;
                     
                    }
                   break;
                
             }
              case 6:
             {
                  err= input_info(index,second,returndata);
                 if(err == 0)
                 {
                    systime.sec= (unsigned char)atoi(returndata);
                     memset(second,0,5);
                     sprintf(second,"%d",systime.sec);
                     Disp_Goto_XY(0,16);
                     DispStr_CE(0,16,"分：",DISP_CURRENT);
                     DispStr_CE(16,6,second,DISP_CURRENT);
                     
                          DispStr_CE(0,24,"",DISP_CURRENT|DISP_CLRLINE);
                     //delay_and_wait_key(0,EXIT_KEY_F2,0);
                    }
                    // break;
              }
             }
           index++; 
         } 
    
      settime = systime;
      
      RTC_Set_DateTime(&settime);
      
     return 0;
     
} 
*/

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
	char		dBuf[40];
	int 		ret;
	int 		year;
	int		month;
	int		day;
	int		day_max;
	char 		str1[20];
	char		str2[20];
	char		str3[20];
	
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
	char		dBuf[40];
    	int 		ret;
	int 		hour;
	int		minute;
	int		second;
	char 		str1[20];
	char		str2[20];
	char		str3[20];
	
	
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
	char		dBuf[40];
	int 		ret;
	
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











