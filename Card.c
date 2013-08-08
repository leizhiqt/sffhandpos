/****************************************************************************
*
*文件(File):         Card.c
*
*修改(Modify):       2012-7-13 14:15:56
*
*作者(Author):       sa
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
| V1.00  2012-7-13 14:15:56           sa 
----------------------------------------------------------------------------
****************************************************************************/
#include "API.h"

#include "Card.h"

/**
*0
*!=0 error
*/
int  ReadNameAndPassword(unsigned char* nname,unsigned char* npass){

	long key_value; 
        
        unsigned char dummy_key[6] = "\xFF\xFF\xFF\xFF\xFF\xFF";
	
        unsigned char 	MATQ[3];	//ATQ(2 bytes)
	unsigned char 	MSNO[5];	//Serial number(4 bytes)
	unsigned char 	MATS[2];	//mifare1:0x08(1 byte)
	unsigned char databuf[30];
	memset(databuf,0,30); 

	int forever = 1;
	while(forever){
		Disp_Clear();
		Disp_Goto_XY(0,36);
		DispStr_CE(0,36,"【F1退出】",DISP_CURRENT);
		DispStr_CE(0,36,"【F3重试】",DISP_RIGHT);

		DispStr_CE(0,2,"正在初始化读卡模块M1...",DISP_CENTER);

		if(RCX_Init(CARD_TYPE_14443A)!=RCX_OK){//初始化失败
			RCX_Close();

			EXT_ClearLine(2,0); 
			DispStr_CE(0,4,"模块初始化失败",DISP_CENTER);
			RCX_Close();
			DispStr_CE(0,6,"继续或退出",DISP_CENTER);
			key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
			switch(key_value){
				case EXIT_KEY_F1 :
				case EXIT_AUTO_QUIT:{
					return -1;
				}
				case EXIT_KEY_F3:{
					break;
				}
			}
			continue;
		}

		//初始化成功 
		DispStr_CE(0,4,"初始化读卡模块成功",DISP_CENTER); 
		DispStr_CE(0,8,"正在读卡...",DISP_CENTER); 

		//读卡
		char RET = CardTypeARequest(PICC_REQALL,MATQ); //may be Halt
		if(RET!=RCX_OK){
			RET = CardTypeARequest(PICC_REQALL,MATQ);//maybe active
		}

		if(RET==RCX_OK){
			RET = CardTypeAAnticollLevel1(MSNO);
		}
		if(RET==RCX_OK){
			RET = CardTypeASelectLevel1(MSNO,MATS);
		}

		//读卡失败
		if(RET!=MI_OK){
			RCX_Close();

			EXT_ClearLine(10,0); 
			DispStr_CE(0,12,"读卡失败",DISP_CENTER);
			WarningBeep(2);
			key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
			switch(key_value){
				case EXIT_KEY_F1 :{
					return -1;
				}
				case EXIT_AUTO_QUIT:
				case EXIT_KEY_F3:{
					break;
				}
			}
			continue;
		}

		//读卡成功
		//读用户名失败
		RET=CardMFCAuthKey(PICC_AUTHENT1A,MSNO,dummy_key,5);
		if(RET==RCX_OK) RET = CardMFCRead16Bytes(5,nname);
		
		if(RET!=RCX_OK){
			RCX_Close();

			EXT_ClearLine(10,0); 
			WarningBeep(2);
			DispStr_CE(0,10,"读用户名失败",DISP_CENTER);
			key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
			switch(key_value){
				case EXIT_KEY_F1 :
				case EXIT_AUTO_QUIT:{
					return -3;//退出循环 回到主菜单
				}
				case EXIT_KEY_F3:{
					break;
				}
			}
			continue;
		}

		//读取用户名成功 & 读密码失败
		if((CardMFCAuthKey(PICC_AUTHENT1A,MSNO,dummy_key,6)!=RCX_OK)
			||(CardMFCRead16Bytes(6,npass)!=RCX_OK)){
			RCX_Close();

			EXT_ClearLine(10,0); 
			DispStr_CE(0,10,"读密码失败",DISP_CENTER);
			key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
			switch(key_value){
				case EXIT_KEY_F1 :
				case EXIT_AUTO_QUIT:{
					return -2;
				} 
				case EXIT_KEY_F3:{
					break;
				}
			}
			continue;
		}

		//验证成功
		RCX_Close();
		return 0;
	}//end while

	RCX_Close();
	return -4;
}

short LoginByCard(){
	memset(username,0,USERNAME_LEN);//清空用户名 
	memset(password,0,USERNAME_LEN);

	int RET=-1;
	RET = ReadNameAndPassword(username, password);

	//读取成功 验证 
	if(RET==0) return 0; 

	return -1;
}

short LoginToSubmit(){
	memset(sname,0,USERNAME_LEN);
	memset(spass,0,USERNAME_LEN);

	int RET=-1;
	RET = ReadNameAndPassword(sname, spass);
	//读取成功 验证 
	if(RET==0) return 0; 

	return -1;
} 

short ultralight_init(){
   short err ;
   err = RCX_Init(CARD_TYPE_14443A);
   if(err == RCX_OK)
   {
       err = 1;
   }
   else
   {
        err = 0;
   }
   return  err; 
}

void ultralight_close(){
  RCX_Close();
}

