/***********************************************************************
		             菜单系统之4位数码管-整体调节模式实现
//此模块为MenuMng的内部实现,非独立使用模块
***********************************************************************/

#include "Menu.h"
#include "MenuMng.h"
#include "MenuAdj.h"

//-------------------------按键处理函数---------------------------
void MenuAdjAll_Key(unsigned char Key)
{
  //确认键无效
  if(Key & MENU_MNG_KEY_ENTER) return;
  //上键调整
  if(Key & MENU_MNG_KEY_UP){
    if(MenuPara.Adj < MenuPara.Max) MenuPara.Adj++;
    else MenuPara.Adj = MenuPara.Min;//回环了
    return;
  }
  //下键调整
  if(MenuPara.Adj > MenuPara.Min) MenuPara.Adj--;
  else MenuPara.Adj = MenuPara.Max;//回环了
}

//----------------------显示更新函数---------------------------
void MenuAdjAll_Disp(void)
{
  MenuAdj_DispVol(); //先更新值显示
  if(MenuMng.Layer == MENU_LAYER_WR){//写状态置全部调整位闪动
    MenuMng_cbSetSegFlash(0xff);
    MenuMng_cbSetDotFlash(MenuPara.Cfg2 & 0x0f);//将小数点一起置为闪动
  }
}

