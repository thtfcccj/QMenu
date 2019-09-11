/***********************************************************************
		             菜单系统之4位数码管-按位调整实现
//此模块为MenuMng的内部实现,非独立使用模块
***********************************************************************/

#include "Menu.h"
#include "MenuMng.h"
#include "MenuAdj.h"
#include "MenuMath.h"

//----------------得到指定位置数值函数------------------------
unsigned char _GetDigitalInPos(unsigned short Value,
                               unsigned char Pos)
{
  while(Pos--){
    Value /= 10;
  }
  return Value % 10;
}

//-----------------------初始化函数---------------------------
void MenuAdjBit_Init(void)
{
  MenuAdj_UpdateMaxPos();
  MenuMng.WrIndex = MenuMng.WrMaxPos; //用作调整位
}

//由调整项得到倍率
static __flash const unsigned short _Bit2Muti[4] = {1,10,100,1000};

//-------------------------按键处理函数---------------------------
void MenuAdjBit_Key(unsigned char Key)
{
  unsigned short Value, Muti;
  unsigned char Vol;
  
  //确认键在调整位间切换
  if(Key & MENU_MNG_KEY_ENTER){
    if(MenuMng.WrIndex) MenuMng.WrIndex--;
    else MenuMng.WrIndex = MenuMng.WrMaxPos; //回环到最高位了
    MenuMng.Flag |= MENU_MNG_FLASH_HIDDEN; //切换时被切换位消隐
    return;
  }
  //上下键符号位调整
  if((MenuPara.Cfg2 & MENU_CFG2_NEGATIVE) && (MenuMng.WrIndex == MenuMng.WrMaxPos)){
    MenuPara.Adj = 0 - MenuPara.Adj; //反转
    return;
  }
  //上下键调整
  Value = MenuMath_Ss2Su(MenuPara.Adj);
  Muti = _Bit2Muti[MenuMng.WrIndex];//倍率数
  Vol = _GetDigitalInPos(Value,MenuMng.WrIndex);  //得到指定位数值
  Value -= Muti * Vol; //去掉该位了
  if(Key & MENU_MNG_KEY_UP){
    if(Vol < 9) Vol++;
    else Vol = 0;
  }
  else{
    if(Vol) Vol--;
    else Vol =  9;
  }
  Value += Vol * Muti; //新的该位数值
  
  //设定新数值
  if(MenuPara.Adj < 0) MenuAdj_SetValue(0 - Value);
  else MenuAdj_SetValue(Value);
}

//----------------------显示更新函数---------------------------
void MenuAdjBit_Disp(void)
{
  MenuAdj_DispVol(); //先更新值显示
  if(MenuMng.Layer == MENU_LAYER_WR){//写状态置正在调整位闪动
    MenuMng_cbSetSegFlash(MenuMng_cbPos2Mask(MenuMng.WrIndex));
  }
}

