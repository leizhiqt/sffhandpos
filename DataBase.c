/****************************************************************************
*
*文件(File):         DataBase.c
*
*修改(Modify):       2012-7-13 14:23:58
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
| V1.00  2012-7-13 14:23:58           sa 
----------------------------------------------------------------------------
****************************************************************************/
#include "DataBase.h"
#include "Common.h"
#include "Card.h"
#include "stdlib.h"
#include "stdio.h"

unsigned int flagarr[PAGENUM];
//======================================================================
//函数名：FindDatabase 
//功能  ：查找数据库 
//参数  ：无 
//返回值：short -1 表示起始地址失败，其他表示数据表的个数 
//======================================================================
short FindDatabase()
{
     short err;
     err = DB_init_sys_param(0);   
     if(err == 1)
     {
         err = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));
         if(err ==0)
         {
            return 0;
         }
         else
         {
          return -1;
         }
     }
     else
     {
        return -1; 
     }
}
//======================================================================
//函数名：CreateDatabase 
//功能  ：创建数据库 
//参数  ：无 
//返回值：short 
//======================================================================
short CreateDatabase()
{
   short ret;
   ret = DB_erase_db_sys();   //0 成功 其他flash失败
   if(ret)
   {
     	DispStr_CE(0,0,"DB_erase_db_sys",DISP_CENTER|DISP_CLRSCR);
 	delay_and_wait_key(0,EXIT_KEY_F2,0);
      goto err_end;
   } 
   ret = DB_format_db(0,BLOCKNUM,sizeof(DataInfo));   //0 成功 其他失败 
   if(ret)
   {
   DispStr_CE(0,0,"DB_format_db",DISP_CENTER|DISP_CLRSCR);
 	delay_and_wait_key(0,EXIT_KEY_F2,0);
    goto err_end;
   } 
   
   ret = DB_init_sys_param(0);         //1 数据表数 
   if(ret!= 1)
   {
   DispStr_CE(0,0,"DB_init_sys_param",DISP_CENTER|DISP_CLRSCR);
 	delay_and_wait_key(0,EXIT_KEY_F2,0);
            goto err_end;
   }
   
   ret = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));     //0格式正确 其他错误 
   if(ret)
   {
    /*DispStr_CE(0,0,"DB_check_format",DISP_CENTER|DISP_CLRSCR);
    char kk[5];
    memset(kk,0,5);
    sprintf(kk,"ret=%d",ret);
        DispStr_CE(0,2,kk,DISP_CENTER);
 	delay_and_wait_key(0,EXIT_KEY_F2,0);  */ 
     goto err_end;
   }
   
    	/*DispStr_CE(0,0,"数据库初始化成功！",DISP_CENTER|DISP_CLRSCR);
    	WarningBeep(1); 
    	delay_and_wait_key(0,EXIT_KEY_F2,0); 
    	*/
    	return 0;
    	
   err_end:
 	/*DispStr_CE(0,0,"数据库初始化失败！",DISP_CENTER|DISP_CLRSCR);
 	    	WarningBeep(2);
 	delay_and_wait_key(0,EXIT_KEY_F2,0);
 	*/ 
 	return -1; 
}



short AddOneRecord(char* name,char* time,char* code,char* record){
	datainfo.id = DB_count_records(0)+1;
	strcpy(datainfo.username,name);
	strcpy(datainfo.antifakecode,code);
	strcpy(datainfo.querytime,time);

	sprintf(record,"%d",datainfo.id);

	unsigned long ret;
	ret = DB_add_record(0,(void*)&datainfo);
	if(ret == 0){
		DispStr_CE(0,0,"插入记录失败",DISP_CENTER|DISP_CLRSCR);
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return -1;
	}else{
		// DispStr_CE(0,0,"插入记录成功",DISP_CENTER);
		return 0;
	}
}

