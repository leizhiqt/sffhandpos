#ifndef   CARD

#define   CARD

#include "Common.h"

//ultralight 
unsigned char	ULATQ[3];
unsigned char	ULSNO[4];

unsigned char username[USERNAME_LEN];//Ñ²¼ìÓÃ»§Ãû
unsigned char password[USERNAME_LEN];//Ñ²¼ìÃÜÂë

char anticode[ANTIFAKECODE_LEN];//·ÀÎ±Âë 

unsigned char sname[USERNAME_LEN];//µÇÂ¼ÓÃ»§Ãû
unsigned char spass[USERNAME_LEN];////µÇÂ¼ÃÜÂë

short ultralight_init();
short ultralight_find_card();
short ultralight_read_card(unsigned char block_add,unsigned char* databuf);
void ultralight_close();
short ultralight_local_read_card(char* ac);

int ReadNameAndPassword(unsigned char* name,unsigned char* password);

short LoginByCard();

short LoginToSubmit();

#endif
