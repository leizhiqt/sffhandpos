#include <api_main.h>
#include <exlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "SIM900.h"

static void UART_Send_CMD(unsigned char *AT)
{
	while(*AT)
	{
		UART_Send_Char(*AT);
		AT++;
	}
	UART_Send_Char('\r');
}

static void UART_Send_Data(unsigned char *data,int len)
{
	int i;	
	
	for(i=0;i<len;i++)
	{
		UART_Send_Char(*data);
		data++;
	}
}

static void UART_Recv_Data(unsigned char *data,int *len,int T1,int T2)
{
	typ_UART_stat_word Ustat;

	*len = 0;
	Sys_Timer_Start(SYS_TIMER1,T1);								// 第一个字符的等待时间
	while(Sys_Timer_Read(SYS_TIMER1))
	{
		Ustat.l_word = UART_Stat();
		if(Ustat.bits.buff_data_available)
		{
			*data = UART_Rev_Char();
			data++;
			(*len)++;
			Sys_Timer_Start(SYS_TIMER1,T2);						// 后续字符的等待时间
		}
	}
}

static void UART_Recv_Fix(unsigned char *data,int len,int T)
{
	typ_UART_stat_word Ustat;

	Sys_Timer_Start(SYS_TIMER1,T);								// 第一个字符的等待时间
	while(Sys_Timer_Read(SYS_TIMER1))
	{
		Ustat.l_word = UART_Stat();
		if (Ustat.bits.buff_data_available)
		{
			*data = UART_Rev_Char();
			data++;
			len--;
			if(len <= 0)		return;
			Sys_Timer_Start(SYS_TIMER1,QUARTER_SECOND);			// 后续字符的等待时间
		}
	}
}

int SIM900_Module_Init(void)
{
	unsigned char rBuf[100];
	int rLen;
	
	UART_Modem_Ctrl(UART_MODEM_CTRL2,UART_MODEM_LOW);				// 唤醒状态
	UART_Modem_Ctrl(UART_MODEM_CTRL1,UART_MODEM_HIGH);				// 开关先置为高电平
	
	Sys_Gprs_Power_Control(1);										// 打开电源
	UART_Init(UART_INNER_ON|UART_8_DATA_BITS|UART_BAUD_115200);		// 打开串口
	UART_Modem_Ctrl(UART_MODEM_CTRL1,UART_MODEM_LOW);				// 打开SIM900模块
	Sys_Delay_MS(1200);
	UART_Modem_Ctrl(UART_MODEM_CTRL1,UART_MODEM_HIGH);
	Sys_Delay_MS(2500);
	
	memset(rBuf,0x00,sizeof(rBuf));
	UART_Recv_Data(rBuf,&rLen,TWO_SECOND,TWO_SECOND);
	if(!strstr((char *)rBuf,"RDY"))									// 如果没有收到"RDY",认为是第一次使用,需配置以下参数
	{
		UART_Send_CMD((unsigned char *)"AT+IPR=115200");			// 设置固定波特率
		UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
		
		UART_Send_CMD((unsigned char *)"AT&F");						// 恢复出厂设置
		UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
	
		UART_Send_CMD((unsigned char *)"AT+CIURC=0");				// 关闭 "Call Ready" 提示
		UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
		
		UART_Send_CMD((unsigned char *)"ATE0&W");					// 关闭回显
		UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
	}

	if(strstr((char*)rBuf,"+CPIN: READY"))
	{
		return 0;
	}		
	else if(strstr((char*)rBuf,"+CPIN: NOT INSERTED"))
	{
		return -1;							// ERR: 无SIM卡
	}
	else if(strstr((char*)rBuf,"+CPIN: SIM PIN"))
	{
		return -2;							// ERR: 未验证PIN码
	}
	else
	{
		return -3;							// ERR: 未知错误	
	}
}

void SIM900_Module_Close(void)
{
	UART_Init(UART_OFF);													// 关闭串口
	UART_Modem_Ctrl(UART_MODEM_CTRL1,UART_MODEM_LOW);						// 关闭SIM300模块
	Sys_Delay_MS(2000);
	UART_Modem_Ctrl(UART_MODEM_CTRL1,UART_MODEM_HIGH);
	Sys_Delay_MS(2000);
	Sys_Gprs_Power_Control(0);												// 关闭电源
}

int SIM900_REG_GSM(void)
{
	unsigned char rBuf[100];
	int i,rLen;
	
	for(i=0;i<20;i++)
	{
		memset(rBuf,0x00,sizeof(rBuf));
		UART_Send_CMD((unsigned char *)"AT+CGATT?");
		UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
		if(strstr((char *)rBuf,"+CGATT: 1"))
		{
			return 0;
		}
		Sys_Delay_MS(1000);
	}
	return -1;
}

