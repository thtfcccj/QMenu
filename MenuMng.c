/***********************************************************************

//		             菜单系统之4位数码管时的管理功能实现

***********************************************************************/

#include "Menu.h"
#include "MenuMng.h"
#include "MenuAdj.h"

#include <string.h>

struct _MenuPara MenuPara;  //用户参考实现
struct _MenuMng MenuMng;   //直接静态定义


/***********************************************************************
                    相关函数实现
***********************************************************************/

//-------------------MenuPara初始化函数--------------------------
static void _MenuParaInit(void)
{
  MenuPara.Adj = 0;      //当前调整值
  MenuPara.Min = 0;      //调整值的最小值,含  
  MenuPara.Max = 9999;   //调整值的最大值,含
  MenuPara.Cfg = 0;     //主配置字,每菜单项均需配置,见定义
  MenuPara.Cfg2 = 0;    //附加配置字,不需要时为0,见定义
}


//------------------判断是否在实时显示状态------------------------
signed char MenuMng_IsRealDisp(void)
{
  if((MenuPara.Cfg & MENU_CFG_REAL_RD) && 
     (MenuMng.Layer == MENU_LAYER_RD)) 
   return -1;
  return 0;
}

//--------------------------任务函数--------------------------
//将此函数放入系统250mS进程中,用于更新实时值的显示
void MenuMng_Task(void)
{
  if(MenuMng_IsRealDisp()){
    MenuFun[MenuMng_GetId()].GetData(MENU_CFG_REAL_RD); //开始实时更新
    MenuMng_UpdateDisp(); //更新显示
  }
}

//----------------------进入菜单初始化函数--------------------------
//由主界面准备进入菜单时调用挂接的菜单查找表并完成内部初始化
void MenuMng_EnterInit(const unsigned char *pLUT)
{
  memset(&MenuPara, 0, sizeof(struct _MenuPara));
  memset(&MenuMng, 0, sizeof(struct _MenuMng));
  MenuMng.pLUT = pLUT;
}

//-------------------------得到当前菜单ID号--------------------
unsigned char MenuMng_GetId(void)
{
  return  *(MenuMng.pLUT + MenuMng.Index + 1);
}

//----------------------更新菜单显示函数--------------------------
//调用此函数主动更新界面显示
void MenuMng_UpdateDisp(void)
{
  unsigned char Id, Data,Pos;
  
  //统一初始化显示部分(不管数码管的显示)
  MenuMng_cbClrSegFlash();
  MenuMng_cbClrDotFlash();
  MenuMng_cbClrDotDisp();  
  
  if(MenuMng.Layer == MENU_LAYER_SEL){//直接用菜单项符号更新显示(含小数点)
    Id = MenuMng_GetId();
    //依次更新显示
    for(Pos = 0; Pos < 4; Pos++){
      Data = MenuNote[Id][Pos];
      MenuMng_cbSetSegDisp(Pos, Data); 
      if(Data & MENU_MNG_CB_DOT_MASK) 
        MenuMng_cbSetDotDisp(MenuMng_cbPos2Mask(Pos));
    }
  }
  else{
    //根据调整方式调用相关模块显示
    switch(MenuPara.Cfg & MENU_CFG_ADJ_MASK){
    case MENU_CFG_ADJ_BIT: 
      MenuAdjBit_Disp(); 
      MenuMng_cbSetDotDisp(MenuPara.Cfg2 & MENU_CFG2_DOT_MASK);//手动打上小数点
      break; 
    case MENU_CFG_ADJ_ALL: 
    case MENU_CFG_ADJ_CMD:       
      MenuAdjAll_Disp(); 
      MenuMng_cbSetDotDisp(MenuPara.Cfg2 & MENU_CFG2_DOT_MASK);//手动打上小数点
      break;
    //case MENU_CFG_ADJ_LOGIC:
    default:MenuAdjLogic_Disp(); break;
    }
  }
  MenuMng_cbForceFlash(MenuMng.Flag & MENU_MNG_FLASH_HIDDEN); //强制更新
  MenuMng.Flag &= ~MENU_MNG_FLASH_HIDDEN;
}

/***********************************************************************
                    按键处理相关函数实现
***********************************************************************/