short  CheckDB()   //检查数据库是否完好 0 表示完好，-1 表示失败 
{
  int pp;
   if(DB_init_sys_param(0)==1)
   {
        //DispStr_CE(0,10,"init_sys_param ",DISP_CENTER|DISP_CLRSCR);
       //delay_and_wait_key(0,EXIT_KEY_F2,0);
      pp = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));
       if(pp ==0)
       {
           // DispStr_CE(0,10,"checkformat ",DISP_CENTER|DISP_CLRSCR);
          // delay_and_wait_key(0,EXIT_KEY_F2,0);
         return 0;
       }
      else
      {
             return -1;
      }
   }
   else
   {
     return -1;
   }
   
} 

short EncodeSendData(char* name ,char* passwd,char* senddata){  //分批封装记录 
	short ret ;
	char flag; 
	DataInfo* pdi;
	DataInfo di;
	int recordnum; //数据表中记录条数 
	int temp =0;
	int encodenum=0; //封装记录条数 
	//检查数据库 
	ret = CheckDB();
	if(ret == 0){  //数据库完好 
		recordnum =0;
		recordnum = DB_count_records(0);//表中已经使用的记录数
		char tempsenddata[30];
		memset(tempsenddata,0,30);

		sprintf(tempsenddata,"*1;%s;%s;",name,passwd);
		strcat(senddata,tempsenddata);

		int norecord=0;

		int i=0;
		memset(Menu,'\0',150*PAGENUM+2);

		while(temp<recordnum){
			pdi=DB_jump_to_record(0,temp,&flag);

			if((flag ==1)||(flag == 2)){//空记录或被删除的记录 
				temp++;
				norecord++;
			}else{
				memcpy(&di,pdi,sizeof(DataInfo));

				sprintf(&Menu[i*28],"%s%s","巡 检 人：",di.username); 
				sprintf(&Menu[(i+1)*28],"%s%s","防 伪 码：",di.antifakecode);
				sprintf(&Menu[(i+2)*28],"%s%s","巡检时间：",di.querytime);
				sprintf(&Menu[(i+3)*28],"%s%d","记录条数：",di.id);
				sprintf(&Menu[(i+4)*28],"%s","<------------------------->");

				i=i+5;

				char tempid[10];
				memset(tempid,0,10);
				sprintf(tempid,"%d",di.id);

				strcat(senddata,tempid);
				strcat(senddata,",");

				strcat(senddata,di.antifakecode);
				strcat(senddata,",");

				strcat(senddata,di.username);
				strcat(senddata,",");

				strcat(senddata,di.querytime);
				strcat(senddata,"/");
				temp++;

				flagarr[encodenum]= di.id;

				encodenum++;
				if(encodenum == PAGENUM){//达到最大发送条数 
					break;//跳出循环 
				}
			}
		}

		senddata[strlen(senddata)-1]='#';
		senddata[strlen(senddata)]='\n';

		if(norecord == recordnum){ //没有记录 
			return -1; 
		}
	}
	return 0; 
}