int SIM900_Verify_PIN(unsigned char *pin)
{
	unsigned char sBuf[100];
	unsigned char rBuf[100];
	int rLen;
	
	memset(sBuf,0x00,sizeof(sBuf));
	memset(rBuf,0x00,sizeof(rBuf));
	sprintf((char *)sBuf,"AT+CPIN=%s",pin);
	UART_Send_CMD(sBuf);
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);	
	if(strstr((char *)rBuf,"OK"))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

void SIM900_Get_IMEI(unsigned char *imei)						// 获取国际移动设备标识
{
	unsigned char rBuf[100];
	int rLen;
	char *pres;
	
	memset(rBuf,0x00,sizeof(rBuf));
	UART_Send_CMD((unsigned char *)"AT+CGSN");
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
	pres = strtok((char *)rBuf+2,"\x0D");
	strcpy((char *)imei,pres);
}

void SIM900_Get_IMSI(unsigned char *imsi)						// 获取国际移动用户标识
{
	unsigned char rBuf[100];
	int rLen;
	char *pres;
	
	memset(rBuf,0x00,sizeof(rBuf));
	UART_Send_CMD((unsigned char *)"AT+CIMI");
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
	pres = strtok((char *)rBuf+2,"\x0D");
	strcpy((char *)imsi,pres);
}

void SIM900_Get_Ver(unsigned char *ver)							// 获取模块软件版本
{
	unsigned char rBuf[100];
	int rLen;
	char *pres;
	
	memset(rBuf,0x00,sizeof(rBuf));
	UART_Send_CMD((unsigned char *)"AT+GMR");
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
	pres = strtok((char *)rBuf,":");	
	pres = strtok('\0',"\r");
	strcpy((char *)ver,pres);
}

void SIM900_Get_Batt(unsigned char *batt)						// 获取电池电量
{
	unsigned char rBuf[100];
	int rLen;
	char *pres;
	
	memset(rBuf,0x00,sizeof(rBuf));
	UART_Send_CMD((unsigned char *)"AT+CBC");
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
	pres = strtok((char *)rBuf,",");	// +CBC: 0	
	pres = strtok('\0',",");			// 66
	pres = strtok('\0',"\x0D");			// 3827
	strcpy((char *)batt,pres);
}

int SIM900_Get_Signal(void)
{
	unsigned char rBuf[100];
	int rLen;
	char *pres;
	int ret;
	
	memset(rBuf,0x00,sizeof(rBuf));
	UART_Send_CMD((unsigned char *)"AT+CSQ");
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);	
	pres = strtok((char *)rBuf," ");
	pres = strtok('\0',",");
	ret = atoi(pres);
	return ret;
}

void TCP_Set_APN(unsigned char *apn,unsigned char *user,unsigned char *pwd)
{
	unsigned char sBuf[100];
	unsigned char rBuf[100];
	int rLen;
	
	memset(sBuf,0,sizeof(sBuf));
	memset(rBuf,0,sizeof(rBuf));
	sprintf((char *)sBuf,"AT+CSTT=\"%s\",\"%s\",\"%s\"",apn,user,pwd);
	UART_Send_CMD(sBuf);
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
	
	memset(rBuf,0,sizeof(rBuf));
	UART_Send_CMD((unsigned char *)"AT+CIICR");						// 激活移动场景
	UART_Recv_Data(rBuf,&rLen,THREE_SECOND,QUARTER_SECOND);
	
	memset(rBuf,0,sizeof(rBuf));
	UART_Send_CMD((unsigned char *)"AT+CIFSR");						// 获取本地IP
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
}

int TCP_Create_Link(unsigned char *ip,unsigned char *port)
{
	unsigned char sBuf[100];
	unsigned char rBuf[100];
	int rLen;
	
	memset(sBuf,0,sizeof(sBuf));
	memset(rBuf,0,sizeof(rBuf));
	sprintf((char *)sBuf,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"",ip,port);
	UART_Send_CMD(sBuf);
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
	if(strstr((char *)rBuf,"OK"))
	{
		memset(rBuf,0,sizeof(rBuf));
		UART_Recv_Data(rBuf,&rLen,FIVE_SECOND,QUARTER_SECOND);
		if(strstr((char *)rBuf,"CONNECT OK"))
		{
			return 0;
		}
		else
		{			
			TCP_Shut_Link();
			return -2;					// 连接错误
		}
	}
	else
	{
		TCP_Shut_Link();
		return -1;						// 命令错误
	}	
}

