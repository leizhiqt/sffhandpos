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

/*
*Display API
*/

short browse_info(int startline,char *p_menu,int *p_cPtr,int *p_lPtr,short flag ){
	BROWINFO	bi;
	short	ret;
	if(flag ==0){
		Disp_Clear();
		DispStr_CE(0,0,"提交记录信息如下",DISP_CENTER);
		Disp_Goto_XY(0,36);
		DispStr_CE(0,36,"【F1退出提交】",DISP_CURRENT);
		DispStr_CE(0,36,"【F3确认提交】",DISP_RIGHT);
	}else if(flag ==1){
		Disp_Clear();
		DispStr_CE(0,0,"错误记录信息如下",DISP_CENTER);
		Disp_Goto_XY(0,36);
		DispStr_CE(0,36,"【F3退出浏览】",DISP_RIGHT);
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
	WarningBeep(2);
	DispStr_CE(0,2,"特别提醒",DISP_CLRSCR|DISP_CENTER);
	EXT_Display_Multi_Lines("此功能要通过GPRS通信，可能会有危险，请确定环境是否安全！",6,10);
	EXT_ClearLine(36,0);
	Disp_Goto_XY(0,36);
	DispStr_CE(0,36,"【F1退出】",DISP_CURRENT);
	DispStr_CE(0,36,"【F3继续】",DISP_RIGHT);
	long key_value; 
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
	Sys_Auto_Poweroff_Set(60*2);		//Power off in 2 minutes if not do anything
	
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
		WarningBeep(2);
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return ; 
	}

	//查找数据库成功
	DispStr_CE(0,4,"请刷卡登陆",DISP_CENTER|DISP_CLRSCR);
	EXT_Display_Multi_Lines("请将您的身份卡放在感应区下方，然后按F3登陆键进行登陆操作，如果想退出可以按F1退出！",8,16);
	EXT_ClearLine(36,0);
	Disp_Goto_XY(0,36);
	DispStr_CE(0,36,"【F1退出】",DISP_CURRENT);
	DispStr_CE(0,36,"【F3登陆】",DISP_RIGHT);

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

	//EXT_Display_Multi_Lines("按任意键进行巡检",8,10);
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
			char record[200];
			memset(record,0,200); 
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

			EXT_ClearLine(36,0);
			Disp_Goto_XY(0,36);
			DispStr_CE(0,36,"【F1退出】",DISP_CURRENT);
			DispStr_CE(0,36,"【F3继续】",DISP_RIGHT);
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

	RET = sim300_init();//连接网络
	if(RET ==0){//连接网络成功
		RET = ConnectServer();//连接服务器 
		if(RET == 0){//连接服务器成功
			return 0; 
		}
	}

	DispStr_CE(0,4,"连接服务器失败，任意键退出",DISP_CENTER|DISP_CLRSCR);
	//WarningBeep(2);
	sim300_close();
	// delay_and_wait_key(0,EXIT_KEY_ALL,0);
	return -1;
}

