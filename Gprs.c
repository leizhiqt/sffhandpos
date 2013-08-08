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
void sim300_close(void){
	DispStr_CE(0,4,"正在关闭gprs模块，请稍等",DISP_CENTER|DISP_CLRSCR);
	SIM900_Module_Close();
}

//断开与服务器连接并关闭gprs模块
void DisConnectServer(){
	DispStr_CE(0,4,"正在注销，请稍等",DISP_CENTER|DISP_CLRSCR);
	unsigned char cshut[4]= "*3#\n";

	TCP_Send_Data(cshut,strlen((char *)&cshut));

	sim300_close();
}

//连接网络，返回0表示成功，其余失败 
short sim300_init(void){
	int ret;
	
	DispStr_CE(0,0,"模块初始化...",DISP_POSITION|DISP_CLRSCR);
	ret = SIM900_Module_Init();
	if(ret == -1)
	{
		DispStr_CE(0,4,"请检查SIM卡连接",DISP_POSITION);
		DispStr_CE(0,8,"关闭模块...",DISP_POSITION);
		SIM900_Module_Close();
		return -1;
	}
	else if(ret == -2)
	{
		if(SIM900_Verify_PIN((unsigned char *)"1234"))
		{
			DispStr_CE(0,4,"验证PIN码错误",DISP_POSITION);
			DispStr_CE(0,8,"关闭模块...",DISP_POSITION);
			SIM900_Module_Close();	
			return -1;
		}
	}
	else if(ret == -3)
	{
		DispStr_CE(0,4,"请检查模块连接",DISP_POSITION);
		DispStr_CE(0,8,"关闭模块...",DISP_POSITION);
		SIM900_Module_Close();
		return -1;
	}
	
	DispStr_CE(0,4,"模块初始化成功",DISP_POSITION);
	DispStr_CE(0,8,"注册网络...",DISP_POSITION);

	ret = SIM900_REG_GSM();
	if(ret<0)
	{
		DispStr_CE(0,12,"注册网络失败",DISP_POSITION);
		DispStr_CE(0,16,"关闭模块...",DISP_POSITION);
		SIM900_Module_Close();
		return -1;
	}

	DispStr_CE(0,12,"注册网络成功",DISP_POSITION);
	delay_and_wait_key(2,EXIT_KEY_ALL,0);

	return 0;

        /*
	DispStr_CE(0,0,"网络模块初始化...",DISP_CENTER|DISP_CLRSCR);
	ret = sim_module_init();
	if(ret<0){
		WarningBeep(2);

		if( g_simerr == SIM_POSTYPE_ERROR ){
			DispStr_CE(0,2,"此机型不支持GPRS",DISP_POSITION);
		}else if( g_simerr == SIM_MODULE_ERROR ){
			DispStr_CE(0,2,"请检查模块连接",DISP_POSITION);
			DispStr_CE(0,4,"关闭模块...",DISP_POSITION);
			SIM900_Module_Close();
		}else if( g_simerr == SIM_NO_CARD ){
			DispStr_CE(0,2,"请检查SIM卡连接",DISP_POSITION);
			DispStr_CE(0,4,"关闭模块...",DISP_POSITION);
			SIM900_Module_Close();
		}
		 return -1;
	}else{
		DispStr_CE(0,2,"模块初始化成功",DISP_POSITION);
		DispStr_CE(0,4,"连接网络...",DISP_POSITION);

                ret = sim_signal_report();
                if(ret<10){
			DispStr_CE(0,8,"信号太差",DISP_POSITION);
			return -2;
		}
               DispStr_CE(0,8,"信号良好",DISP_POSITION);

		ret = sim_detect_net();
		if(ret<0){
			DispStr_CE(0,10,"连接网络失败",DISP_POSITION);
			 WarningBeep(2);
			SIM900_Module_Close();
			return -3;
			
		}
	}

	DispStr_CE(0,10,"连接网络成功",DISP_POSITION);
	return 0;
*/
}



short ConnectServer(){//连接服务器   0表示连接成功，-1 表示连接失败 
	short ret ;
	DispStr_CE(0,10,"连接服务器，请稍等...",DISP_POSITION|DISP_CLRSCR);
	unsigned char host[] = "118.123.244.109";
	unsigned char port[] = "8000";
	ret = TCP_Create_Link(host,port);

	//连接成功
	if(ret==0){
		return 0;
	}

	if(ret==-1){
		DispStr_CE(0,8,"命令错误",DISP_CENTER);
	}else if(ret==-2){
		DispStr_CE(0,8,"连接服务器失败",DISP_CENTER);
	}

	delay_and_wait_key(0,EXIT_KEY_F2,0);
	WarningBeep(2);
	SIM900_Module_Close();
	
	return -1;
}

short SendData(unsigned char* data){    //发送数据    0 表示发送成功 -1 表示失败 
	DispStr_CE(0,12,"正在上传，请稍等...",DISP_CENTER|DISP_CLRSCR);  
	if(TCP_Send_Data(data,strlen((char *)data))==0){//发送成功 
		return 0;
	}else{ 
		return -1;
	}
}

short GetRecvData(unsigned char* recvdata){//接收服务器返回的结果 0 表示接收成功，-1 表示返回失败 
	int len;
	int RET=0;

	RET = TCP_Recv_Data(recvdata,&len,256);
	if(RET != 0){
		DispStr_CE(0,10,"接收失败",DISP_CENTER|DISP_CLRSCR);
		delay_and_wait_key(10,EXIT_KEY_F2,10);
		delay_and_wait_key(1,EXIT_KEY_F2|EXIT_AUTO_QUIT,1);
		WarningBeep(2);
		return -1;
	}

	return 0;
}
