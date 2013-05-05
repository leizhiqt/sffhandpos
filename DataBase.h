/****************************************************************************
*
*文件(File):         DataBase.h
*
*修改(Modify):       2012-7-13 14:23:46
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
| V1.00  2012-7-13 14:23:46           sa 
----------------------------------------------------------------------------
****************************************************************************/

#include "api.h"
#include "string.h"
#include "common.h"

#ifndef		DATABASE
#define		DATABASE

#define BLOCKNUM 10 //数据库表占块数

#define PAGENUM 30

//定义数据表结构 
typedef struct { 
	unsigned int id;//id号方便查找 
	char username[USERNAME_LEN];//用户名
	char antifakecode[ANTIFAKECODE_LEN];//防伪码
	char querytime[QUERYTIME_LEN];//巡检时间
}DataInfo;

DataInfo datainfo;

char Menu[150*28+2];

short FindDatabase();

short CreateDatabase();

short InitDatabase(); 

short AddOneRecord(char* name,char* time,char* code,char* record);

short EncodeSendData(char* name ,char* passwd,char* senddata);

#endif