//======================================================================
//函数名：SubmitData 
//功能  ：完成数据提交 
//参数  ：无 
//返回值：无 
//======================================================================
void SubmitData(){

	long choose ;
	int cLoop =0;

	int i=0;
	
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
	EXT_ClearLine(36,0);
	Disp_Goto_XY(0,36);
	DispStr_CE(0,36,"【F1退出】",DISP_CURRENT);
	DispStr_CE(0,36,"【F3登陆】",DISP_RIGHT);
	long key_value; 
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

	int RET = LoginToSubmit();
/*	DispStr_CE(0,0,"ReadNameAndPassword",DISP_CENTER|DISP_CLRSCR);

	char tmpbuf[100];
	sprintf(tmpbuf,"RET: %d",RET);
	DispStr_CE(0,0,tmpbuf,DISP_POSITION|DISP_5x7);
*/
	if(RET != 0){
		DispStr_CE(0,0,"刷卡失败!",DISP_CENTER|DISP_CLRSCR);
		return;
	}

	//刷卡登陆成功   获取了登陆用户名和密码
	int maxs=60*PAGENUM+5;
	int maxr=6*PAGENUM+5;

	unsigned char senddata[maxs];
	unsigned char recvdata[maxr];

	memset(senddata,0,maxs);
	memset(recvdata,0,maxr);

	//首次检查数据
	RET=EncodeSendData(sname,spass,senddata);
	if(RET<0){
		CreateDatabase();

		WarningBeep(0); 
		DispStr_CE(0,0,"没有数据提交，按任意键退出",DISP_CENTER|DISP_CLRSCR);
		delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);

		return; 
	}

	//打开GPRS模块 && 服务器
	for(i=0;i<3;i++){
		if((RET=OpenGPRS())==0){
			break;
		}
	}

	//GPRS打开失败
	if(RET != 0){ 
		DispStr_CE(0,0,"请检查SIM卡或通信模块",DISP_CENTER|DISP_CLRSCR); 
		DispStr_CE(0,2,"按任意键退出",DISP_CENTER); 
		delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);

		return;
	}

	int upCount =0;
	while(1){
		memset(senddata,0,maxs);
		memset(recvdata,0,maxr);

		//暂存没有数据
		RET=EncodeSendData(sname,spass,senddata);

		if(RET<0){
			CreateDatabase();

			WarningBeep(0); 
			DispStr_CE(0,0,"数据上传完毕，按任意键退出",DISP_CENTER|DISP_CLRSCR);
			delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);

			break; 
		}

		//上传数据
		WarningBeep(0);
		RET = Display(0);
		if(RET == -1){//退出 
			break;
		}

		upCount++;
		//发送数据
		RET = SendData(senddata);
		if(RET==-1){
			//发送失败
			WarningBeep(2);

			DispStr_CE(0,0,"网络断开，请重新连接",DISP_CENTER|DISP_CLRSCR);

			Disp_Goto_XY(0,36);
			DispStr_CE(0,36,"【F1退出重连】",DISP_CURRENT);

			long temp_value; 
			temp_value=delay_and_wait_key(30,EXIT_KEY_F1,30);
			if(EXIT_KEY_F1 == temp_value || cLoop==3){//退出 
				break;
			}
			continue;
		}

		//发送成功 && 接收返回值
		RET =  GetRecvData(recvdata);
		if(RET !=0){ //接收失败
			WarningBeep(2); 
			DispStr_CE(0,4,"网络异常 稍后再试!",DISP_CENTER|DISP_CLRSCR);
			Disp_Goto_XY(0,36);
			DispStr_CE(0,36,"【F1退出】",DISP_CURRENT);
			delay_and_wait_key(0,EXIT_KEY_F1,0);

			break;
		}

		//处理接收消息
		RET =HandleRecvData(recvdata);
		if(RET == 1){//用户名错误 
			WarningBeep(2); 
			DispStr_CE(0,4,"用户名错误",DISP_CENTER|DISP_CLRSCR);
			DispStr_CE(0,6,"请确认后再提交，谢谢使用！",DISP_CENTER);
			Disp_Goto_XY(0,36);
			DispStr_CE(0,36,"【F1退出】",DISP_CURRENT);
			delay_and_wait_key(0,EXIT_KEY_F1,0);

			break;
		}else if(RET ==2){//用户密码错误 
			WarningBeep(2);
			DispStr_CE(0,4,"用户密码错误",DISP_CENTER|DISP_CLRSCR);
			DispStr_CE(0,6,"请确认后再提交，谢谢使用！",DISP_CENTER);
			Disp_Goto_XY(0,36);
			DispStr_CE(0,36,"【F1退出】",DISP_CURRENT);
			delay_and_wait_key(0,EXIT_KEY_F1,0);

			break;
		}else if(RET ==3){//退出 

			break;
		}else{
			WarningBeep(0);
			DispStr_CE(0,0,"按任意键提交下一批",DISP_CENTER|DISP_CLRSCR);
			delay_and_wait_key(30,EXIT_AUTO_QUIT|EXIT_KEY_ALL,30);
		}
	}//loop while

	//free
	sim300_close();
	DisConnectServer();
}

void FormatDatabase(){
     WarningBeep(2);
     DispStr_CE(0,4,"温馨提醒",DISP_CENTER|DISP_CLRSCR);
     
     EXT_Display_Multi_Lines("该操作将完全覆盖现有数据，并不能恢复现有数据，请谨慎操作!",8,16);
     EXT_ClearLine(36,0);
     Disp_Goto_XY(0,36);
     DispStr_CE(0,36,"【F1取消】",DISP_CURRENT);
     DispStr_CE(0,36,"【F3确定】",DISP_RIGHT);  
     long key_value ;
     key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
     if((key_value == EXIT_KEY_F1)||(key_value == EXIT_AUTO_QUIT))
     {
          return ;                             
     }
     else if(key_value == EXIT_KEY_F3)
     {
         int i =0;
         while(i<3)
         {
           if(CreateDatabase()==0)
           {
               WarningBeep(0);
               DispStr_CE(0,4,"格式化成功，任意键退出",DISP_CENTER|DISP_CLRSCR);
               delay_and_wait_key(30,EXIT_KEY_ALL|EXIT_AUTO_QUIT,30);
              break;
           }
           else
           {
             i++;
           }
         }
         if(i ==3)
         {
                WarningBeep(2);
               DispStr_CE(0,4,"格式化失败，任意键退出",DISP_CENTER|DISP_CLRSCR);
               delay_and_wait_key(30,EXIT_KEY_ALL|EXIT_AUTO_QUIT,30);
              return ;
         }
         
     }
     

}