//---------------------进入读写状态初始化函数--------------------------
void _EnterRdWr(unsigned char IsWr)
{
  if(IsWr){
    MenuMng.Layer = MENU_LAYER_WR;
  }
  else MenuMng.Layer = MENU_LAYER_RD;
  
  MenuMng.WrIndex = 0;
  MenuMng.OrgAdj = MenuPara.Adj;
  
  //根据调整方式初始化相关模块
  switch(MenuPara.Cfg & MENU_CFG_ADJ_MASK){
    case MENU_CFG_ADJ_BIT: MenuAdjBit_Init(); break;
    case MENU_CFG_ADJ_CMD: MenuMng.OrgAdj++;//让命令模式可写
    case MENU_CFG_ADJ_ALL: MenuAdjAll_Init(); break;
    //case MENU_CFG_ADJ_LOGIC:
    default:MenuAdjLogic_Init(); break;
  }
}
//---------------------退出写状态处理函数--------------------------
//形参为写状态:0:禁止写,其它:传递给".SetData"的形参
void _ExitWr(unsigned char WrState)
{
  //保存退出时,若数据未改变,则需根据退出保存进一步处理
  if((WrState == MENU_LAYER_WR) && (MenuMng.OrgAdj == MenuPara.Adj)){
    if(MenuPara.Cfg2 & MENU_CFG2_QUIT_SAVE) //退出时也保存时,调用退出保存
      WrState = MENU_CFG2_QUIT_SAVE;
    else WrState = 0;//退出不保存
  }
  if(WrState != 0){//允许保存时
    unsigned char Id = MenuMng_GetId(); //PIC这要才管用
    MenuFun[Id].SetData(WrState); //写数据    
  }  
  if(MenuPara.Cfg2 & MENU_CFG2_QUIT){ //直接退出菜单
    MenuMng.Layer = MENU_LAYER_NULL;
    return;
  }
  
  MenuFun[MenuMng_GetId()].GetData(MENU_LAYER_SEL);//更新一下数据获得写结果或更新
  if(MenuPara.Cfg & MENU_CFG_WR) //只写时直接退至选择状态
    MenuMng.Layer = MENU_LAYER_SEL;
  else
    _EnterRdWr(0); //退出到读状态并初始化
}

//---------------------菜单选择状态按键处理函数--------------------------
void _SelKey(unsigned char Key)
{
  //响应按键处理
  if(Key & MENU_MNG_KEY_LONG) return; //不响应长按键
  
  if(Key == (MENU_MNG_KEY_EXIT | MENU_MNG_KEY_SHORT)){//短按退出菜单
    MenuMng.Layer = MENU_LAYER_NULL;
    return;
  }
  
  if(Key == (MENU_MNG_KEY_ENTER | MENU_MNG_KEY_SHORT)){//短按进入菜单
    if(MenuPara.Cfg & MENU_CFG_WR){ //直接进入写状态
      MenuFun[MenuMng_GetId()].GetData(MENU_LAYER_WR);//写数据准备
      _EnterRdWr(1);//进入写状态
    }
    else _EnterRdWr(0);//进入读状态
    return;
  }
  //菜单项加减
  if(Key & MENU_MNG_KEY_UP){//菜单项加
    MenuMng.Index++;
    if(MenuMng.Index >= *MenuMng.pLUT) MenuMng.Index = 0;//回环了
  }
  else if(Key & MENU_MNG_KEY_DOWN){//菜单项减
    if(!MenuMng.Index) MenuMng.Index = *MenuMng.pLUT - 1;//回环了
    else MenuMng.Index--;
  }
  //else return; //保存与退出保持时
  
  _MenuParaInit(); //重新初始化
  MenuFun[MenuMng_GetId()].GetData(MenuMng.Layer);//菜单选择状态预读数据准备
}

//---------------------菜单参数显示(读)状态按键处理函数--------------------------
void _RdKey(unsigned char Key)
{
  /*/只读时,长保存键,若用户有保存函数,则调用该函数执行保存功能
  if((Key & (MENU_MNG_KEY_LONG | MENU_MNG_KEY_ENTER)) ==
     (MENU_MNG_KEY_LONG | MENU_MNG_KEY_ENTER)){
    if(MenuPara.Cfg & MENU_CFG_RD){//只读时允许进入   
      if(MenuFun[MenuMng_GetId()].SetData != NULL){
        MenuFun[MenuMng_GetId()].SetData(MENU_CFG_WR);//写数据
        MenuFun[MenuMng_GetId()].GetData(MENU_LAYER_SEL);//更新一下数据获得写结果
        MenuMng.Layer = MENU_LAYER_SEL;//退到上层菜单
      }
    }
    return;
  }*/ //已用 MENU_CFG_ADJ_CMD功能代替
  //不响应长按与保持按键
  if(Key & (MENU_MNG_KEY_LONG | MENU_MNG_KEY_KEEP)) return;

  if(Key & MENU_MNG_KEY_ENTER){//短按确认键进入写状态
    if(!(MenuPara.Cfg & MENU_CFG_RD)){//非只读时允许进入
      MenuFun[MenuMng_GetId()].GetData(MENU_LAYER_WR);//写数据准备
      _EnterRdWr(1);//进入写状态
    }
  }
  else if(Key & MENU_MNG_KEY_EXIT){//短按退出键退出显示状态
    MenuMng.Layer = MENU_LAYER_SEL;
  }
  else{ //上下键
    //将逻辑时的上下键解释为翻页显示
    if((MenuPara.Cfg & MENU_CFG_ADJ_MASK) >= MENU_CFG_ADJ_LOGIC){
      MenuAdjLogic_RdPageKey();
    }
  }
}

