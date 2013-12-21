/****************************************************************************
*
*文件(File):         SF.c
*
*修改(Modify):       2012-7-13 13:58:26
*
*作者(Author):       sa
*
*编绎(Compile):      智能平台(Smart Platform)
*
*描述(Description):
*1..ARM 项目文件(ARM Project):SF.dsp 
*2..项目二进制输出文件(project binary output  file):SF.bin
*
----------------------------------------------------------------------------
|
| Version | Datetime             |   Author    | Description
| --------+----------------------+-------------+----------------------------
|
| V1.00  2012-7-13 13:58:26           sa 
----------------------------------------------------------------------------
****************************************************************************/

/*在此增加代码(TODO: add  code here)*/

#include "API.h"
#include "Card.h"
#include "DataBase.h"
#include "Gprs.h"
#include "UiProg.h"
#include "SF.h"
#include "sim900.h"
#include "common.h"

//======================================================================
//函数名：main 
//功能  ：主函数 程序入口 
//参数  ：无 
//返回值：int
//======================================================================
int main(void){
	InitSystem();
	MainMenu();
	return 0;
}

short Display(short flag){
	int lPtr = 0;
	int cPtr=0;
	int IntRet;

	PackUpMenuData(Menu, 150, 28);
	while(1){ 	
		IntRet = browse_info(2,Menu,&cPtr,&lPtr,flag);
		if(IntRet == KEY_F1){ //退出 
			return -1;
		}else if(IntRet == KEY_F3){//确认提交 
			return 0;
		}
	}
}

