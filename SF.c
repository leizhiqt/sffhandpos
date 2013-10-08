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
				for(i = 0; i < 400; i++)
				{
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
//函数名：SubmitData 
//功能  ：完成数据提交 
//参数  ：无 
//返回值：无 
//======================================================================
void SubmitData(){
	int cLoop =0;
	int i=0;
	int RET = 0;	
	long choose = 0;
	long key_value = 0; 
	unsigned char data[1024];
	
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
	if(RET<0){
		CreateDatabase();

		WarningBeep(0); 
		DispStr_CE(0,2,"没有数据提交，按任意键退出",DISP_POSITION|DISP_CLRSCR);
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
		if(RET<1){
			DB_erase_filesys(0);
			//CreateDatabase();

			WarningBeep(0);
			DispStr_CE(0,2,"所有数据上传完毕，按任意键退出",DISP_POSITION|DISP_CLRSCR);
			KEY_Flush_FIFO();
			delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);

			break;
		}

		//上传数据
		WarningBeep(0);
		RET = Display(0);
		if(RET == -1){//退出 
			break;
		}

		//发送数据
		RET = SendData(data);

		if(RET==-1){
			//发送失败
			cLoop++;
			
			WarningBeep(2);
			DispStr_CE(0,4,"数据发送失败!",DISP_POSITION|DISP_CLRSCR);
			DispStr_CE(0,36,"【F1退出重连】",DISP_POSITION | DISP_CLRLINE);
			KEY_Flush_FIFO();
			
			long temp_value;
			temp_value=delay_and_wait_key(30,EXIT_KEY_F1,30);
			if((EXIT_KEY_F1 == temp_value) && (cLoop == 3)){//退出 
				break;
			}

			continue;
		}

		memset(data, '\0', sizeof(data));
		//发送成功 && 接收返回值
		RET = GetRecvData(data);
		if(RET<0){ //接收失败
			WarningBeep(2);
			DispStr_CE(0,4,"上传成功",DISP_POSITION|DISP_CLRSCR); 
			//DispStr_CE(0,36,"【F1退出】",DISP_POSITION | DISP_CLRLINE);
			KEY_Flush_FIFO();
			delay_and_wait_key(0,EXIT_KEY_ALL,0);
			//break;
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

void  SysSetMenu(){
	int ret = 0; 
	int LineLen = 26;
	SMemu SMemuObj =
	{
		1, 3, 1,
		{		
			{
				{12, 64,	SetTime,	0}, {88, 64, FormDB, 0}, {164, 64, ExitSet, 0}, 
			},	
		},
		{3, 0, 0},0, 0,
	};		

	char MAIN_MENU[7 * 26 + 2] = 
		{
			"1. 设置时间               "
			"2. 格式数据表             "
			"3. 退出设置               "
		};


	while(1){
		Disp_Clear();

		ret = Browse_Icon("请选择功能模块", MAIN_MENU, &SMemuObj, LineLen, 1, 0, 0);
		switch(ret){
			case 0:{
				Modify_Date();
				Modify_Time();
				break;
			}
			case 1:{
				FormatDatabase(); 
				break;
			}
			
			case 2:{
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
			"5. SFV2.08                "
		};

	Disp_Clear();
	while(1){
		Disp_Clear();
		ret = Browse_Icon("请选择功能模块", MAIN_MENU, &SMemuObj, LineLen, 1, 0, 0);
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