//---------------------菜单参数调整(写)状态按键处理函数-------------------------
void _WrKey(unsigned char Key)
{
  if(Key & MENU_MNG_KEY_LONG){//长按键
    if(Key & MENU_MNG_KEY_ENTER) _ExitWr(MENU_LAYER_WR);//长按确认键保存并退出
    return;
  }
  if(Key & MENU_MNG_KEY_EXIT){//短按退出菜单
    if(Key & MENU_MNG_KEY_SHORT){
      if(MenuPara.Cfg2 & MENU_CFG2_QUIT_SAVE) //退出时也保存
        _ExitWr(MENU_CFG2_QUIT_SAVE);
      else _ExitWr(0);//退出时不保存
    }
    return;
  }
  
  //其它按键根据调整方式决定
  switch(MenuPara.Cfg & MENU_CFG_ADJ_MASK){
    case MENU_CFG_ADJ_BIT: MenuAdjBit_Key(Key); break;
    case MENU_CFG_ADJ_ALL: MenuAdjAll_Key(Key); break;
    case MENU_CFG_ADJ_LOGIC: MenuAdjLogic_Key(Key); break;
    //case MENU_CFG_ADJ_CMD:      //左右按键无效
    default: break;
  }
  if(MenuPara.Cfg & MENU_CFG_REAL_WR){//实时写时,数据变化写数据
    if(Key & (MENU_MNG_KEY_UP | MENU_MNG_KEY_DOWN)){
      unsigned char Id = MenuMng_GetId(); //PIC这要才管用
      MenuFun[Id].SetData(MENU_CFG_REAL_WR); //实时写数据
    }
  }
}

//--------------------------快速任务函数--------------------------
//将此函数放入系统快速进程中,用于处理键值
void MenuMng_FastTask(void)
{
  unsigned char Key = MenuMng.Key;
  if(!Key) return; //无键值
  MenuMng.Key = 0;
  
  unsigned char PrvLayer = MenuMng.Layer;
  switch(MenuMng.Layer){
  case MENU_LAYER_NULL: //不在菜单系统
    if(Key != MENU_MNG_KEY_ENTER) break; //
    MenuMng.Layer++; //进入
    Key = 0;//继续往下处理
  case MENU_LAYER_SEL: //菜单系统选择:不响应长按键
    _SelKey(Key);
    break;
  case MENU_LAYER_RD: //菜单显示数据状态
   _RdKey(Key);
    break;
  case MENU_LAYER_WR: //菜单调整数据状态
   _WrKey(Key);
    break;
  default: //异常退出
    MenuMng.Layer = MENU_LAYER_NULL;
    break;
  }
  if(MenuMng.Layer != MENU_LAYER_NULL) MenuMng_UpdateDisp();//更新显示
  else if(PrvLayer != MENU_LAYER_NULL){//退出菜单系统
    MenuMng_cbExitNotify(); //最后退出菜单系统通报
  }
}

//----------------------退出菜单函数--------------------------
//在菜单状态,外部强制退出菜单时调用此函数
void MenuMng_Exit(void)
{
  if(MenuMng.Layer == MENU_LAYER_NULL) return; 
  
  if(MenuPara.Cfg2 & MENU_CFG2_QUIT_SAVE){ //退出时也保存
    MenuPara.Cfg2 |= MENU_CFG2_QUIT;//调用前临时置强制退出状态
    _ExitWr(MENU_CFG2_QUIT_SAVE);
  }
  MenuMng_cbExitNotify(); //最后退出菜单系统通报
}  