long Alert(){
	long key_value = 0; 
	
	WarningBeep(2);
	DispStr_CE(0,2,"特别提醒",DISP_CLRSCR|DISP_CENTER);
	EXT_Display_Multi_Lines("此功能要通过GPRS通信，可能会有危险，请确定环境是否安全！",6,10);	
 	DispStr_CE(0,36,"【F1退出】     【F3继续】",DISP_CENTER | DISP_CLRLINE);
 	key_value = delay_and_wait_key(30,EXIT_KEY_ENTER|EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
	return key_value;
}

//======================================================================
//函数名：InitSystem 
//功能  ：系统初始化设置 
//参数  ：无 
//返回值：无 
//======================================================================
void InitSystem()
{
	int ret = 0;
  Disp_Init(DISP_INIT_ON);		//Open LCD
	Disp_Clear();				
	
	KEY_Init(KEY_BEEP_KEY|KEY_AUTO_EL_ON);	//Open Keyborad
	KEY_Beep_Mask_Set(0x00000000);			
	KEY_EL_Mask_Set(0xffffffff);
	EL_Set_Timeout(60);			//close EL in 10 second if not press any key
	Sys_Auto_Poweroff_Set(AutoPowerOff);		//Power off in 2 minutes if not do anything
	
	//Meanless for 0518
	Disp_Icon_Battery_Time_Set(1,ICON_AT_TAIL);
	Disp_Icon_Battery_Time_Refresh();
	
	Disp_set_color(SET_FONT_COLOR, 0x0000);		//Black
	Disp_Set_Color(SET_BACK_COLOR, 0xffff);	//green
	Disp_set_color(SET_SELECT_COLOR,  0x07E0);	//Green
	Disp_set_color(SET_CLEAR_COLOR, 0xffff);	//White


	//判断数据库是否已被格式化
	ret = DB_init_sys_param(0);
	//数据库没有初始化
	if(ret == -1)
	{
		//设置为默认值
		SysObj.SimCard = 0;
		//写参数到NorFalsh		
		User_Param_Write(0, (unsigned char *)&SysObj, sizeof(SysPara));
	}
	//已格式化
	else
	{
		//读参数据到结构
		User_Param_Read(0, (unsigned char *)&SysObj, sizeof(SysPara));			
		if((SysObj.SimCard != 0) && (SysObj.SimCard != 1))
		{ 	
			DispStr_CE(0,10,"SIM卡运营商参数错误",DISP_CENTER|DISP_CLRSCR); 
			DispStr_CE(0,12,"强制设置为移动",DISP_CENTER);			
			delay_and_wait_key(30,EXIT_KEY_F2,30);			
		
			SysObj.SimCard = 0;
			User_Param_Write(0, (unsigned char *)&SysObj, sizeof(SysPara)); 	
		}		
	}
}

//======================================================================
//函数名：Query 
//功能  ：完成巡检功能 
//参数  ：无 
//返回值：无 
//======================================================================
void Query(){
	int ret ;
	long key_value;
	char record[200];

	ret = FindDatabase(); 
	//没找到数据库 创建 
	if(ret != 0){
		int i=0; 
		for(i=0;i<4;i++){
			if((ret = CreateDatabase())==0)
				break;
		}
	}

	if(ret != 0){
		DispStr_CE(0,0,"查找数据库失败",DISP_CLRSCR|DISP_CENTER);
		DispStr_CE(0,36,"【F2退出】",DISP_POSITION | DISP_CLRLINE);		
		WarningBeep(2);
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return ; 
	}

	//查找数据库成功
	DispStr_CE(0,4,"请刷卡登陆",DISP_CENTER|DISP_CLRSCR);
	EXT_Display_Multi_Lines("请将您的身份卡放在感应区下方，然后按F3登陆键进行登陆操作，如果想退出可以按F1退出！",8,16);
 	DispStr_CE(0,36,"【F1退出】     【F3登陆】",DISP_CENTER | DISP_CLRLINE); 
	key_value = delay_and_wait_key(30,EXIT_KEY_ENTER|EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
	switch(key_value){
		case EXIT_KEY_F1:
		case EXIT_AUTO_QUIT:{
			return ; 
		}

		case EXIT_KEY_F3:
		case EXIT_KEY_ENTER:{
			break;
		}
	}

	ret = LoginByCard();
	if(ret != 0){//登陆失败
		return ; 
	}

	//登陆成功
	Disp_Clear();

	DispStr_CE(0,2,"登陆成功!",DISP_CENTER|DISP_CLRSCR);
	DispStr_CE(0,4,"巡 检 人:",DISP_POSITION);
	DispStr_CE(0,4,(char *)&username,DISP_CURRENT);

	DispStr_CE(0,8,"按任意键进行巡检",DISP_CENTER);
	EXT_Display_Multi_Lines("请将巡检芯片放在感应区下方，按任意键进行巡检。",10,16);
	WarningBeep(0);
	delay_and_wait_key(0,EXIT_KEY_ALL,0);

	//得到了username
	//获取防伪码 和 时间
	int flag =1; 
	while(flag){
		memset(systime,0,QUERYTIME_LEN);   //清空系统时间 
		memset(anticode,0,ANTIFAKECODE_LEN);  //清空防伪码 
		//获取防伪码
		if(ultralight_local_read_card(anticode)==0){
			//获取防伪码成功 
			//获取系统时间
			GetSysTime(systime);
			//插入记录 
			//short AddOneRecord(char* name,char* time,char* code)			
			memset(record,0,sizeof(record)); 

			/*
			{//lzy 生成测试数据
				int i = 0;
				char Temp[40];
				
				for(i = 0; i < 1000; i++)
				{
					memset(Temp, '\0', sizeof(Temp));
					sprintf(Temp, "i = %d", i);
					DispStr_CE(0,0,Temp,DISP_CENTER|DISP_CLRSCR); 
					
					memset(systime, '\0', sizeof(systime));
					GetSysTime(systime);
					AddOneRecord(username,systime,anticode,record);
					Sys_Delay_MS(500);
				}
			}
			*/
			
			ret = AddOneRecord(username,systime,anticode,record);
			if(ret == 0){   //插入成功 
				DispStr_CE(0,0,"巡检成功",DISP_CENTER|DISP_CLRSCR);
				DispStr_CE(0,2,"巡检结果如下",DISP_CENTER);
				DispStr_CE(0,6,"巡 检 人：",DISP_POSITION);
				DispStr_CE(0,6,(char *)username,DISP_CURRENT);
				DispStr_CE(0,8,"防 伪 码：",DISP_POSITION);
				DispStr_CE(0,8,anticode,DISP_CURRENT);
				DispStr_CE(0,10,"巡检时间：",DISP_POSITION);
				DispStr_CE(0,10,systime,DISP_CURRENT);
				DispStr_CE(0,12,"记 录 号：",DISP_POSITION);
				DispStr_CE(0,12,record,DISP_CURRENT);
			}else{//插入失败 
				DispStr_CE(0,0,"插入记录失败，请从新巡检",DISP_CENTER|DISP_CLRSCR); 
			}

 			DispStr_CE(0,36,"【F1退出】     【F3继续】",DISP_CENTER | DISP_CLRLINE); 
			key_value = delay_and_wait_key(0,EXIT_KEY_F1|EXIT_KEY_F3,0);
			switch(key_value){
				case EXIT_KEY_F1:{
					flag = 0;//退出循环
					break;
				}
				case EXIT_KEY_F3:{
					flag =1; 
					break; 
				}
			}
		}else{
			flag = 0; 
		} 
	}//end while
}

short OpenGPRS(){
	short RET=-1;

	RET = sim900_init();//连接网络
	if(RET ==0){//连接网络成功
		RET = ConnectServer();//连接服务器 
		if(RET == 0){//连接服务器成功
			return 0; 
		}
	}

	DispStr_CE(0,4,"连接服务器失败，任意键退出",DISP_CENTER|DISP_CLRSCR);
	sim900_close();
	KEY_Flush_FIFO();
	return -1;
}
//======================================================================
//功能  ：同步时间
//参数  ：无 
//返回值：无 
//发送:*70#
//返回信息:*0;70;2013-12-20 17:24:15#
//======================================================================
void SynTime(void)
{
	int i=0;
	int Len = 0;
	int RET = 0;	
	char Temp[40];
	char *PTemp1 = NULL;
	char *PTemp2 = NULL;	
	unsigned char data[1024 * 2];
	
	long choose = 0; 	
	Time TObj;
	typ_DATE_TIME DTObj;
	
	choose = Alert(); 
	switch(choose)
	{
		case EXIT_KEY_F3:
		case EXIT_KEY_ENTER: 
		{}break;

		default:
		{return;}break;
	}	
	
	//初始化模块
	RET = sim900_init();
	if(RET != 0)
	{
		DispStr_CE(0,4,"连接服务器失败，任意键退出",DISP_CENTER|DISP_CLRSCR);
		KEY_Flush_FIFO();
		delay_and_wait_key(0,EXIT_KEY_ALL,0);
		return;
	}
	
	//打开GPRS模块 && 服务器
	for(i=0;i<3;i++)
	{
		RET = ConnectServer();
		if(RET == 0)
		{
			break;
		}
	}
	
	//GPRS打开失败
	if(RET != 0)
	{ 
		DispStr_CE(0,2,"请检查SIM卡或通信模块",DISP_POSITION|DISP_CLRSCR); 
		DispStr_CE(0,4,"按任意键退出",DISP_POSITION); 
		KEY_Flush_FIFO();
		delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);
		goto LoopA;			
	}	

	//发送数据
	memset(data, '\0', sizeof(data));
	strcpy((char*)data, "*70#\n");
	RET = SendData(data, 1);

	//接收返回数据
	memset(data, '\0', sizeof(data));
	RET = GetRecvData(data);
	if(RET < 0)
	{ 
		WarningBeep(2);
		DispStr_CE(0, 2, "时间同步失败", DISP_POSITION | DISP_CLRSCR); 
		DispStr_CE(0, 4, "接收返回数据失败", DISP_POSITION); 
		KEY_Flush_FIFO();
		delay_and_wait_key(30, EXIT_KEY_ALL, 30);
		goto LoopA;		
	}
	else
	{
		memset(Temp, '\0', sizeof(Temp));
		strcpy(Temp, "*0;70;2013-12-20 17:24:15#");	
		Len = strlen(Temp);
		if((int)strlen((char*)data) < Len)
		{
			WarningBeep(2);
			DispStr_CE(0, 2, "数据丢失", DISP_POSITION | DISP_CLRSCR); 
			DispStr_CE(0, 4, "接收返回数据失败", DISP_POSITION); 
			KEY_Flush_FIFO();
			delay_and_wait_key(30, EXIT_KEY_ALL, 30);		
			goto LoopA;			
		}
		
		//校验数据的正确性
		//*0;70;2013-12-20 17:24:15#
		//判断报头
		memset(Temp, '\0', sizeof(Temp));
		strcpy(Temp, "*0;70;");
		PTemp1 = strstr((char*)data, Temp);
		if(PTemp1 == NULL)
		{
			WarningBeep(2);
			DispStr_CE(0, 2, "报头丢失", DISP_POSITION | DISP_CLRSCR); 
			DispStr_CE(0, 4, "接收返回数据失败", DISP_POSITION); 
			KEY_Flush_FIFO();
			delay_and_wait_key(30, EXIT_KEY_ALL, 30);		
			goto LoopA;			
		}

		//移动指针到数据开始位置
		PTemp1 += strlen(Temp);

		//判断结束符
		PTemp2 = strstr(PTemp1, "#");
		if(PTemp2 == NULL)
		{
			WarningBeep(2);
			DispStr_CE(0, 2, "报尾丢失", DISP_POSITION | DISP_CLRSCR); 
			DispStr_CE(0, 4, "接收返回数据失败", DISP_POSITION); 
			KEY_Flush_FIFO();
			delay_and_wait_key(30, EXIT_KEY_ALL, 30); 	
			goto LoopA;			
		}

		//数据长度
		Len = (PTemp2 - PTemp1);
		if(Len <= 0)
		{
			WarningBeep(2);
			DispStr_CE(0, 2, "日期时间数据丢失", DISP_POSITION | DISP_CLRSCR); 
			DispStr_CE(0, 4, "接收返回数据失败", DISP_POSITION); 
			KEY_Flush_FIFO();
			delay_and_wait_key(30, EXIT_KEY_ALL, 30); 	
			goto LoopA;
		}

		//填充暂时结构
		memset(&TObj, '\0', sizeof(Time));
		memcpy(&TObj, PTemp1, sizeof(Time));
		PTemp2 = (char*)&TObj;
		//把-,:和空格换成字符串结束符\0
		for(i = 0; i < Len; i++)
		{
			Temp[0] = PTemp2[i];
			if((Temp[0] == '-') || (Temp[0] == ':') || (Temp[0] == 0x20))
			{
				PTemp2[i] = '\0';
			}
		}
		
		//填充真实的日期时间结构
		memset(&DTObj, '\0', sizeof(typ_DATE_TIME));

		DTObj.year = atoi(TObj.year);
		DTObj.month = atoi(TObj.month);
		DTObj.day = atoi(TObj.day);

		DTObj.hour = atoi(TObj.hour);
		DTObj.min = atoi(TObj.min);
		DTObj.sec = atoi(TObj.sec);
		DTObj.week = RTC_get_week(DTObj.year, DTObj.month, DTObj.day);		
		RTC_Set_DateTime(&DTObj);		
		
		memset(Temp, '\0', sizeof(Temp));		
		DispStr_CE(0, 2, "日期时间同步成功", DISP_POSITION | DISP_CLRSCR); 
		sprintf(Temp, "%s", PTemp1);
		DispStr_CE(0, 4, Temp, DISP_POSITION); 
		WarningBeep(1);
		KEY_Flush_FIFO();
		delay_and_wait_key(30, EXIT_KEY_ALL, 30); 			
	}
LoopA:
	
	DispStr_CE(0,4,"正在注销，请稍等..",DISP_CENTER|DISP_CLRSCR);
	DisConnectServer();	
	KEY_Flush_FIFO();
}

//======================================================================
//函数名：SubmitData 
//功能  ：完成数据提交 
//参数  ：无 
//返回值：无 
//======================================================================
void SubmitData(){
	int cLoop =0;
	int i=0;
	int Len = 0;
	int RET = 0;	
	long choose = 0;
	long key_value = 0; 
	unsigned char data[1024 * 2];
	
	choose = Alert(); 
	switch(choose){
		case EXIT_KEY_F1:
		case EXIT_AUTO_QUIT:{
			return;
		}
		
		case EXIT_KEY_F3:
		case EXIT_KEY_ENTER: {
			break;
		}		
	}

	//刷卡登陆 信息验证 
	DispStr_CE(0,4,"请刷卡登陆",DISP_CENTER|DISP_CLRSCR);
	EXT_Display_Multi_Lines("请将您的身份卡放在感应区下方，然后按F3登陆键进行登陆操作，如果想退出可以按F1退出！",8,16);	
 	DispStr_CE(0,36,"【F1退出】     【F3登陆】",DISP_CENTER | DISP_CLRLINE); 	
	key_value = delay_and_wait_key(30,EXIT_KEY_ENTER|EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
	switch(key_value){
		case EXIT_KEY_F1:
		case EXIT_AUTO_QUIT:{
			return ;
		}

		case EXIT_KEY_F3:
		case EXIT_KEY_ENTER:{
			break;
		}
	}
 	RET = LoginToSubmit();
	if(RET != 0){
		DispStr_CE(0,2,"刷卡失败!",DISP_POSITION|DISP_CLRSCR);
		return;
	}

	memset(data,'\0',sizeof(data));

	//首次检查数据
	RET=EncodeSendData(sname,spass,data);
	/*
	//lzy tset
	{
		char TempA[1024];
		
		memset(TempA, '\0', sizeof(TempA));
		sprintf(TempA, "T1:%d", strlen(data));
		DispStr_CE(0, 0, TempA, DISP_CENTER | DISP_CLRSCR);
		
		delay_and_wait_key(0,EXIT_KEY_ALL,0);
	}	
	*/
	if(RET<0){
		CreateDatabase();

		WarningBeep(0); 
		DispStr_CE(0,2,"没有数据提交，按任意键退出",DISP_POSITION|DISP_CLRSCR);
		KEY_Flush_FIFO();
		delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);

		return;
	}
	Len = strlen((char*)data);
	if(Len > 1024)
	{
		WarningBeep(0); 
		DispStr_CE(0,2,"提交数据太长，按任意键退出",DISP_POSITION|DISP_CLRSCR);
		KEY_Flush_FIFO();
		delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);

		return;		
	}
	
	//初始化模块
	RET = sim900_init();
	if(RET != 0)
	{
		DispStr_CE(0,4,"连接服务器失败，任意键退出",DISP_CENTER|DISP_CLRSCR);
		KEY_Flush_FIFO();
		delay_and_wait_key(0,EXIT_KEY_ALL,0);
		return;
	}
	
	//打开GPRS模块 && 服务器
	for(i=0;i<3;i++){
		RET = ConnectServer();
		if(RET == 0){
			break;
		}
	}
	
	//GPRS打开失败
	if(RET != 0){ 
		DispStr_CE(0,2,"请检查SIM卡或通信模块",DISP_POSITION|DISP_CLRSCR); 
		DispStr_CE(0,4,"按任意键退出",DISP_POSITION); 
		KEY_Flush_FIFO();
		delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);
		return;
	}
 
	while(1){
		memset(data,'\0',sizeof(data));

		//暂存没有数据
		RET=EncodeSendData(sname,spass,data);		
		//延时2秒
		Sys_Delay_MS(64 * 2);
		
		/*
		//lzy tset
		{
			char TempA[1024];
			
			memset(TempA, '\0', sizeof(TempA));
			sprintf(TempA, "T2:%d", strlen(data));
			DispStr_CE(0, 0, TempA, DISP_CENTER | DISP_CLRSCR);
			
			delay_and_wait_key(0,EXIT_KEY_ALL,0);
		} 
		*/
		if(RET<1){
			DB_erase_filesys(0);
			//CreateDatabase();

			WarningBeep(0);
			DispStr_CE(0,2,"所有数据上传完毕，按任意键退出",DISP_POSITION|DISP_CLRSCR);
			KEY_Flush_FIFO();
			delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);

			break;
		}
		Len = strlen((char*)data);
		if(Len > 1024)
		{
			//断开联接关闭模块
			DisConnectServer();
			
			WarningBeep(0); 
			DispStr_CE(0,2,"提交数据太长，按任意键退出",DISP_POSITION|DISP_CLRSCR);
			KEY_Flush_FIFO();
			delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);
						
			return; 	
		}

		//上传数据
		WarningBeep(0);
		RET = Display(0);
		if(RET == -1){//退出 
			break;
		}

		cLoop = 0;
		while(1)
		{
			//发送数据
			RET = SendData(data, 0);
			if(RET != 0)
			{
				//发送失败
				cLoop++;
				
				WarningBeep(2);
				DispStr_CE(0,4,"数据发送失败!",DISP_POSITION|DISP_CLRSCR);
				DispStr_CE(0,36,"【F1退出重连】",DISP_POSITION | DISP_CLRLINE);
				KEY_Flush_FIFO();
				
				long temp_value;
				temp_value=delay_and_wait_key(30,EXIT_KEY_F1,30);
				if((EXIT_KEY_F1 == temp_value) && (cLoop == 3))
				{
					DispStr_CE(0,4,"正在注销，请稍等.",DISP_CENTER|DISP_CLRSCR);
					DisConnectServer();
					//退出 
					//break;
					return; 	
				}
				else
				{
					//判断是否联接服务器
					RET = TCP_Check_Link();
					if(RET != 0)
					{
						//重新联接服务器,只尝试一次
						ConnectServer();
					}					
					continue;
				}
			}
			else
			{break;}
		}
		
		memset(data, '\0', sizeof(data));
		//发送成功 && 接收返回值
		RET = GetRecvData(data);
		if(RET<0){ //接收失败
			WarningBeep(2);
			//DispStr_CE(0,4,"上传成功!!",DISP_POSITION|DISP_CLRSCR); 
			DispStr_CE(0,4,"接收返回数据失败",DISP_POSITION|DISP_CLRSCR); 
			//DispStr_CE(0,36,"【F1退出】",DISP_POSITION | DISP_CLRLINE);
			KEY_Flush_FIFO();
			delay_and_wait_key(30, EXIT_KEY_ALL, 30);
			break;
		}
		else
		{
			//处理接收消息
			RET =HandleRecvData(data);
			if(RET<0){
				//dbClean();
				/*WarningBeep(2);
				DispStr_CE(0,4,"no data",DISP_POSITION|DISP_CLRSCR);
				KEY_Flush_FIFO();
				delay_and_wait_key(0,EXIT_KEY_F1,0);
				*/
			}else if(RET == 0){
				dbClean();
			}else if(RET == 1){//用户名错误
				WarningBeep(2);
				DispStr_CE(0,4,"用户名错误",DISP_POSITION|DISP_CLRSCR);
				DispStr_CE(0,6,"请确认后再提交，谢谢使用！",DISP_POSITION);
				DispStr_CE(0,36,"【F1退出】",DISP_POSITION | DISP_CLRLINE);
				KEY_Flush_FIFO();
				delay_and_wait_key(0,EXIT_KEY_F1,0);

				break;
			}else if(RET == 2){//用户密码错误 
				WarningBeep(2);
				DispStr_CE(0,4,"用户密码错误",DISP_POSITION|DISP_CLRSCR);
				DispStr_CE(0,6,"请确认后再提交，谢谢使用！",DISP_POSITION);
				DispStr_CE(0,36,"【F1退出】",DISP_POSITION | DISP_CLRLINE);
				KEY_Flush_FIFO();
				delay_and_wait_key(0,EXIT_KEY_F1,0);

				break;
			}else if(RET == 3){//退出
				//更新数据库
				UpdateDatabase(data);
			}
		}
	}//loop while

	//free
	DispStr_CE(0,4,"正在注销，请稍等..",DISP_CENTER|DISP_CLRSCR);
	DisConnectServer();
 }

