
#include <Api.h>
#include "UiProg.h"
#include "Common.h"

static void DisplaySelectionBox(int idx, int Font);
static void Select_Frame(int x,int y,int col);
static void ClearSelectionBox(int idx, int Font);
static int Disp_IconEx(SMemu *pSMObj, int Prompt, int Font);

static void Select_Frame(int x,int y,int col)
{
	int i,j;	
	for(i = x-2;i <= x+64+2;i++)
	{
		Disp_Put_Pixel_Col(i,y-1,col);
		Disp_Put_Pixel_Col(i,y-2,col);
		Disp_Put_Pixel_Col(i,y+64+1,col);
		Disp_Put_Pixel_Col(i,y+64+2,col);
	}
	for(j = y-2;j <= y+64+2;j++)
	{
		Disp_Put_Pixel_Col(x-1,j,col);
		Disp_Put_Pixel_Col(x-2,j,col);
		Disp_Put_Pixel_Col(x+64+1,j,col);
		Disp_Put_Pixel_Col(x+64+2,j,col);
	}
}

static int Disp_IconEx(SMemu *pSMObj, int Prompt, int Font)
{
	int MCount = 0;	
	int iPage = 0;
	char CharTemp[40];
	unsigned char buf[1024 * 1024];
	unsigned long len;
	char txt[200];
	int i = 0,j = 0;
	MenuInfo *pMIobj = NULL;
	
	if(pSMObj == NULL)
	{
		DispStr_CE(0, (3 * g_LINESpacing), "函数参数指针为空", DISP_CENTER | DISP_CLRSCR);
		memset(CharTemp, '\0', sizeof(CharTemp));
		sprintf(CharTemp, "%s", __FUNCTION__);
		DispStr_CE(0, (5 * g_LINESpacing), CharTemp, DISP_CENTER); 								
		delay_and_wait_key(30, EXIT_KEY_ALL, 30);		
		return(1);
	}
	
	Disp_Clear();
	for(i = 0;i <= 240;i++)								// 画横线
	{
		Disp_Put_Pixel_Col(i,58,0x0000);
		Disp_Put_Pixel_Col(i,134,0x0000);
		Disp_Put_Pixel_Col(i,210,0x0000);
		Disp_Put_Pixel_Col(i,286,0x0000);
	}
	for(j = 58;j <= (58+240-12);j++)					// 画坚线
	{
		Disp_Put_Pixel_Col(82,j,0x0000);
		Disp_Put_Pixel_Col(158,j,0x0000);
	}
	
	//计算此页码有多少菜单
	iPage = pSMObj->m_CPage;
	MCount = (pSMObj->m_MCount - (iPage * 9));
	if(MCount >= 9)
	{MCount = 9;}
	for(i = 0; i < MCount; i++)
	{
		pMIobj = &pSMObj->m_Mem[iPage][i]; 
		if(pSMObj->m_BType == 0)
		{
			memset((char*)buf, '\0', sizeof(buf));
			JPG2BMP(pMIobj->m_pMume, pMIobj->m_MSize,(char *)buf, &len, txt);
			if(Font == 0)
			{Disp_Put_Bmp_New(pMIobj->m_xpos, pMIobj->m_ypos, buf);}
			else
			{Disp_Put_Bmp_New(pMIobj->m_xpos, pMIobj->m_ypos + 20, buf);}
		}
		else
		{
			if(Font == 0)
			{Disp_Put_Bmp_New(pMIobj->m_xpos, pMIobj->m_ypos, (unsigned char*)pMIobj->m_pMume);}
			else
			{Disp_Put_Bmp_New(pMIobj->m_xpos, pMIobj->m_ypos + 20, (unsigned char*)pMIobj->m_pMume);}
		}
	}
	Disp_Icon_Battery_Time_Refresh();
	if(Prompt == 0)
	{DispStr_CE(0, 36, "F1-退出      Enter-选择",DISP_CENTER);}	
	else if(Prompt == 1)
	{DispStr_CE(0, 36, "Enter-选择",DISP_CENTER);}
	else if(Prompt == 2)
	{DispStr_CE(0, 36, "F1-返回      Enter-选择",DISP_CENTER);}
	else
	{
		DispStr_CE(0, 34, "F1-返回/Return", DISP_CENTER | DISP_CLRLINE);
		DispStr_CE(0, 36, "Enter-选择/Choice", DISP_CENTER | DISP_CLRLINE);		
	}
	return(0);
}

