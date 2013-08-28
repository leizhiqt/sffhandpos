#ifndef		DATABASE
#define		DATABASE

#include "common.h"

//数据库表占块数
#define BLOCKNUM 10

//行长度
#define LINE_LEN 30

//每次上传条数
//GPRS一次最大能传输1024字节
// 1024 / sizeof(DataInfo) + 10
// 10为记录与记录这类的分隔符
#define PAGE_SIZE 20

//定义数据表结构 
typedef struct {
	unsigned int id;//id号方便查找
	char username[USERNAME_LEN];//用户名
	char antifakecode[ANTIFAKECODE_LEN];//防伪码
	char querytime[QUERYTIME_LEN];//巡检时间
}DataInfo;

extern char Menu[150*LINE_LEN+2];

int FindDatabase();

short CreateDatabase();

short InitDatabase(); 

short AddOneRecord(unsigned char* name,char* time,char* code,char* record);

short EncodeSendData(unsigned char* name ,unsigned char* passwd,unsigned char* senddata);

short HandleRecvData(unsigned char* recvdata);

void UpdateDatabase(unsigned char* recvdata);

int dbClean();

#endif
