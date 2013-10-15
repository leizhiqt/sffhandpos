#ifndef		DATABASE
#define		DATABASE

#include "common.h"

#define USERNAME_LEN 17 //max 8 chinese
#define ANTIFAKECODE_LEN 17
#define QUERYTIME_LEN 21


//数据库表占块数
#define BLOCKNUM 10

//行长度
#define LINE_LEN 30

//每次上传条数
//GPRS一次最大能传输1024字节
// (1024 - 20) / (sizeof(DataInfo) + 10)
//	20 为前面的数据
// 10为记录与记录这类的分隔符
#define PAGE_SIZE 14

//定义数据表结构 
typedef struct {
	//id号方便查找
	unsigned int id;
	
	//用户名
	char username[USERNAME_LEN];

	//防伪码
	char antifakecode[ANTIFAKECODE_LEN];

	//巡检时间
	char querytime[QUERYTIME_LEN];
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