void FormatDatabase(){
	long key_value = 0;
	WarningBeep(2);
	DispStr_CE(0,4,"温馨提醒",DISP_CENTER|DISP_CLRSCR);

	EXT_Display_Multi_Lines("该操作将完全覆盖现有数据，并不能恢复现有数据，请谨慎操作!",8,16);
 	DispStr_CE(0,36,"【F1退出】     【F3确定】",DISP_CENTER | DISP_CLRLINE); 	
	key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
	if((key_value == EXIT_KEY_F1)||(key_value == EXIT_AUTO_QUIT))	{
		return ;
	}else if(key_value == EXIT_KEY_F3){
		int i =0;
		while(i<3){
			if(CreateDatabase()==0){
				WarningBeep(0);
				DispStr_CE(0,4,"格式化成功，任意键退出",DISP_CENTER|DISP_CLRSCR);
				delay_and_wait_key(30,EXIT_KEY_ALL|EXIT_AUTO_QUIT,30);
				break;
			}else{
				i++;
			}
		}

		if(i ==3){
			WarningBeep(2);
			DispStr_CE(0,4,"格式化失败，任意键退出",DISP_CENTER|DISP_CLRSCR);
			delay_and_wait_key(30,EXIT_KEY_ALL|EXIT_AUTO_QUIT,30);
			return ;
		}
	}
}

