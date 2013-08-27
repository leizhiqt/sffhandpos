#ifndef   Gprs_h_2012_07_17_11_17_38_INCLUDED

#define   Gprs_h_2012_07_17_11_17_38_INCLUDED

short sim900_init(void);

void sim900_close(void);

short ConnectServer();

void DisConnectServer();

short SendData(unsigned char* data);

short GetRecvData(unsigned char* recvdata);

#endif   /*Gprs_h_2012_07_17_11_17_38_INCLUDED*/