/*
short EncodeSendData(char* name ,char* passwd,char* senddata)
{
   short ret ;
   char flag; 
   DataInfo* pdi;
   DataInfo di;
   int recordnum;
   int temp =0;
   int encodenum=0; 
   if(DB_init_sys_param(0)==1)
   {
      //DispStr_CE(0,10,"init_sys_param ",DISP_CENTER|DISP_CLRSCR);
     //delay_and_wait_key(0,EXIT_KEY_F2,0);
   }
   int pp = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));
   if(pp ==0)
   {
        // DispStr_CE(0,10,"checkformat ",DISP_CENTER|DISP_CLRSCR);
    // delay_and_wait_key(0,EXIT_KEY_F2,0);
   }
   else
   {
         char temppp[9];
          memset(temppp,0,9);
          sprintf(temppp,"pp=%d",pp);
          DispStr_CE(0,10,temppp,DISP_CENTER|DISP_CLRSCR);
         delay_and_wait_key(0,EXIT_KEY_F2,0);
   }
   
   recordnum = DB_count_records(0);//表中已经使用的记录数
   char tempsenddata[30];
   memset(tempsenddata,0,30);
  
   sprintf(tempsenddata,"*1;%s;%s;",name,passwd);
   strcat(senddata,tempsenddata);
   int norecord=0;
   while(temp<recordnum)
   {
       pdi=DB_jump_to_record(0,temp,&flag);
       if((flag ==1)||(flag == 2))
       {
          temp++;
          norecord++;
       }
       else
       {
          
            memcpy(&di,pdi,sizeof(DataInfo));
            char tempid[10];
            memset(tempid,0,10);
            sprintf(tempid,"%d",di.id);
            strcat(senddata,tempid);
            strcat(senddata,",");
         
            strcat(senddata,di.antifakecode);
            strcat(senddata,",");
            strcat(senddata,di.username);
            strcat(senddata,",");
            strcat(senddata,di.querytime);
            strcat(senddata,"/");              
           temp++;
       }
        
   }
   senddata[strlen(senddata)-1]='#';
   senddata[strlen(senddata)]='\n';
   if(norecord == recordnum) //没有记录 
   {
     return 1; 
   }
   DispStr_CE(0,0,"本次提交数据如下：",DISP_CENTER|DISP_CLRSCR);
    DispStr_CE(0,2,"按任意键提交",DISP_CENTER);
   EXT_Display_Multi_Lines(senddata,4,34);
  
   delay_and_wait_key(0,EXIT_KEY_ALL,0);
            
            return 0;
 
}
 */
short HandleRecvData(char* recvdata)
{
       char wronginfo[3000];
       char right[5];
       char wrong[5];
       memset(right,0,5);
       memset(wrong,0,5);       
       strncpy(right,recvdata,2);
       strncpy(wrong,recvdata,2);
       
       if(strcmp(right,"*0")==0)
       {
          //重新创建数据库 
          //DispStr_CE(0,10,"完全正确",DISP_CENTER|DISP_CLRSCR);
        // delay_and_wait_key(0,EXIT_KEY_F2,0);
          
          //更新数据
         DataInfo di;
         DataInfo* pdi;
         int recordnum; 
         recordnum = DB_count_records(0);
         int index =0;
         int flag; 
       while(index<recordnum)
       {
          
         pdi=DB_jump_to_record(0,index,&flag);
         if((flag ==1)||(flag == 2))    //空记录或已删除记录 
         {
             index++;
         }
         else
         {
            
            memcpy(&di,pdi,sizeof(DataInfo));
            int j;
            for(j=0;j<PAGENUM;j++)
            {   
                if(di.id == flagarr[j])
                {
                    DB_delete_record(0,index);//删除该记录 
                  break; 
                }
            } 
            index ++;
         }
       } 
         /* int index=0; 
          int ret; 
     while(1)
     { 
        ret = CreateDatabase();
        if(ret!= 0)
        {
         index++;
         if(index ==4)
         { 
            break; 
         } 
        }
        else
        {
          break; 
        } 
     } 
      */
          return 0;//完全正确 
       }
       else  if(strcmp(wrong,"*1")==0)   //用户名错误 无改用户 
       {
             // DispStr_CE(0,0,"用户名不存在",DISP_CENTER|DISP_CLRSCR); 
             // delay_and_wait_key(0,EXIT_KEY_F2,0);
              return 1; 
       }
       else if(strcmp(wrong,"*2") ==0)  //密码错误 
       {
            //DispStr_CE(0,0,"用户名不存在",DISP_CENTER|DISP_CLRSCR); 
             // delay_and_wait_key(0,EXIT_KEY_F2,0);
              return 2;
       } 
       else   //上传记录有错误 
       {
           //更新数据库            
            /* DispStr_CE(0,0,"更新数据库",DISP_CENTER|DISP_CLRSCR);
             delay_and_wait_key(0,EXIT_KEY_F2,0);
             */
             
            UpdateDatabase(recvdata,wronginfo) ;
            
             Disp_Clear();
             
              //DispStr_CE(0,0,"错误信息",DISP_CENTER|DISP_CLRSCR);
             
              WarningBeep(2); 
              
                Display(1);
                WarningBeep(0); 
               DispStr_CE(0,4,"恭喜！本次数据提交完毕",DISP_CENTER|DISP_CLRSCR);
                EXT_ClearLine(36,0);
                 Disp_Goto_XY(0,36);
                DispStr_CE(0,36,"【F1退出】",DISP_CURRENT);
               DispStr_CE(0,36,"【F3上传下一批】",DISP_RIGHT);
              
              long key_value; 
              key_value = delay_and_wait_key(0,EXIT_KEY_F1|EXIT_KEY_F3,0);
              
              switch(key_value)
              {
                  case EXIT_KEY_F1:   //退出 
                  {
                     return 3;
                     break;
                  }
                 case EXIT_KEY_F3:
                 {
                     return 4;   //继续 
                    break;
                 }
                 default:
                 {
                    WarningBeep(1);
                    break;
                 }
              }
       
           
       }
} 