int Browse_Icon(char *pTitle, char *menu, SMemu *pSMObj, int LineLen, int idx, int Prompt, int Font)
{
	long key;
	//int idx = 1;
	char MenuT[40];

	clr_scr();
	//Disp_Set_Magnification(2);
	Disp_IconEx(pSMObj, Prompt, Font);
	DisplaySelectionBox(idx, Font);
	while(1)
	{		
		if(Font == 0)
		{	
			DispStr_CE(0, 2, pTitle, DISP_CENTER);			
			memset(MenuT, '\0', sizeof(MenuT));
			memcpy(MenuT, &menu[((pSMObj->m_CPage * 9) + (idx-1))* LineLen], LineLen);
			DispStr_CE(0, 5, MenuT, DISP_POSITION | DISP_CLRLINE);			
		}
		else
		{
			Disp_Set_Magnification(2);
			DispStr_CE(0, 2, pTitle, DISP_CENTER);			
			Disp_Set_Magnification(1);			

			memset(MenuT, '\0', sizeof(MenuT));
			memcpy(MenuT, &menu[((pSMObj->m_CPage * 9) + (idx-1))* LineLen], LineLen);
			DispStr_CE(0, 6, MenuT, DISP_POSITION | DISP_CLRLINE);			
		}
		//自动关机
		if(AutoPowerOff == 0)
		{key = delay_and_wait_key(30, EXIT_KEY_ALL, 30);}
		else
		{
			key = delay_and_wait_key(AutoPowerOff, EXIT_KEY_ALL, AutoPowerOff);
			if(key == EXIT_AUTO_QUIT)				
			{Sys_Power_Sleep(3);}
			else
			{Disp_Icon_Battery_Time_Refresh();}
		}
		if(key == EXIT_AUTO_QUIT)
		{
			Disp_Icon_Battery_Time_Refresh();
		}		
		else
		{
			switch(key)
			{
				case EXIT_KEY_1:
					{
						if(1 <= pSMObj->m_PMCount[pSMObj->m_CPage])
						{return ((1 + (pSMObj->m_CPage * 9) - 1));}
					}break;
				
				case EXIT_KEY_2:
					{
						if(2 <= pSMObj->m_PMCount[pSMObj->m_CPage])
						{return ((2 + (pSMObj->m_CPage * 9) - 1));}
					}break;
				
				case EXIT_KEY_3:
					{
						if(3 <= pSMObj->m_PMCount[pSMObj->m_CPage])
						{return ((3 + (pSMObj->m_CPage * 9) - 1));}
					}break;
				
				case EXIT_KEY_4:
					{
						if(4 <= pSMObj->m_PMCount[pSMObj->m_CPage])
						{return ((4 + (pSMObj->m_CPage * 9) - 1));}
					}break;

			case EXIT_KEY_5:
				{
					if(5 <= pSMObj->m_PMCount[pSMObj->m_CPage])
					{return ((5 + (pSMObj->m_CPage * 9) - 1));}
				}break;

			case EXIT_KEY_6:
				{
					if(6 <= pSMObj->m_PMCount[pSMObj->m_CPage])
					{return ((6 + (pSMObj->m_CPage * 9) - 1));}
				}break;
			
			case EXIT_KEY_7:
				{
					if(7 <= pSMObj->m_PMCount[pSMObj->m_CPage])
					{return ((7 + (pSMObj->m_CPage * 9) - 1));}
				}break;
			
			case EXIT_KEY_8:
				{
					if(8 <= pSMObj->m_PMCount[pSMObj->m_CPage])
					{return ((8 + (pSMObj->m_CPage * 9) - 1));}
				}break;

			case EXIT_KEY_9:
				{
					if(9 <= pSMObj->m_PMCount[pSMObj->m_CPage])
					{return ((9 + (pSMObj->m_CPage * 9) - 1));}
				}break;
				
			}	
			switch(key)
			{									
				case EXIT_KEY_UP:
				{
					if(idx <= 1)		
					{					
					  //判断是否需要换页
					  //是否到开始页码
					  if(pSMObj->m_CPage == pSMObj->m_SPage)
						{
							//设置当前页为最后一页码
							pSMObj->m_CPage = (pSMObj->m_PCount - 1);

							//清除
							ClearSelectionBox(idx, Font);
							
							idx = pSMObj->m_PMCount[pSMObj->m_CPage];
							Disp_IconEx(pSMObj, Prompt, Font);
						}
						//没有到开始页码
						else
						{
							//换到下一页码
							(pSMObj->m_CPage)--;
							//选择第一个
							idx = 1;
							Disp_IconEx(pSMObj, Prompt, Font);
						}
					}
					else 
					{idx--;}
				}break;
				
				case EXIT_KEY_DOWN:
				{
					if(idx >= (pSMObj->m_PMCount[pSMObj->m_CPage]))		
					{
					  //判断是否需要换页
					  if(pSMObj->m_CPage == (pSMObj->m_PCount - 1))
						{
							//转到开始页
							pSMObj->m_CPage = pSMObj->m_SPage;

							//清除
							ClearSelectionBox(idx, Font);
							//选择第一个
							idx = 1;
							Disp_IconEx(pSMObj, Prompt, Font);
 						}
						//没有到最后一页码
						else
						{
							//换下一页码
							(pSMObj->m_CPage)++;
							//选择第一个
							idx = 1;
							Disp_IconEx(pSMObj, Prompt, Font);
						}
					}
					else 
					{idx++;}
				}break;
				
				case EXIT_KEY_ENTER:
					{return ((idx + (pSMObj->m_CPage * 9) - 1));}
					
				case EXIT_KEY_F1:
					{return -1;}
			}
			DisplaySelectionBox(idx, Font);
		}
	}
}