void  SysSetMenu()
{
    int ret;
    int flag =1;
         BROWINFO  sys_set_menu;
        char MAIN_MENU[] = "1. 设置时间  2. 格式数据表3. 退出设置  "; 
        char welcome[20];
        char choose[20];
        memset(welcome,0,20);
        memset(choose,0,20);
        strcpy(welcome ,"欢迎使用");
        strcpy(choose,"请选择功能模块");
        
       
        sys_set_menu.iStr = MAIN_MENU;
        sys_set_menu.lPtr = 0;
        sys_set_menu.cPtr = 0;
        sys_set_menu.startLine = 10;
        while(flag)
        {
           Disp_Clear();
           
           DispStr_CE(0,4,welcome,DISP_CENTER|DISP_CLRSCR);
           DispStr_CE(0,6,choose,DISP_CENTER);
           
           
           sys_set_menu.dispLines = 3;
           sys_set_menu.mInt = 3;
           sys_set_menu.lineMax = 13;
           sys_set_menu.sFont = 0;
           sys_set_menu.numEnable = 1;
           sys_set_menu.qEvent = EXIT_KEY_F1 ; 
            Disp_Set_Magnification(2);
           ret = EXT_Brow_Select(&sys_set_menu);
            Disp_Set_Magnification(1);
         
            
           switch(ret)
           {
              case 0:
              {
                 Modify_Date();
                 Modify_Time();
                break;
              }
              case 1:
              {
                  FormatDatabase(); 
                 break;
              }
              case 2:
              {
                flag =0;
                break;
              }
              default:
              {
                break;
              }
           }
        }
} 

void GetInfo(){ //标签校验 

	long key_value;
	int flag = 1;
	//int index=0; 

	while(flag){
		memset(anticode,0,ANTIFAKECODE_LEN);//清空防伪码 
		short err = ultralight_local_read_card(anticode);

		//获取防伪码
		if(err ==0){
			//Disp_Set_Magnification(3);
			//Disp_Set_Magnification(1);

			DispStr_CE(0,4,"标签读取成功",DISP_CENTER|DISP_CLRSCR);
			DispStr_CE(0,8,"校 验 码：",DISP_POSITION);
			DispStr_CE(0,8,anticode,DISP_CURRENT);

			EXT_ClearLine(36,0);
			Disp_Goto_XY(0,36);
			DispStr_CE(0,36,"【F1退出】",DISP_CURRENT);
			DispStr_CE(0,36,"【F3继续】",DISP_RIGHT);

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
	short ret;
	BROWINFO  main_menu;
	char MAIN_MENU[] = "1. 离线巡检2. GPRS上传3. 标签校验4. 系统设置5. SFV1.6"; 
	char welcome[20];
	char choose[20];
	memset(welcome,0,20);
	memset(choose,0,20);
	strcpy(welcome ,"欢迎使用");
	strcpy(choose,"请选择功能模块");
	main_menu.iStr = MAIN_MENU;
	main_menu.lPtr = 0;
	main_menu.cPtr = 0;
	main_menu.startLine = 10;

	while(1){
		Disp_Clear();
		DispStr_CE(0,4,welcome,DISP_CENTER|DISP_CLRSCR);
		DispStr_CE(0,6,choose,DISP_CENTER);

		main_menu.dispLines = 5;
		main_menu.mInt = 5;
		main_menu.lineMax = 11;
		main_menu.sFont = 0;
		main_menu.numEnable = 1;
		main_menu.qEvent = EXIT_KEY_F1 ;

		Disp_Set_Magnification(2);
		ret = EXT_Brow_Select(&main_menu);
		Disp_Set_Magnification(1);

		switch(ret){
			case 0:{
				//巡检
				Query(); 
				break;
			}
			case 1:{
				//提交数据 
				SubmitData();
				break;
			}
			case 2:{
				GetInfo();
				break; 
			} 
			case 3:{  
				SysSetMenu();
			} 
                       
                        default :{
				break;
			}
		}
	} 
}

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