int TCP_Check_Link(void)
{
	unsigned char rBuf[100];
	int rLen;
	
	memset(rBuf,0,sizeof(rBuf));
	UART_Send_CMD((unsigned char *)"AT+CIPSTATUS");
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
	if(strstr((char *)rBuf,"STATE: CONNECT OK"))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int TCP_Shut_Link(void)
{
	unsigned char rBuf[100];
	int rLen;
		
	memset(rBuf,0,sizeof(rBuf));
	UART_Send_CMD((unsigned char *)"AT+CIPSHUT");
	UART_Recv_Data(rBuf,&rLen,THREE_SECOND,QUARTER_SECOND);	
	if(strstr((char *)rBuf,"SHUT OK"))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int TCP_Send_Data(unsigned char *data,int len)
{
	unsigned char sBuf[100];
	unsigned char rBuf[100];
	int rLen;

	memset(sBuf,0,sizeof(sBuf));
	memset(rBuf,0,sizeof(rBuf));
	sprintf((char *)sBuf,"AT+CIPSEND=%d",len);
	UART_Send_CMD(sBuf);
	UART_Recv_Data(rBuf,&rLen,THREE_SECOND,QUARTER_SECOND);
	if(strstr((char *)rBuf,">"))
	{
		UART_Send_Data(data,len);
		UART_Recv_Fix(rBuf,11,FIVE_SECOND);			// 当发送、接收联合使用时，可能出现接收数据紧随"SEND OK"的情况，所以采用接收固定长度数据
		if(strstr((char*)rBuf,"SEND OK"))
		{
			return 0;
		}
		else
		{
			return -2;								// 发送失败
		}
	}
	else
	{
		return -1;									// 命令错误
	}
}

int TCP_Recv_Data(unsigned char *data,int *len,int T)
{
	UART_Recv_Data(data,len,T,QUARTER_SECOND);

	if(*len)	return 0;

	return -1;
}

void FTP_Set_Server(unsigned char *ip,int port)
{
	unsigned char sBuf[100];
	unsigned char rBuf[100];
	int rLen;

	memset(sBuf,0,sizeof(sBuf));
	memset(rBuf,0,sizeof(rBuf));
	sprintf((char *)sBuf,"AT+FTPSERV=\"%s\"",ip);
	UART_Send_CMD(sBuf);
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
	
	memset(sBuf,0,sizeof(sBuf));
	memset(rBuf,0,sizeof(rBuf));
	sprintf((char *)sBuf,"AT+FTPPORT=%d",port);
	UART_Send_CMD(sBuf);
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
}

void FTP_Set_User(unsigned char *user,unsigned char *pwd)
{
	unsigned char sBuf[100];
	unsigned char rBuf[100];
	int rLen;

	memset(sBuf,0,sizeof(sBuf));
	memset(rBuf,0,sizeof(rBuf));
	sprintf((char *)sBuf,"AT+FTPUN=\"%s\"",user);
	UART_Send_CMD(sBuf);
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
	
	memset(sBuf,0,sizeof(sBuf));
	memset(rBuf,0,sizeof(rBuf));
	sprintf((char *)sBuf,"AT+FTPPW=\"%s\"",pwd);
	UART_Send_CMD(sBuf);
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
}

void FTP_Set_GetFile(unsigned char *path,unsigned char *file)
{
	unsigned char sBuf[100];
	unsigned char rBuf[100];
	int rLen;

	memset(sBuf,0,sizeof(sBuf));
	memset(rBuf,0,sizeof(rBuf));
	sprintf((char *)sBuf,"AT+FTPGETPATH=\"%s\"",path);
	UART_Send_CMD(sBuf);
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);		
	
	memset(sBuf,0,sizeof(sBuf));
	memset(rBuf,0,sizeof(rBuf));
	sprintf((char *)sBuf,"AT+FTPGETNAME=\"%s\"",file);
	UART_Send_CMD(sBuf);
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
}

void FTP_Set_PutFile(unsigned char *path,unsigned char *file)
{
	unsigned char sBuf[100];
	unsigned char rBuf[100];
	int rLen;

	memset(sBuf,0,sizeof(sBuf));
	memset(rBuf,0,sizeof(rBuf));
	sprintf((char *)sBuf,"AT+FTPPUTPATH=\"%s\"",path);
	UART_Send_CMD(sBuf);
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);		
	
	memset(sBuf,0,sizeof(sBuf));
	memset(rBuf,0,sizeof(rBuf));
	sprintf((char *)sBuf,"AT+FTPPUTNAME=\"%s\"",file);
	UART_Send_CMD(sBuf);
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
}