static void DisplaySelectionBox(int idx, int Font)
{
	if(Font == 0)
	{
		switch(idx)
		{
			case 1:
				Select_Frame(164,216,0xFFFF);
				Select_Frame(12,64,0xF000);
				Select_Frame(88,64,0xFFFF);
				break;
			case 2:
				Select_Frame(12,64,0xFFFF);
				Select_Frame(88,64,0xF000);
				Select_Frame(164,64,0xFFFF);
				break;
			case 3:
				Select_Frame(88,64,0xFFFF);
				Select_Frame(164,64,0xF000);
				Select_Frame(12,140,0xFFFF);
				break;
			case 4:
				Select_Frame(164,64,0xFFFF);
				Select_Frame(12,140,0xF000);
				Select_Frame(88,140,0xFFFF);
				break;
			case 5:
				Select_Frame(12,140,0xFFFF);
				Select_Frame(88,140,0xF000);
				Select_Frame(164,140,0xFFFF);
				break;
			case 6:
				Select_Frame(88,140,0xFFFF);
				Select_Frame(164,140,0xF000);
				Select_Frame(12,216,0xFFFF);
				break;
			case 7:
				Select_Frame(164,140,0xFFFF);
				Select_Frame(12,216,0xF000);
				Select_Frame(88,216,0xFFFF);
				break;
			case 8:
				Select_Frame(12,216,0xFFFF);
				Select_Frame(88,216,0xF000);
				Select_Frame(164,216,0xFFFF);
				break;
			case 9:
				Select_Frame(88,216,0xFFFF);
				Select_Frame(164,216,0xF000);
				Select_Frame(12,64,0xFFFF);
				break;
		}	
	}
	else
	{
		switch(idx)
		{
			case 1:
				Select_Frame(164,216 + 20,0xFFFF);
				Select_Frame(12,64 + 20,0xF000);
				Select_Frame(88,64 + 20,0xFFFF);
				break;
			case 2:
				Select_Frame(12,64 + 20,0xFFFF);
				Select_Frame(88,64 + 20,0xF000);
				Select_Frame(164,64 + 20,0xFFFF);
				break;
			case 3:
				Select_Frame(88,64 + 20,0xFFFF);
				Select_Frame(164,64 + 20,0xF000);
				Select_Frame(12,140 + 20,0xFFFF);
				break;
			case 4:
				Select_Frame(164,64 + 20,0xFFFF);
				Select_Frame(12,140 + 20,0xF000);
				Select_Frame(88,140 + 20,0xFFFF);
				break;
			case 5:
				Select_Frame(12,140 + 20,0xFFFF);
				Select_Frame(88,140 + 20,0xF000);
				Select_Frame(164,140 + 20,0xFFFF);
				break;
			case 6:
				Select_Frame(88,140 + 20,0xFFFF);
				Select_Frame(164,140 + 20,0xF000);
				Select_Frame(12,216 + 20,0xFFFF);
				break;
			case 7:
				Select_Frame(164,140 + 20,0xFFFF);
				Select_Frame(12,216 + 20,0xF000);
				Select_Frame(88,216 + 20,0xFFFF);
				break;
			case 8:
				Select_Frame(12,216 + 20,0xFFFF);
				Select_Frame(88,216 + 20,0xF000);
				Select_Frame(164,216 + 20,0xFFFF);
				break;
			case 9:
				Select_Frame(88,216 + 20,0xFFFF);
				Select_Frame(164,216 + 20,0xF000);
				Select_Frame(12,64 + 20,0xFFFF);
				break;
		}	
		
	}
}

static void ClearSelectionBox(int idx, int Font)
{
	if(Font == 0)
	{
		switch(idx)
		{
			case 1:
				Select_Frame(12,64,0xFFFF); break;
				
			case 2:
				Select_Frame(88,64,0xFFFF); break;
				
			case 3:
				Select_Frame(164,64,0xFFFF); break;
				
			case 4:
				Select_Frame(12,140,0xFFFF); break;
				
			case 5:
				Select_Frame(88,140,0xFFFF); break;
				
			case 6:
				Select_Frame(164,140,0xFFFF); break;
				
			case 7:
				Select_Frame(12,216,0xFFFF); break;
				
			case 8:
				Select_Frame(88,216,0xFFFF); break;
				
			case 9:
				Select_Frame(164,216,0xFFFF); break;
				
		}
	}	
	else
	{
		switch(idx)
		{
			case 1:
				Select_Frame(12,64 + 20,0xFFFF); break;
				
			case 2:
				Select_Frame(88,64 + 20,0xFFFF); break;
				
			case 3:
				Select_Frame(164,64 + 20,0xFFFF); break;
				
			case 4:
				Select_Frame(12,140 + 20,0xFFFF); break;
				
			case 5:
				Select_Frame(88,140 + 20,0xFFFF); break;
				
			case 6:
				Select_Frame(164,140 + 20,0xFFFF); break;
				
			case 7:
				Select_Frame(12,216 + 20,0xFFFF); break;
				
			case 8:
				Select_Frame(88,216 + 20,0xFFFF); break;
				
			case 9:
				Select_Frame(164,216 + 20,0xFFFF); break;
		}		
	}
}