//选择SIM卡商
void SelectCarriers(void)
{
	int lPtr = 0;
	int cPtr = 0;
	int ret = 0;
	char MenuT[16 * 2 + 2] = {"01.移动SIM卡    02.联通SIM卡     \n"};

	cPtr = SysObj.SimCard;	
	if((cPtr != 0) && (cPtr != 1))
	{		
		DispStr_CE(0,10,"SIM卡运营商参数错误",DISP_CENTER|DISP_CLRSCR);	
		DispStr_CE(0,12,"强制设置为移动",DISP_CENTER);			
		delay_and_wait_key(30,EXIT_KEY_F2,30);			

		cPtr = 0;
		SysObj.SimCard = 0;
		User_Param_Write(0, (unsigned char *)&SysObj, sizeof(SysPara));		
	}
	while(1)
	{ 	
		clr_scr();
		DispStr_CEEX(0, 0, "请选择电信商", WHITE, BLUE, DISP_CENTER);
		ret = browse_menu(2, MenuT, &cPtr, &lPtr, 16, 2);
		switch(ret)
		{
			case 0:
			case 1:
			{
				if(SysObj.SimCard != ret)
				{
					SysObj.SimCard = ret;
					User_Param_Write(0, (unsigned char *)&SysObj, sizeof(SysPara));
				}
				
				DispStr_CE(0, 8, "设置成功", DISP_CENTER | DISP_CLRSCR);
				delay_and_wait_key(30, EXIT_KEY_ALL, 30);				
			}break;

			case -2:
			{return;}
		}
	}	
	
}