short ultralight_find_card(){
   short ret; 
   short err;
   unsigned char	sak;
// 开始寻卡 
        ret = CardTypeARequest(PICC_REQALL,ULATQ); 
       	if( (ret)||(ULATQ[0]!=0x44 )||(ULATQ[1]!=0x00 ) )
	{		
		err = 0;
		return err;
	} 
	ret = CardTypeAAnticoll(PICC_ANTICOLL1,ULSNO);
	ret |= CardTypeASelect(PICC_ANTICOLL1,ULSNO,&sak);		
	if(sak&0x04)
	{		
		ret |= CardTypeAAnticoll(PICC_ANTICOLL2,ULSNO);	
		ret |= CardTypeASelect(PICC_ANTICOLL2,ULSNO,&sak);			
		if(sak&0x04)
		{		
			ret |= CardTypeAAnticoll(PICC_ANTICOLL3,ULSNO);				
			ret |= CardTypeASelect(PICC_ANTICOLL3,ULSNO,&sak);			
		}
	}
	else
	{
	   err = 0;
	   return err;
	}
	if( ret ) 
        {
          err = 0;
          return err;
        }
        err =1;
        return err;
}
short ultralight_read_card(unsigned char block_add,unsigned char* databuf)
{
  short err ;
  err = CardMFCRead16Bytes(block_add,databuf);
  if( err != RCX_OK)
  {
         ultralight_close();
         err = 0;
        
  }
  else
  {
    ultralight_close();
    err = 1;
  }
   return err;
}

short ultralight_local_read_card(char* ac){
	short flag ;
	short init_err;
	short find_card_err;
	short init_flag; 
	short read_card_err;
	long  key_value; 
	unsigned char block_add;
	unsigned char databuf[30];
	memset(databuf,0,30);
	block_add = 4;

	flag = 1;
	init_flag =0;
   while(flag)
   {
     Disp_Clear();
     Disp_Goto_XY(0,36);
     DispStr_CE(0,36,"【F1退出】",DISP_CURRENT);
     DispStr_CE(0,36,"【F3重试】",DISP_RIGHT);
    
     
      if(init_flag == 0)
      {
        DispStr_CE(0,2,"正在初始化读卡模块UL...",DISP_CENTER); 
        init_err = ultralight_init();
        if(0 == init_err )  //初始化失败 
       {
        EXT_ClearLine(2,0); 
        DispStr_CE(0,4,"模块初始化失败",DISP_CENTER);
        ultralight_close();
        DispStr_CE(0,6,"继续或退出",DISP_CENTER);       
        key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
        switch(key_value)
        {
         case EXIT_KEY_F1 :               
         case EXIT_AUTO_QUIT:
          {
           init_flag =0;
           flag = 0;     //退出循环 回到主菜单 
           break;
          }
          case EXIT_KEY_F3:
          {
             init_flag =1;
             break; 
          }  
        } 
       }
       else//初始化成功 
       {
              DispStr_CE(0,4,"初始化读卡模块成功",DISP_CENTER); 
              DispStr_CE(0,6,"正在寻卡...",DISP_CENTER); 
              find_card_err = ultralight_find_card();
              if(0 == find_card_err)//寻卡失败
              {
                   EXT_ClearLine(8,0); 
                   DispStr_CE(0,8,"无卡！请核对？",DISP_CENTER);
                   WarningBeep(2);
                   key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
                   switch(key_value)
                   {
                      case EXIT_KEY_F1 :
                      case EXIT_AUTO_QUIT:
                      {
                             flag =0;     //退出循环 回到主菜单 
                             break;
                      } 
                      case EXIT_KEY_F3:
                      {
                         break;
                      }
                   } 
       
              }
              else//寻卡成功 准备读取卡 
              {
                    EXT_ClearLine(8,0); 
                    DispStr_CE(0,8,"寻卡成功",DISP_CENTER); 
                    DispStr_CE(0,10,"正在读卡...",DISP_CENTER); 
                    read_card_err = ultralight_read_card(block_add, databuf);
                    if( 0 == read_card_err)     //读卡失败 
                    {
                          EXT_ClearLine(10,0); 
                          DispStr_CE(0,10,"读卡失败",DISP_CENTER);
                           WarningBeep(2);
                          key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
                          switch(key_value)
                          {
                            case EXIT_KEY_F1 :
                            case EXIT_AUTO_QUIT:
                              {
                                 flag =0;     //退出循环 回到主菜单 
                                  break;
                               } 
                            case EXIT_KEY_F3:
                             {
                               break;
                              }
                           } 
                    }
                    else 
                    {
                         /* EXT_ClearLine(12,0); 
                          DispStr_CE(0,12,"读卡成功",DISP_CENTER);
                          EXT_ClearLine(16,0);
                          DispStr_CE(0,16,"防伪码为：",DISP_CENTER);
                          DispStr_CE(0,18,databuf,DISP_CENTER);
                           */
                          strcpy(ac,(char *)&databuf);
                          
                           WarningBeep(0);
                           return  0; 
                          
                          EXT_ClearLine(36,0);
                          Disp_Goto_XY(0,36);
                          DispStr_CE(0,36,"【F1退出】",DISP_CURRENT);
                          DispStr_CE(0,36,"【F3继续】",DISP_RIGHT);
                          key_value = delay_and_wait_key(30,EXIT_KEY_F1|EXIT_KEY_F3|EXIT_AUTO_QUIT,30);
                          switch(key_value)
                          {
                           case EXIT_KEY_F1:
                           case EXIT_AUTO_QUIT:
                           {
                              flag = 0;//退出循环 
                              return -1; 
                              break;
                           }
                           case EXIT_KEY_F3:
                           {
                             break; 
                           } 
                          } 
                    }
              } 
         }
      }
   } 
   return 0;
}
