/***********************************************************************
		             菜单系统之4位数码管-公共部分实现
//此模块为所有调整实现的总接口
***********************************************************************/

#include "Menu.h"
#include "MenuMng.h"
#include "MenuAdj.h"
#include "MenuMath.h"

//----------------------更新最大调整位置函数---------------------------
//用于决定显示多少位
void MenuAdj_UpdateMaxPos(void)
{
  unsigned char Pos;
  //以绝对值作为条件
  unsigned short Max,Min; 
  Max = MenuMath_Ss2Su(MenuPara.Max);
  Min = MenuMath_Ss2Su(MenuPara.Min);
  if(Min > Max) Max = Min;
  //Min = MenuMath_Ss2Su(MenuPara.Adj); 
  //if(Min > Max) Max = Min;
  
  if(Max >= 1000) Pos = 3; //最大4位
  else{
    if(Max >= 100) Pos = 2;
    else if(Max >= 10) Pos = 1;
    else Pos = 0;
    if(MenuPara.Cfg2 & MENU_CFG2_NEGATIVE) Pos++;//带符号了
  }
  MenuMng.WrMaxPos = Pos;
}


//---------------------重新设定新数值函数-----------------------
void MenuAdj_SetValue(signed short Value)
{
  if(Value < MenuPara.Min) Value = MenuPara.Min;
  else if(Value > MenuPara.Max) Value = MenuPara.Max;
  MenuPara.Adj = Value;          
}

//----------------------显示数值函数---------------------------
//仅更新数值显示,不负责小数点,闪动位驱动
#include "LedNum4.h" //LedNum4_Disp();
void MenuAdj_DispVol(void)
{
  unsigned char Flag = MenuMng.WrMaxPos; //小数点
  if(MenuPara.Cfg2 & MENU_CFG2_NEGATIVE){
    Flag--;//符号位小数点不算
    Flag |= 0x04;
    //调整模式时,正号也要带符号
    if(MenuMng.Layer == MENU_LAYER_WR)
      Flag |= 0x08;      
  }
  
  LedNum4_Disp(MenuPara.Adj, Flag);  //调用辅助函数
}

