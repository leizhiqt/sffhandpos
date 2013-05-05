/****************************************************************************
*
*文件(File):         Card.h
*
*修改(Modify):       2012-7-13 14:15:45
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
| V1.00  2012-7-13 14:15:45           sa 
----------------------------------------------------------------------------
****************************************************************************/

#include "Common.h"

#ifndef   CARD

#define   CARD

//ultralight 
unsigned char	ATQ[3];
unsigned char	sno[4];

char username[USERNAME_LEN];//用户名 
char password[USERNAME_LEN];//密码 

unsigned char anticode[ANTIFAKECODE_LEN];//防伪码 

char submitname[USERNAME_LEN];
char submitpassword[USERNAME_LEN];

short ultralight_init();
short ultralight_find_card();
short ultralight_read_card(unsigned char block_add,unsigned char* databuf);
void ultralight_close();
short ultralight_local_read_card(unsigned char* ac);

int ReadNameAndPassword(char* name,char* password);
short LoginByCard();
short LoginToSubmit();

#endif
