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
	DispStr_CE(0,4,"正在注销，请稍等",DISP_CENTER|DISP_CLRSCR);
	unsigned char cshut[4]= "*3#\n";

	TCP_Send_Data(cshut,strlen((char *)&cshut));

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



short ConnectServer(){//连接服务器   0表示连接成功，-1 表示连接失败 
	int RET=-1;

	DispStr_CE(0,10,"连接服务器，请稍等...",DISP_POSITION|DISP_CLRSCR);
	unsigned char host[] = "118.123.244.109";
	unsigned char port[] = "8000";

	RET = TCP_Create_Link(host,port);

	//连接成功
	if(RET==0){
		return 0;
	}

	if(RET==-1)
		DispStr_CE(0,8,"命令错误",DISP_POSITION);
	else if(RET==-2)
		DispStr_CE(0,8,"连接服务器失败",DISP_POSITION);
	else
		DispStr_CE(0,8,"连接服务器未知错误",DISP_POSITION);

	delay_and_wait_key(0,EXIT_KEY_F2,0);
	WarningBeep(2);
	SIM900_Module_Close();

	return RET;
}

short SendData(unsigned char* data){    //发送数据    0 表示发送成功 -1 表示失败 
	int RET = 0;
	
	DispStr_CE(0,12,"正在上传，请稍等...",DISP_CENTER|DISP_CLRSCR);
	RET=TCP_Send_Data(data,strlen((char *)data));
	if(RET==0){//发送成功 
		return 0;
	}

	return RET;
}

short GetRecvData(unsigned char* recvdata){//接收服务器返回的结果 0 表示接收成功，-1 表示返回失败 
	int len=0;
	int RET=0;

	RET = TCP_Recv_Data(recvdata,&len,FIVE_SECOND);
	if(RET==0){
		return 0;
	}

	return RET;
}
