#ifndef _SIM900_H
#define _SIM900_H

#define TEN_SECOND			640	 
#define NINE_SECOND			576
#define EIGHT_SECOND		512
#define SEVEN_SECOND		448
#define SIX_SECOND			384
#define FIVE_SECOND			320
#define FOUR_SECOND			256
#define THREE_SECOND		192
#define TWO_SECOND			128
#define ONE_SECOND			64
#define HALF_SECOND			32
#define	QUARTER_SECOND		16
#define	EIGHTH_SECOND		8
#define	SIXTEENTH_SECOND	4


int SIM900_Module_Init(void);

void SIM900_Module_Close(void);

int SIM900_REG_GSM(void);

int SIM900_Verify_PIN(unsigned char *pin);

void SIM900_Get_IMEI(unsigned char *imei);

void SIM900_Get_IMSI(unsigned char *imsi);

void SIM900_Get_Ver(unsigned char *ver);

void SIM900_Get_Batt(unsigned char *batt);

int SIM900_Get_Signal(void);

void TCP_Set_APN(unsigned char *apn,unsigned char *user,unsigned char *pwd);

int TCP_Create_Link(unsigned char *ip,unsigned char *port);

int TCP_Check_Link(void);

int TCP_Shut_Link(void);

int TCP_Send_Data(unsigned char *data,int len);

int TCP_Recv_Data(unsigned char *data,int *len,int T);

void FTP_Set_Server(unsigned char *ip,int port);

void FTP_Set_User(unsigned char *user,unsigned char *pwd);

void FTP_Set_GetFile(unsigned char *path,unsigned char *file);

void FTP_Set_PutFile(unsigned char *path,unsigned char *file);

int FTP_Open_Bearer(void);

void FTP_Close_Bearer(void);

int FTP_Get_File(unsigned char *data,int *len);

int FTP_Put_File(unsigned char *data,int len);

#endif