void  SysSetMenu(){
	int ret = 0; 
	int LineLen = 26;
	SMemu SMemuObj =
	{
		1, 4, 1,
		{		
			{
				{12, 64,	SetTime,	0}, {88, 64, FormDB, 0}, {164, 64, SCard, 0}, 
				{12, 140, ExitSet, 0},
			},	
		},
		{4, 0, 0},0, 0,
	};		

	char MAIN_MENU[7 * 26 + 2] = 
		{
			"1. 同步时间               "
			"2. 格式数据表             "
			"3. 选择电信商             "
			"4. 退出设置               "
		};


	while(1){
		Disp_Clear();

		ret = Browse_Icon("请选择功能模块", MAIN_MENU, &SMemuObj, LineLen, 1, 0, 0);
		switch(ret){
			case 0:{
				SynTime();
				//Modify_Date();
				//Modify_Time();
				break;
			}
			case 1:{
				FormatDatabase(); 
				break;
			}

			//选择电信商
			case 2:{
				SelectCarriers();
				break;
			}
			
			case 3:{
				return;
				break;
			}
			
			case -1:{
				return;
				}break;
			
			default:{
				break;
			}
		}
	}
} 

void GetInfo(){ //标签校验 
	long key_value = 0;
	int flag = 1;
 
	while(flag){
		memset(anticode,0,ANTIFAKECODE_LEN);//清空防伪码 
		short err = ultralight_local_read_card(anticode);

		//获取防伪码
		if(err ==0){ 
			DispStr_CE(0,4,"标签读取成功",DISP_CENTER|DISP_CLRSCR);
			DispStr_CE(0,8,"校 验 码：",DISP_POSITION);
			DispStr_CE(0,8,anticode,DISP_CURRENT);

 			DispStr_CE(0,36,"【F1退出】     【F3继续】",DISP_CENTER | DISP_CLRLINE); 
			key_value = delay_and_wait_key(0,EXIT_KEY_F1|EXIT_KEY_F3,0); 
			switch(key_value){
				case EXIT_KEY_F1:{
					flag = 0;//退出循环 
					break;
				}
				case EXIT_KEY_F3:{
					break; 
				} 
			}

		}else{
			flag =0;
		}
	}
}

