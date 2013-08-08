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
#include "string.h"
#include "api.h"

extern  short Display(short flag);

unsigned int flagarr[PAGENUM];

//======================================================================
//函数名：FindDatabase 
//功能  ：查找数据库 
//参数  ：无 
//返回值：short -1 表示起始地址失败，其他表示数据表的个数 
//======================================================================
int FindDatabase(){
	int RET=-1;
	RET = DB_init_sys_param(0);

	if(RET != 1){
		return -1;
	}

	RET = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));
	if(RET ==0){
		return 0;
	}

	return -1;
}
//======================================================================
//函数名：CreateDatabase 
//功能  ：创建数据库 
//参数  ：无 
//返回值：short 
//======================================================================
short CreateDatabase(){
	int RET=-1;

	RET = DB_erase_db_sys();   //0 成功 其他flash失败
	if(RET){
		DispStr_CE(0,0,"DB_erase_db_sys",DISP_CENTER|DISP_CLRSCR);
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return -1;
	} 

	RET = DB_format_db(0,BLOCKNUM,sizeof(DataInfo));   //0 成功 其他失败 
	if(RET){
		DispStr_CE(0,0,"DB_format_db",DISP_CENTER|DISP_CLRSCR);
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return -1;
	}

	RET = DB_init_sys_param(0);         //1 数据表数 
	if(RET!= 1){
		DispStr_CE(0,0,"DB_init_sys_param",DISP_CENTER|DISP_CLRSCR);
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return -1;
	}

	RET = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));//0格式正确 其他错误 
	if(RET){
		return -1;
	}

	return 0;
}

short AddOneRecord(unsigned char* name,char* time,char* code,char* record){
	datainfo.id = DB_count_records(0)+1;
	strcpy(datainfo.username,(char *)name);
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

short  CheckDB(){//检查数据库是否完好 0 表示完好，-1 表示失败 
	int RET = -1;

	RET=DB_init_sys_param(0);
	if(RET!=1)
		return -1;

	RET = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));
	if(RET ==0){
		return 0;
	}

	return -1;
}

short EncodeSendData(unsigned char* name ,unsigned char* passwd,unsigned char* senddata){  //分批封装记录 
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
		strcat((char *)senddata,tempsenddata);

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

				strcat((char *)senddata,tempid);
				strcat((char *)senddata,",");

				filter(di.antifakecode,';');
				filter(di.antifakecode,',');
				strcat((char *)senddata,di.antifakecode);
				strcat((char *)senddata,",");

				strcat((char *)senddata,di.username);
				strcat((char *)senddata,",");

				strcat((char *)senddata,di.querytime);
				strcat((char *)senddata,"/");
				temp++;

				flagarr[encodenum]= di.id;

				encodenum++;
				if(encodenum == PAGENUM){//达到最大发送条数 
					break;//跳出循环 
				}
			}
		}

		senddata[strlen((char *)senddata)-1]='#';
		senddata[strlen((char *)senddata)]='\n';

		if(norecord == recordnum){ //没有记录 
			return -1; 
		}
	}
	return 0; 
}

short HandleRecvData(unsigned char* recvdata){
	char wronginfo[3000];
	char right[5];
	char wrong[5];
	memset(right,0,5);
	memset(wrong,0,5);
	strncpy(right,(char *)recvdata,2);
	strncpy(wrong,(char *)recvdata,2);

	if(strcmp(right,"*0")==0){
		//重新创建数据库 
		//更新数据
		DataInfo di;
		DataInfo* pdi;
		int recordnum; 
		recordnum = DB_count_records(0);
		int rloop =0;
		char flag; 
		while(rloop<recordnum){
			pdi=DB_jump_to_record(0,rloop,(char *)&flag);
			if((flag ==1)||(flag == 2)){    //空记录或已删除记录 
				rloop++;
			}else{
				memcpy(&di,pdi,sizeof(DataInfo));
				int j;
				for(j=0;j<PAGENUM;j++){   
					if(di.id == flagarr[j]){
						DB_delete_record(0,rloop);//删除该记录 
						break; 
					}
				} 
				rloop ++;
			}
		} 
		return 0;//完全正确 
	}else  if(strcmp(wrong,"*1")==0){   //用户名错误 无改用户 
		return 1; 
	}else if(strcmp(wrong,"*2") ==0) {  //密码错误 
		return 2;
	}else{	//上传记录有错误 
		//更新数据库
		UpdateDatabase(recvdata,wronginfo) ;
		Disp_Clear();
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

		switch(key_value){
			case EXIT_KEY_F1:{	//退出 
				return 3;
				break;
			}case EXIT_KEY_F3:{
				return 4;	//继续 
				break;
			}default:{
				WarningBeep(1);
				break;
			}
		}
	}
	return 0;
}

