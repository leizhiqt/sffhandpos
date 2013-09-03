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

char Menu[150*LINE_LEN+2];

static unsigned int pids[PAGE_SIZE];
static DataInfo datainfo;
static int PAGE_OFFSET=0;

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
		DispStr_CE(0,36,"【F2退出】",DISP_POSITION);		
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return -1;
	} 

	RET = DB_format_db(0,BLOCKNUM,sizeof(DataInfo));   // 0 成功 其他失败 
	if(RET){
		DispStr_CE(0,0,"DB_format_db",DISP_CENTER|DISP_CLRSCR);
		DispStr_CE(0,36,"【F2退出】",DISP_POSITION);				
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return -1;
	}

	RET = DB_init_sys_param(0);         // 1     数据表数 
	if(RET!= 1){
		DispStr_CE(0,0,"DB_init_sys_param",DISP_CENTER|DISP_CLRSCR);
		DispStr_CE(0,36,"【F2退出】",DISP_POSITION);				
		delay_and_wait_key(0,EXIT_KEY_F2,0);
		return -1;
	}

	RET = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));// 0格式正确 其他错误 
	if(RET){
		return -1;
	}

	return 0;
}

short AddOneRecord(unsigned char* name,char* time,char* code,char* record){
	unsigned long ret = 0;
	
	datainfo.id = DB_count_records(0)+1;
	strcpy(datainfo.username,(char *)name);
	strcpy(datainfo.antifakecode,code);
	strcpy(datainfo.querytime,time);

	sprintf(record,"%d",datainfo.id);
	
	ret = DB_add_record(0,(void*)&datainfo);
	if(ret == 0){
		DispStr_CE(0,0,"插入记录失败",DISP_CENTER|DISP_CLRSCR);
		DispStr_CE(0,36,"【F2退出】",DISP_POSITION);				
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
	if(RET!=1){
		return -1;
	}

	RET = DB_check_format(0,BLOCKNUM,sizeof(DataInfo));
	if(RET ==0){
		return 0;
	}

	return RET;
}

DataInfo* dbRetrieve(char id[5]){
	char flag=0;
	char myid[5]={0};
	DataInfo di;
	DataInfo* pdi=NULL;	
	int i =0;	
	 
	int dbCount = DB_count_records(0); 
	for(i=0; i<dbCount;i++){
		pdi=DB_jump_to_record(0,i,&flag);
		if(flag==0){//非空记录 && 存在记录
			memcpy(&di,pdi,sizeof(DataInfo));
			memset(myid,0,5);
			sprintf(myid,"%d",di.id); 
			if(strcmp(myid,id)==0){
				return pdi;
			}
		}
	}
	return NULL;
}

int dbDelete(unsigned int id){
	DataInfo* pdi=NULL;
	char flag = 0;
	int i =0;	
	int dbCount = DB_count_records(0);
	
	for(i=0; i<dbCount;i++){
		pdi=DB_jump_to_record(0,i,(char *)&flag);

		if(flag==0){//非空记录 && 存在记录 
			if(pdi->id == id){
				DB_delete_record(0,i);//删除该记录
				return 0;
			}
		}
	}
	return -1;
}

int dbClean(){
	DataInfo* pdi=NULL;
	DataInfo di;
	char flag = 0;
	int i = 0;	
	int j = 0;
	int dbCount = DB_count_records(0);
	
	for(i=0; i<dbCount;i++){
		pdi=DB_jump_to_record(0,i,(char *)&flag);

		if(flag==0){//非空记录 && 存在记录
			memcpy(&di,pdi,sizeof(DataInfo));
			for(j=0;j<PAGE_OFFSET;j++){
				if(di.id==pids[j]) DB_delete_record(0,i);//删除该记录
			}
		}
	}
	//memset(pids,'\0',sizeof(unsigned int)*PAGE_SIZE);
	return 0;
}

short EncodeSendData(unsigned char* name ,unsigned char* passwd,unsigned char* senddata){  //记录封装 协议数据格式 
	int norecord=0;
	int i=0;
	int RET=-1 ;
	int dbCount=0; //数据表中记录条数
	int rloop =0;
	int encodenum=0; //封装记录条数 
	
	DataInfo di;
	DataInfo* pdi = NULL;
	char flag = 0;	
	char tempsenddata[30];
	
	//检查数据库 
	RET = CheckDB();
	if(RET != 0){
		return -1;
	}
	
	//数据库完好 
	dbCount = DB_count_records(0);//表中已经使用的记录数

	if(dbCount<1){ //没有记录
		return -1;
	}

	memset(tempsenddata,0,30);
	sprintf(tempsenddata,"*1;%s;%s;",name,passwd);
	strcat((char *)senddata,tempsenddata);

	memset(Menu,'\0',sizeof(Menu));
	while(rloop<dbCount){
		pdi=DB_jump_to_record(0,rloop,&flag);

		if(flag==0){ 
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

			pids[encodenum]= di.id;

			encodenum++;
			if(encodenum == PAGE_SIZE){//达到最大发送条数 
				break;//跳出循环
			}
		}else{//空记录或被删除的记录
			norecord++;
		}//end if

		rloop++;
	}

	PAGE_OFFSET=encodenum;

	if(norecord==dbCount){ //没有记录
		return -1;
	}

	senddata[strlen((char *)senddata)-1]='#';
	senddata[strlen((char *)senddata)]='\n';

	return encodenum; 
}

short HandleRecvData(unsigned char* recvdata){
	char rets[5]={0};
	
	strncpy(rets,(char *)recvdata,2);

	if(strcmp(rets,"*0")==0){
		return 0;//完全正确
	}

	if(strcmp(rets,"*1")==0){	//用户名错误 无改用户 
		return 1; 
	}

	if(strcmp(rets,"*2") ==0) {	//密码错误 
		return 2;
	}

	if(strcmp(rets,"*3") ==0){	//上传记录有错误
		return 3;
	}

	return -1;
}

void UpdateDatabase(unsigned char* recvdata){
 	int i = 0;
	char temprecvdata[1024];
	char* token = NULL;

	memset(temprecvdata, '\0', sizeof(temprecvdata));
	strncpy(temprecvdata,(char *)recvdata,strlen((char *)recvdata));

	memset(Menu,'\0',sizeof(Menu));
	token= strtok(temprecvdata,";");//1 spit
	token = strtok(NULL,";");//2 spit

	while(token){
		char idarr[5]={0};
		char errcodearr[5]={0};
		char * ip=idarr;
		char * ep=errcodearr;
		char *p = token;

		int head=1;
		while(*p!=0 && *p!='#' && *p!='*'){
			if(head){
				if(*p!=',') {
					//printf("->%c\n",*p);
					*ip=*p;
					ip++;
				}else{
					head=0;
				}
			}else{
				//printf("<-%c\n",*p);
				*ep=*p;
				ep++;
			}
			p++;
		}

		if(head==0){
			DataInfo* pdi=dbRetrieve(idarr);
			DataInfo di;

			//失败记录  记录失败原因
			if(pdi!=NULL){
				memcpy(&di,pdi,sizeof(DataInfo));

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
				}else{  //其他原因 
					sprintf(&Menu[(i+3)*28],"%s%d%s","错误原因：",di.id,"其他原因");
					sprintf(&Menu[(i+4)*28],"%s","<------------------------->");
					// sprintf(temp_wronginfo,"%d%s%s%s%s", di.id,di.username,di.antifakecode,di.querytime,"其他原因");
				} 
				i=i+5;
			}
		}
		//继续下一个处理
		token = strtok(NULL,";");
	}
	//清理数据库
	dbClean();
}
