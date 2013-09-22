/****************************************************************************
*
*文件(File):         Gprs.c
*
*修改(Modify):       2012-7-17 8:17:48
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
| V1.00  2012-7-17 8:17:48            
----------------------------------------------------------------------------
****************************************************************************/
#include "exlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "common.h"
#include "SIM900.h"
#include "gprs.h"

//extern void WarningBeep();

//关闭gprs模块
void sim900_close(void){
	DispStr_CE(0,4,"正在关闭gprs模块，请稍等",DISP_CENTER|DISP_CLRSCR);
	SIM900_Module_Close();
}

//断开与服务器连接并关闭gprs模块
void DisConnectServer(){
	int Ret = 0;
	DispStr_CE(0,4,"正在注销，请稍等",DISP_CENTER|DISP_CLRSCR);
	unsigned char cshut[4]= "*3#\n";


	Ret = TCP_Check_Link();
	if(Ret == 0)
	{
		TCP_Send_Data(cshut,strlen((char *)&cshut));

		//断开与服务器联接
		TCP_Shut_Link();
	}
	sim900_close();
}

//连接网络，返回0表示成功，其余失败 
short sim900_init(void){
	int RET=0;
	
	DispStr_CE(0,0,"模块初始化...",DISP_POSITION|DISP_CLRSCR);
	RET = SIM900_Module_Init();
	if(RET == -1)
	{
		DispStr_CE(0,4,"请检查SIM卡连接",DISP_POSITION);
		DispStr_CE(0,8,"关闭模块...",DISP_POSITION);
		SIM900_Module_Close();
		return -1;
	}
	else if(RET == -2)
	{
		if(SIM900_Verify_PIN((unsigned char *)"1234"))
		{
			DispStr_CE(0,4,"验证PIN码错误",DISP_POSITION);
			DispStr_CE(0,8,"关闭模块...",DISP_POSITION);
			SIM900_Module_Close();	
			return -1;
		}
	}
	else if(RET == -3)
	{
		DispStr_CE(0,4,"请检查模块连接",DISP_POSITION);
		DispStr_CE(0,8,"关闭模块...",DISP_POSITION);
		SIM900_Module_Close();
		return -1;
	}
	
	DispStr_CE(0,4,"模块初始化成功",DISP_POSITION);
	DispStr_CE(0,8,"注册网络...",DISP_POSITION);

	RET = SIM900_REG_GSM();
	if(RET<0)
	{
		DispStr_CE(0,12,"注册网络失败",DISP_POSITION);
		DispStr_CE(0,16,"关闭模块...",DISP_POSITION);
		SIM900_Module_Close();
		return -1;
	}

	DispStr_CE(0,12,"注册网络成功",DISP_POSITION);
	delay_and_wait_key(2,EXIT_KEY_ALL,0);

	return 0;
}


//连接服务器   0表示连接成功，-1 表示连接失败 
short ConnectServer(){
	int i = 0;
	int RET=-1;

	DispStr_CE(0,10,"连接服务器，请稍等...",DISP_POSITION|DISP_CLRSCR);
	
	//lzy 测试服务器	
	//unsigned char host[] = "113.89.1.149";
	//unsigned char port[] = "5300";

	//lzy 实际的服务器
	unsigned char host[] = "118.123.244.109";
	unsigned char port[] = "8000";	

	//连接成功
	for(i = 0; i < 3; i++)
	{
		Sys_Delay_MS(100);
		RET = TCP_Create_Link(host,port);
		if(RET==0){
			return 0;
		}
		else{
			break;
		}
	}

	if(RET==-1)
		DispStr_CE(0,12,"命令错误",DISP_POSITION);
	else if(RET==-2)
		DispStr_CE(0,12,"连接服务器失败",DISP_POSITION);
	else
		DispStr_CE(0,12,"连接服务器未知错误",DISP_POSITION);

	DispStr_CE(0,36,"【F2退出】",DISP_POSITION|DISP_CLRLINE); 	

	WarningBeep(2);
	KEY_Flush_FIFO();
	delay_and_wait_key(30,EXIT_KEY_F2,30);	
	return RET;
}

short SendData(unsigned char* data){    //发送数据    0 表示发送成功 -1 表示失败 
	int RET = 0;

	//测试信号强度
	RET = TestSignal();
	if(RET != 0)
	{
		DispStr_CE(0,12,"信号太错,请稍后再上传...",DISP_CENTER|DISP_CLRSCR);
		WarningBeep(2);
		KEY_Flush_FIFO();
		delay_and_wait_key(30,EXIT_KEY_ALL,30);			
		return RET;
	}

	//测试是否联接上服务器
	RET = TCP_Check_Link();
	if(RET != 0)
	{
		DispStr_CE(0,12,"服务器断开,不能上传.",DISP_CENTER|DISP_CLRSCR);
		WarningBeep(2);
		KEY_Flush_FIFO();
		delay_and_wait_key(30,EXIT_KEY_ALL,30);				
		return RET;
	}
	
	DispStr_CE(0,12,"正在上传，请稍等...",DISP_CENTER|DISP_CLRSCR);
	RET=TCP_Send_Data(data,strlen((char *)data));
	if(RET==0){//发送成功 
		return 0;
	}

	return RET;
}

short GetRecvData(unsigned char* recvdata){//接收服务器返回的结果 0 表示接收成功，-1 表示返回失败 
	int len=0;
	int RET=-1;

	//测试是否联接上服务器
	RET = TCP_Check_Link();
	if(RET != 0)
	{
		DispStr_CE(0,12,"服务器断开,不能接收数据.",DISP_CENTER|DISP_CLRSCR);
		WarningBeep(2);
		delay_and_wait_key(30,EXIT_KEY_ALL,30);				
		return RET;
	}

	RET = TCP_Recv_Data(recvdata,&len,SEVEN_SECOND);
	if(RET==0){
		return 0;
	}

	return RET;
}


//测试GPRS信息号
int TestSignal(void){    //发送数据    0 表示发送成功 -1 表示失败 
	int i = 0;
	int signal = 10;
	//int signal = 30;
	int RET = 0;
	char Temp[40];
	short Key = 0;
	
	DispStr_CE(0,12,"信号测试，请稍等...",DISP_CENTER|DISP_CLRSCR);
	while(1)
	{
		memset(Temp, '\0', sizeof(Temp));
		sprintf(Temp, "最低信号强度%d", signal);
		DispStr_CE(0,14,Temp,DISP_CENTER|DISP_CLRLINE);				
		DispStr_CE(0,36,"【F1退出】",DISP_CENTER | DISP_CLRLINE);
		RET=SIM900_Get_Signal();				
		
		memset(Temp, '\0', sizeof(Temp));
		sprintf(Temp, "信号强度(0-32):%d,尝试:%d次", RET, ++i); 
		DispStr_CE(0,16,Temp,DISP_CENTER|DISP_CLRLINE);		
		if(RET>=signal)
		{
			Sys_Delay_MS(100);
			return(0);	
		}
		
		Sys_Delay_MS(500);
		Key = KEY_read();
		KEY_Flush_FIFO();
		if(Key == KEY_F1)
		{break;}
	}
	return(-1);
}