void UpdateDatabase(char* recvdata,char* wronginfo){
	int recordnum;
	memset(wronginfo,0,3000); 
	int index =0;
	DataInfo di;
	DataInfo* pdi;
	char flag; 
	char temprecvdata[200];
	char temprecvdata1[200];

	recordnum = DB_count_records(0);//表中已经使用的记录数

	/* DispStr_CE(0,8,"开始更新数据库",DISP_CENTER|DISP_CLRSCR);
	DispStr_CE(0,10,recvdata,DISP_CENTER);
	delay_and_wait_key(0,EXIT_KEY_F2,0);
	*/
	memset(temprecvdata,0,200);  
	strncpy(temprecvdata,recvdata,strlen(recvdata));

	int i=0;
	memset(Menu,'\0',150*28+2);

	while(index<recordnum) {
	strncpy(temprecvdata,recvdata,strlen(recvdata));
	pdi=DB_jump_to_record(0,index,&flag);
	if((flag ==1)||(flag == 2)){//空记录或已删除记录 
		index++;
	}else{
		memcpy(&di,pdi,sizeof(DataInfo));


		// DispStr_CE(0,0,"Update recvdata",DISP_CENTER|DISP_CLRSCR);
		// DispStr_CE(0,2,temprecvdata,DISP_CENTER);

		//delay_and_wait_key(0,EXIT_KEY_F2,0);

		int k=0;

		/* DispStr_CE(0,4,temprecvdata,DISP_CENTER|DISP_CLRSCR);
		DispStr_CE(0,2,"TEMPRECVDATA",DISP_CENTER);
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		*/
		char* token = NULL;
		token= strtok(temprecvdata,";");
		token = strtok(NULL,";");

		/* DispStr_CE(0,2,"token before while",DISP_CENTER|DISP_CLRSCR);
		DispStr_CE(0,4,token,DISP_CENTER);
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		*/

		while(strcmp(token,"#")){
			int len = strlen(token);
			int j=0;
			char idarr[5];
			char errcodearr[5];
			memset(idarr,0,5);
			memset(errcodearr,0,5);
			int flag=0;
			int index=0;
			for(;j<len;j++)	{
				if((token[j]!=',')&&(flag ==0)){
					index++;
					idarr[j] = token[j];
				}else  if((token[j] == ',')&&(flag ==0)){
					flag =1;
					index++;
					continue;
				}else   if((token[j] != ',')&&(flag ==1)){
					errcodearr[j-index]=token[j];
				}
			}
			/* DispStr_CE(0,0,"id+ err",DISP_CENTER|DISP_CLRSCR);
			DispStr_CE(0,2,idarr,DISP_CENTER);
			DispStr_CE(0,4,errcodearr,DISP_CENTER);
			delay_and_wait_key(0,EXIT_KEY_F2,0);
			*/


			char myid[5];
			memset(myid,0,5);
			sprintf(myid,"%d",di.id); 

			/*DispStr_CE(0,0,myid,DISP_CENTER|DISP_CLRSCR);
			DispStr_CE(0,2,"toke",DISP_CENTER);
			DispStr_CE(0,4,token,DISP_CENTER);
			delay_and_wait_key(0,EXIT_KEY_F2,0);
			*/

                  if(strcmp(myid,idarr)==0)  //失败记录  记录失败原因  
                  {
                     
                      char temp_wronginfo[100];
                      memset(temp_wronginfo,0,100);
                      
                       sprintf(&Menu[i*28],"%s%s","巡 检 人：",di.username); 
                       sprintf(&Menu[(i+1)*28],"%s%s","防 伪 码：",di.antifakecode);
                       sprintf(&Menu[(i+2)*28],"%s%s","巡检时间：",di.querytime);

                      if(strcmp(errcodearr,"1")==0)    //卡不存在 
                      {
                             sprintf(&Menu[(i+3)*28],"%s%d%s","错误原因：",di.id,"后台无该卡信息");
                                 sprintf(&Menu[(i+4)*28],"%s","<------------------------->");
                         // sprintf(temp_wronginfo,"%d%s%s%s%s", di.id,di.username,di.antifakecode,di.querytime,"后台无该卡信息");
                      }
                      else if(strcmp(errcodearr,"2")==0)//卡为被激活
                      {
                             sprintf(&Menu[(i+3)*28],"%s%d%s","错误原因：",di.id,"该卡未被激活");
                                 sprintf(&Menu[(i+4)*28],"%s","<------------------------->");
                            // sprintf(temp_wronginfo,"%d%s%s%s%s", di.id,di.username,di.antifakecode,di.querytime,"该卡未被激活");
                      }
                      else  if(strcmp(errcodearr,"3")==0)  //其他原因 
                      {
                                  sprintf(&Menu[(i+3)*28],"%s%d%s","错误原因：",di.id,"其他原因");
                                 sprintf(&Menu[(i+4)*28],"%s","<------------------------->");
                              // sprintf(temp_wronginfo,"%d%s%s%s%s", di.id,di.username,di.antifakecode,di.querytime,"其他原因");
                      } 
                       i=i+5; 
                       
                      strcat(wronginfo,temp_wronginfo);
                     
                            /*DispStr_CE(0,4,"删除错误记录",DISP_CENTER|DISP_CLRSCR);
                      delay_and_wait_key(0,EXIT_KEY_F2,0);
                      */
                      DB_delete_record(0,di.id-1);//删除该记录 
                      
                      break; 
                    
                  }
                  else   //继续下一个处理 
                  {
                       token = strtok(NULL,";") ;
                       continue;
                  }
                 
            }
            if(strcmp(token,"#")==0)  //recvdata中没有该记录 则记录上传成功 
            {
                /* DispStr_CE(0,4,"TOKEN == #",DISP_CENTER|DISP_CLRSCR);
                 delay_and_wait_key(0,EXIT_KEY_F2,0);
                 */
                 
                 
                      int kk =0;
                      for(;kk<PAGENUM;kk++)
                      {
                       
                        if(di.id == flagarr[kk])
                        {       /*DispStr_CE(0,4,"删除其他记录",DISP_CENTER|DISP_CLRSCR);
                          delay_and_wait_key(0,EXIT_KEY_F2,0);
                          */
                          DB_delete_record(0,index);//删除该记录 
                         }
                      }
            }
          
             
              
            
            
             /*DispStr_CE(0,4,"out while",DISP_CENTER|DISP_CLRSCR);
              delay_and_wait_key(0,EXIT_KEY_F2,0);
              */
              memset(temprecvdata,0,200);
           
          index++;
                    
       }
       
   } 
}
