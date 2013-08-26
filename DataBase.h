#ifndef		DATABASE
#define		DATABASE

#include "common.h"

//数据库表占块数
#define BLOCKNUM 10

//每次上传条数
#define PAGE_SIZE 30

//定义数据表结构 
typedef struct {
	unsigned int id;//id号方便查找
	char username[USERNAME_LEN];//用户名
	char antifakecode[ANTIFAKECODE_LEN];//防伪码
	char querytime[QUERYTIME_LEN];//巡检时间
}DataInfo;

DataInfo datainfo;

char Menu[150*PAGE_SIZE+2];

int FindDatabase();

short CreateDatabase();

short InitDatabase(); 

short AddOneRecord(unsigned char* name,char* time,char* code,char* record);

short EncodeSendData(unsigned char* name ,unsigned char* passwd,unsigned char* senddata);

short HandleRecvData(unsigned char* recvdata);

void UpdateDatabase(unsigned char* recvdata);

int dbClean();

#endif
