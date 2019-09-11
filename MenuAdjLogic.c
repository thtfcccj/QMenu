/***********************************************************************
		             菜单系统之4位数码管-逻辑数调节模式实现
//最低位逻辑值,位于高位显示
//WrIndex位移值分别为:3,2,1,0, 7,6,5,4
//对就调整位分别为:   0,1,2,3, 4,5,6,7
//逻辑数的可调节位由Max位控制,置1时表示可调节,否则只读
//此模块为MenuMng的内部实现,非独立使用模块
***********************************************************************/

#include "Menu.h"
#include "MenuMng.h"
#include "MenuAdj.h"

//-----------------------得到下个可调整位---------------------------
//>= 8表未找到
static unsigned char _GetNextWrIndex(unsigned char NextIndex)
{
  for(; NextIndex < 8; NextIndex++)
    if((1 << NextIndex) & MenuPara.Max) break;
  return NextIndex;
}

//-----------------------初始化函数---------------------------
void MenuAdjLogic_Init(void)
{
  MenuMng.WrIndex = _GetNextWrIndex(0);
}

//-------------------------按键处理函数---------------------------
void MenuAdjLogic_Key(unsigned char Key)
{
  unsigned char Data;
  
  //确认键切换调整位, MenuMng.WrIndex表示当前调整的数值位
  if(Key & MENU_MNG_KEY_ENTER){
    Data = _GetNextWrIndex(MenuMng.WrIndex + 1);
    if(Data >= 8) Data = _GetNextWrIndex(0); //回环了
    MenuMng.WrIndex = Data;
    
    MenuMng.Flag |= MENU_MNG_FLASH_HIDDEN; //切换时被切换位消隐
    return;
  }
  
  //上下键切换逻辑
  Data = 1 << MenuMng.WrIndex;
  if(MenuPara.Adj & Data) MenuPara.Adj &= ~Data;
  else MenuPara.Adj |= Data;
}

//-----------------读数据时的上下键翻页处理函数---------------------------
void MenuAdjLogic_RdPageKey(void)
{
  unsigned char Data = _GetNextWrIndex(MenuMng.WrIndex + 4);
  if(Data >= 8) Data = _GetNextWrIndex(0); //回环了
  MenuMng.WrIndex = Data;
}

//----------------------显示更新函数---------------------------
void MenuAdjLogic_Disp(void)
{
  unsigned char Pos,IsHi;
  unsigned char Data = MenuPara.Adj;
  
  if(MenuMng.WrIndex >= 4) Data >>= 4;//显示高位值及符号
  
  //将最小数送最左边
  for(Pos = 0; Pos < 4; Pos++){ //判断数值
    if(Data & (1 << Pos)) IsHi = 1;
    else  IsHi = 0;
    MenuMng_cbSetSegDisp(3 - Pos, MenuMng_cbVol2Seg(IsHi));     
  }
  
  if(MenuMng.WrIndex >= 4){
    Pos = MenuMng.WrIndex - 4;
    IsHi = 0x02;
  }
  else{
    Pos = MenuMng.WrIndex;
    IsHi = 0x08;    
  }
  MenuMng_cbSetDotDisp(IsHi); //置小数点表示在第2页
  
  if(MenuMng.Layer == MENU_LAYER_WR)//写状态置调整位闪动
    MenuMng_cbSetSegFlash(MenuMng_cbPos2Mask(3 - Pos));
}