//======================================================================
//函数名：MainMenu 
//功能  ：主菜单界面 
//参数  ：无 
//返回值：无 
//======================================================================
void MainMenu(){
	//初始化菜单 
	short ret = 0;
	int LineLen = 26;
	char Temp[40];
	SMemu SMemuObj =
	{
		1, 5, 1,
		{		
			{
				{12, 64,	Offline,	0}, {88, 64, Upload, 0}, {164, 64, Check, 0}, 
				{12, 140, SysSet, 0}, {88, 140, VerInfo, 0},
			},	
		},
		{5, 0, 0},0, 0,
	};		
	
	char MAIN_MENU[7 * 26 + 2] = 
		{
			"1. 离线巡检               "
			"2. GPRS上传               "
			"3. 标签校验               "
			"4. 系统设置               "
			"5. SFV2.13                "
		};

	Disp_Clear();
	while(1)
	{
		Disp_Clear();
		memset(Temp, '\0', sizeof(Temp));
		if(SysObj.SimCard == 0)
		{sprintf(Temp, "请选择功能模块 %s", "移动");}
		else
		{sprintf(Temp, "请选择功能模块 %s", "联通");}			
		ret = Browse_Icon(Temp, MAIN_MENU, &SMemuObj, LineLen, 1, 0, 0);
		switch(ret){
			
			//巡检
			case 0:{				
				Query(); 
				break;
			}

			//提交数据 
			case 1:{				
				SubmitData();
				break;
			}

			//标签校验
			case 2:{
				GetInfo();
				break; 
			}

			//系统设置
			case 3:{  
				SysSetMenu();
			} 

			default :{
				break;
			}
		}
	} 
}