void UpdateDatabase(unsigned char* recvdata,char* wronginfo){
	int recordnum;
	memset(wronginfo,0,3000); 
	int rloop =0;
	DataInfo di;
	DataInfo* pdi;
	char flag; 
	char temprecvdata[200];
	recordnum = DB_count_records(0);//表中已经使用的记录数

	memset(temprecvdata,0,200);  
	strncpy(temprecvdata,(char *)recvdata,strlen((char *)recvdata));

	int i=0;
	memset(Menu,'\0',150*28+2);

	while(rloop<recordnum) {
		strncpy(temprecvdata,(char *)recvdata,strlen((char *)recvdata));
		pdi=DB_jump_to_record(0,rloop,&flag);
		if((flag ==1)||(flag == 2)){//空记录或已删除记录 
			rloop++;
		}else{
			memcpy(&di,pdi,sizeof(DataInfo));
			char* token = NULL;
			token= strtok(temprecvdata,";");
			token = strtok(NULL,";");

			while(strcmp(token,"#")){
				int len = strlen(token);
				int j=0;
				char idarr[5];
				char errcodearr[5];
				memset(idarr,0,5);
				memset(errcodearr,0,5);
				int sign=0;
				rloop=0;
				for(;j<len;j++)	{
					if((token[j]!=',')&&(sign ==0)){
						rloop++;
						idarr[j] = token[j];
					}else  if((token[j] == ',')&&(sign ==0)){
						sign =1;
						rloop++;
						continue;
					}else   if((token[j] != ',')&&(sign ==1)){
						errcodearr[j-rloop]=token[j];
					}
				}

				char myid[5];
				memset(myid,0,5);
				sprintf(myid,"%d",di.id); 

				if(strcmp(myid,idarr)==0){  //失败记录  记录失败原因  
					char temp_wronginfo[100];
					memset(temp_wronginfo,0,100);

					sprintf(&Menu[i*28],"%s%s","巡 检 人：",di.username); 
					sprintf(&Menu[(i+1)*28],"%s%s","防 伪 码：",di.antifakecode);
					sprintf(&Menu[(i+2)*28],"%s%s","巡检时间：",di.querytime);

					if(strcmp(errcodearr,"1")==0){    //卡不存在 
						sprintf(&Menu[(i+3)*28],"%s%d%s","错误原因：",di.id,"后台无该卡信息");
						sprintf(&Menu[(i+4)*28],"%s","<------------------------->");
						// sprintf(temp_wronginfo,"%d%s%s%s%s", di.id,di.username,di.antifakecode,di.querytime,"后台无该卡信息");
					}else if(strcmp(errcodearr,"2")==0){//卡为被激活
						sprintf(&Menu[(i+3)*28],"%s%d%s","错误原因：",di.id,"该卡未被激活");
						sprintf(&Menu[(i+4)*28],"%s","<------------------------->");
						// sprintf(temp_wronginfo,"%d%s%s%s%s", di.id,di.username,di.antifakecode,di.querytime,"该卡未被激活");
					}else  if(strcmp(errcodearr,"3")==0){  //其他原因 
						sprintf(&Menu[(i+3)*28],"%s%d%s","错误原因：",di.id,"其他原因");
						sprintf(&Menu[(i+4)*28],"%s","<------------------------->");
						// sprintf(temp_wronginfo,"%d%s%s%s%s", di.id,di.username,di.antifakecode,di.querytime,"其他原因");
					} 
					i=i+5; 

					strcat(wronginfo,temp_wronginfo);
					DB_delete_record(0,di.id-1);//删除该记录 
					break; 
				}else {  //继续下一个处理 
					token = strtok(NULL,";") ;
					continue;
				}
			}
			if(strcmp(token,"#")==0){  //recvdata中没有该记录 则记录上传成功 
				int kk =0;
				for(;kk<PAGENUM;kk++){
					if(di.id == flagarr[kk]){ 
						DB_delete_record(0,rloop);//删除该记录 
					}
				}
			}
			memset(temprecvdata,0,200);
			rloop++;
		}
	}
}
