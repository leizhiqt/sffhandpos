
#ifndef _UIPROG_HEAD_
#define _UIPROG_HEAD_

extern char SetTime[];
extern char FormDB[];
extern char ExitSet[];
extern char Offline[];
extern char Upload[];
extern char Check[];
extern char SysSet[];
extern char VerInfo[]; 
extern char SCard[]; 


typedef struct
{
	//X位置
	unsigned int m_xpos;

	//Y位置
	unsigned int m_ypos;
	
	//菜单指针
	char *m_pMume;

	//菜单长度
	int m_MSize;

}MenuInfo;

//菜单结构
typedef struct
{
	//图片格式
	// 0 jpg
	// 1 bmp
	int m_BType;
	
	//菜单总数
	int m_MCount;

	//总页数
	int m_PCount;
	
	//菜单的数据指针
	//最大3页码,每页码9菜单
	MenuInfo m_Mem[3][9];

	//每一页码的菜单数
	int m_PMCount[3];
	
	//菜单开始页
	int m_SPage;

	//菜单当前页面
	int m_CPage;	
}SMemu;

int Browse_Icon(char *pTitle, char *menu, SMemu *pSMObj, int LineLen, int idx, int Prompt, int Font);

#endif
