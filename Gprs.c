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
  #include"Gprs.h"
 void sim300_close(void) //关闭gprs模块 
{
      DispStr_CE(0,4,"正在关闭gprs模块，请稍等",DISP_CENTER|DISP_CLRSCR);
      sim_module_close();
}
void DisConnectServer() //断开与服务器连接并关闭gprs模块 
{
       DispStr_CE(0,4,"正在注销，请稍等",DISP_CENTER|DISP_CLRSCR);
       tcp_send_data("*3#\n",strlen("*3#\n"));
       sim300_close();
}
 short sim300_init(void)   //连接网络，返回1表示成功，0 表示失败 
{
	short ret;
	DispStr_CE(0,0,"网络模块初始化...",DISP_CENTER|DISP_CLRSCR);
	ret = sim_module_init();
	if(ret<0)
	{
	        WarningBeep(2);
		if( g_simerr == SIM_POSTYPE_ERROR )
		{
			DispStr_CE(0,2,"此机型不支持GPRS",DISP_POSITION);
		}
		if( g_simerr == SIM_MODULE_ERROR )
		{
			DispStr_CE(0,2,"请检查模块连接",DISP_POSITION);
			DispStr_CE(0,4,"关闭模块...",DISP_POSITION);
			sim_module_close();
		}
		else if( g_simerr == SIM_NO_CARD )
		{
			DispStr_CE(0,2,"请检查SIM卡连接",DISP_POSITION);
			DispStr_CE(0,4,"关闭模块...",DISP_POSITION);
			sim_module_close();
		}
		 return 0;
	}
	else
	{
		DispStr_CE(0,2,"模块初始化成功",DISP_CENTER);
		DispStr_CE(0,4,"连接网络...",DISP_CENTER);		
		ret = sim_detect_net();
		if(ret<0)
		{
			DispStr_CE(0,6,"连接网络失败",DISP_POSITION);
			 WarningBeep(2);
			sim_module_close();
			return 0;
			
		}
		else
		{
			DispStr_CE(0,6,"连接网络成功",DISP_CENTER);
			return 1;
		}
	}
		
}



short ConnectServer()//连接服务器   0表示连接成功，-1 表示连接失败 
{
         short ret ;
         DispStr_CE(0,10,"连接服务器，请稍等...",DISP_CENTER|DISP_CLRSCR);	
            ret = tcp_create_link("118.123.244.109","8000");
            if(ret < 0)
	    {
		if( g_simerr == SIM_NO_ECHO )
		{
			DispStr_CE(0,6,"模块无响应",DISP_CENTER);
			delay_and_wait_key(0,EXIT_KEY_F2,0);
		}
		else if( g_simerr == SIM_COMMAND_ERROR )
		{
			DispStr_CE(0,6,"命令错误",DISP_CENTER);
			delay_and_wait_key(0,EXIT_KEY_F2,0);
		}
		else if( g_simerr == SIM_LINK_ERROR )
		{
			DispStr_CE(0,6,"连接失败",DISP_CENTER);
			delay_and_wait_key(0,EXIT_KEY_F2,0);
		}
		 WarningBeep(2);
		 sim_module_close();
		return -1;
	    }
	    else if(ret ==0)//连接成功 
	    {
	        return 0;
	           /* DispStr_CE(0,10,"连接服务器成功",DISP_CENTER|DISP_CLRSCR);
	            DispStr_CE(0,12,"正在上传，请稍等...",DISP_CENTER|DISP_CLRSCR);  
	             if(tcp_send_data(data,strlen(data))==0)//发送成功 
                     {    
                       // DispStr_CE(0,12,"发送成功",DISP_CENTER|DISP_CLRSCR);  
                        //delay_and_wait_key(0,EXIT_KEY_F2,0);                         
                      return 0;      
                     }
                     else
                     { 
                     WarningBeep(2);
                     DispStr_CE(0,10,"发送失败",DISP_CENTER|DISP_CLRSCR);
                     delay_and_wait_key(1,EXIT_KEY_F2|EXIT_AUTO_QUIT,1);
                     sim_module_close();
                     return -1;
                     } 
                     */
	    }
} 


short SendData(unsigned char* data)    //发送数据    0 表示发送成功 -1 表示失败 
{
     DispStr_CE(0,12,"正在上传，请稍等...",DISP_CENTER|DISP_CLRSCR);  
     if(tcp_send_data(data,strlen(data))==0)//发送成功 
     {    
         /* DispStr_CE(0,12,"发送成功",DISP_CENTER|DISP_CLRSCR);  
         delay_and_wait_key(0,EXIT_KEY_F2,0);  
         */                       
         return 0;      
     }
     else
     { 
       //  WarningBeep(2);
        // DispStr_CE(0,10,"发送失败",DISP_CENTER|DISP_CLRSCR);
         //delay_and_wait_key(10,EXIT_KEY_F2|EXIT_AUTO_QUIT,10);
         return -1;
     } 
} 

 short GetRecvData(char* recvdata)     //接收服务器返回的结果 0 表示接收成功，-1 表示返回失败 
  {
                          short len;
                          int ret;
                        
                              ret = tcp_recv_data(recvdata,&len,256);
                              
                              if(ret == 0)                               
                              {
                              // DispStr_CE(0,10,"接收成功",DISP_CENTER|DISP_CLRSCR);
                                //delay_and_wait_key(10,EXIT_KEY_F2,10);
                                 return 0;
                              }
                              else
                              {
                                      
                                DispStr_CE(0,10,"接收失败",DISP_CENTER|DISP_CLRSCR);
                                delay_and_wait_key(10,EXIT_KEY_F2,10);
                                      delay_and_wait_key(1,EXIT_KEY_F2|EXIT_AUTO_QUIT,1);
                                WarningBeep(2);
                                     return -1; 
                              } 
                            
  }