int FTP_Open_Bearer(void)
{
	unsigned char rBuf[100];
	int rLen;

	memset(rBuf,0,sizeof(rBuf));
	UART_Send_CMD((unsigned char *)"AT+FTPCID=1");
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
	
	memset(rBuf,0,sizeof(rBuf));
	UART_Send_CMD((unsigned char *)"AT+SAPBR=1,1");
	UART_Recv_Data(rBuf,&rLen,FIVE_SECOND,QUARTER_SECOND);	
	if(strstr((char *)rBuf,"OK"))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

void FTP_Close_Bearer(void)
{
	unsigned char rBuf[100];
	int rLen;

	memset(rBuf,0,sizeof(rBuf));
	UART_Send_CMD((unsigned char *)"AT+SAPBR=0,1");
	UART_Recv_Data(rBuf,&rLen,HALF_SECOND,QUARTER_SECOND);
}

int FTP_Get_File(unsigned char *data,int *len)
{
	unsigned char rBuf[100];
	int rLen;
	char *pres;

	memset(rBuf,0,sizeof(rBuf));
	UART_Send_CMD((unsigned char *)"AT+FTPGET=1");
	UART_Recv_Data(rBuf,&rLen,THREE_SECOND,QUARTER_SECOND);
	if(strstr((char *)rBuf,"OK"))
	{
		UART_Recv_Data(rBuf,&rLen,TEN_SECOND,QUARTER_SECOND);
		if(strstr((char *)rBuf,"+FTPGET:1,1"))
		{
			UART_Send_CMD((unsigned char *)"AT+FTPGET=2,1024");
			UART_Recv_Data(rBuf,&rLen,THREE_SECOND,QUARTER_SECOND);
			
			pres = strtok((char *)rBuf,",");
			pres = strtok('\0',"\x0A");
			*len = atoi(pres);
			pres = strtok('\0',"\x0D");
			strcpy((char *)data,pres);
			return 0;
		}
		else
		{
			return -2;
		}
	}
	else
	{
		return -1;
	}
}

int FTP_Put_File(unsigned char *data,int len)
{
	unsigned char sBuf[100];
	unsigned char rBuf[100];
	int rLen;

	memset(rBuf,0,sizeof(rBuf));
	UART_Send_CMD((unsigned char *)"AT+FTPPUT=1");
	UART_Recv_Data(rBuf,&rLen,THREE_SECOND,QUARTER_SECOND);
	if(strstr((char *)rBuf,"OK"))
	{
		UART_Recv_Data(rBuf,&rLen,TEN_SECOND,QUARTER_SECOND);
		if(strstr((char *)rBuf,"+FTPPUT:1,1"))
		{
			memset(sBuf,0x00,sizeof(sBuf));
			sprintf((char *)sBuf,"AT+FTPPUT=2,%d",len);
			UART_Send_CMD(sBuf);
			UART_Recv_Data(rBuf,&rLen,THREE_SECOND,QUARTER_SECOND);
			{
				char str[20];
				
				sprintf(str,"%d",rLen);
				DispStr_CE(0,0,str,DISP_POSITION|DISP_CLRSCR);
				DispStr_CE(0,4,(char *)rBuf,DISP_POSITION);
				DispStr_CE(0,8,(char *)rBuf+15,DISP_POSITION);
				DispStr_CE(0,12,(char *)rBuf+30,DISP_POSITION);
				DispStr_CE(0,16,(char *)rBuf+45,DISP_POSITION);
				delay_and_wait_key(0,EXIT_KEY_ALL,0);
				UART_Send_Data(data,len);
			}
			return 0;
		}
		else
		{
			return -2;
		}
	}
	else
	{
		return -1;
	}
	
}


//	{
//		char str[20];
//		
//		sprintf(str,"%d",rLen);
//		DispStr_CE(0,0,str,DISP_POSITION|DISP_CLRSCR);
//		DispStr_CE(0,4,(char *)rBuf,DISP_POSITION);
//		DispStr_CE(0,8,(char *)rBuf+15,DISP_POSITION);
//		DispStr_CE(0,12,(char *)rBuf+30,DISP_POSITION);
//		DispStr_CE(0,16,(char *)rBuf+45,DISP_POSITION);
//		delay_and_wait_key(0,EXIT_KEY_ALL,0);
//	}


