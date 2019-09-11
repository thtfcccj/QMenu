/***********************************************************************

//		             菜单系统之4位数码管时的管理功能实现

***********************************************************************/
#ifndef _MENU_MNG_H
#define	_MENU_MNG_H

/***********************************************************************
                            说明
***********************************************************************/
//菜单共分三层:
//第一层为菜单选择,显示一组操作字符提示,此时可选择进入或退出,
//第二层为参数显示,可一次性读入显示值,也可实时更新显示值
//第三层为参数调节:当第二层非只读时,按确认键进入
//第二,第三层根据情况可有可无
//菜单系统仅响应进入,退出,上,下键的短按,长按,保持时响应
/***********************************************************************
                    相关配置
***********************************************************************/


/***********************************************************************
                    相关结构-菜单管理器
//与用户交互部见在Menu.h里定义
***********************************************************************/


//用于菜单系统运行管理,用户禁止直接访问
struct _MenuMng{
  //当前所处层,见定义0:不在菜单,否则在1,2,3菜单中
  unsigned char Layer;
  //当前处于第几个菜单位置
  unsigned char Index;
  //缓冲的最后键值,用于减少系统嵌套调用
  unsigned char Key;
  //相关标志,见定义
  unsigned char Flag;
  
  //参数调整时,参数的内部变量:
  unsigned char WrIndex;//调整数值时的位置
  unsigned char WrMaxPos;//调整数值时的最大范围,含
  signed short OrgAdj;  //进入调用时的调整项原始值
  
   //用户挂接的菜单查找表,首位为查找表大小. 可使用此机制实现权限访问
  const unsigned char *pLUT;   
};

//其中,菜单层次见Menu中的层次定义:

//相关标志定义为:
//闪动位强制消隐显示标志,否则按键将使闪动位强制显示,此标志在调用DISP后自动取消
#define MENU_MNG_FLASH_HIDDEN   0x01  

extern struct _MenuMng MenuMng; //直接静态定义

/***********************************************************************
                        相关函数
***********************************************************************/

//--------------------------任务函数--------------------------
//将此函数放入系统250mS进程中,用于更新实时值的显示
void MenuMng_Task(void);

//----------------------更新菜单显示函数--------------------------
//调用此函数主动更新界面显示
void MenuMng_UpdateDisp(void);

//----------------------进入菜单初始化函数--------------------------
//由主界面准备进入菜单时调用挂接的菜单查找表并完成内部初始化
//若pLUT != NULL 则还需调用MenuMng_Key(MENU_MNG_KEY_ENTER)正式进入
void MenuMng_EnterInit(const unsigned char *pLUT);

//----------------------退出菜单函数--------------------------
//在菜单状态,外部强制退出菜单时调用此函数
void MenuMng_Exit(void);
                  
//---------------------初始化函数----------------------------
//开机时调用
//void MenuMng_Init(void);
#include <string.h>
#define MenuMng_Init() do{MenuMng_EnterInit(NULL);}while(0)

//----------------------接键处理函数--------------------------
//形参Key按位定义为:
#define MENU_MNG_KEY_MASK   0x0f  //键值定义掩码
#define MENU_MNG_KEY_MODE   0x70  //按键方式掩码
#define MENU_MNG_KEY_UP     0x01  //上键
#define MENU_MNG_KEY_DOWN   0x02  //下键
#define MENU_MNG_KEY_ENTER  0x04  //进入键
#define MENU_MNG_KEY_EXIT   0x08  //退出键
#define MENU_MNG_KEY_SHORT  0x10  //短按键
#define MENU_MNG_KEY_LONG   0x20  //长按键
#define MENU_MNG_KEY_KEEP   0x40  //保持按键(按住不放)
//void MenuMng_Key(unsigned char Key);
#define MenuMng_Key(key) do{MenuMng.Key = key;}while(0)

//--------------------------快速任务函数--------------------------
//将此函数放入系统快速进程中,用于处理键值
void MenuMng_FastTask(void);

//-------------------------得到当前菜单ID号--------------------
unsigned char MenuMng_GetId(void);

//-------------------------得到调整前蝗值-----------------------
//signed short MenuMng_GetOrgAdj(void);
#define MenuMng_GetOrgAdj() (MenuMng.OrgAdj)

//-------------------------得到当前调整位------------------------
//unsigned char MenuMng_GetWrIndex(void);
#define MenuMng_GetWrIndex() (MenuMng.WrIndex)

//------------------判断是否在实时显示状态------------------------
signed char MenuMng_IsRealDisp(void);

/*********************************************************************
                    资源需求回调函数或所需定义声明
*********************************************************************/
//此模块显示部分独立于系统,需要4个"8.8.8.8"的段码显示,用户需实现列接口
//对8.8.8.8的外部驱动满足:
//1.每个8有独立的显示缓冲,由0~3表示(顺序可指定)
//2.每个8有独立的闪动功能,并用掩码来控制闪动,由位到掩码由应用决定
//3.每个小数点的显示与闪动可独立控制,掩码来控制闪动

#include "Led.h"
#include "LedSign.h"
//---------------------由调整位置得到显示位置掩码----------------------
//MenuMng归定最右侧位置为Pos0,通过此转换为对应数码管或小数点位置
//unsigned char MenuMng_cbPos2Mask(unsigned char Pos);
#define MenuMng_cbPos2Mask(pos) (1 << pos)

//---------------------由十进制数值得到段码显示----------------------
//unsigned char MenuMng_cbVol2Seg(unsigned char Vol);
#define MenuMng_cbVol2Seg(vol) LED_SIGN_NumLut[vol]

//---------------------得到负号的段码表示----------------------
//unsigned char MenuMng_cbGetNegaSeg(void);
#define MenuMng_cbGetNegaSeg() (LED_SIGN__)

//------------------------更新某个显示缓冲区显示------------------------
//void MenuMng_cbSetSegDisp(unsigned char Pos, unsigned char Seg);
#define MenuMng_cbSetSegDisp(pos, seg) do{Led.SegDisp[pos] = seg;}while(0)

//------------------------使掩码对应的数码管闪动------------------------
//void MenuMng_cbSetSegFlash(unsigned char Mask);
#define MenuMng_cbSetSegFlash(mask)  do{Led.SegFlash |= mask;}while(0)

//----------------------------使所有数码管停闪--------------------------
//void MenuMng_cbClrSegFlash(void);
#define MenuMng_cbClrSegFlash()  do{Led.SegFlash &= ~0x0f;}while(0)

//------------------------使掩码对应的小数点显示------------------------
//void MenuMng_cbSetDotDisp(unsigned char Mask);
#define MenuMng_cbSetDotDisp(mask)  do{Led.DotDisp |= mask;}while(0)

//------------------------使所有小数点停显------------------------
//void MenuMng_cbClrDotDisp(void);
#define MenuMng_cbClrDotDisp()  do{Led.DotDisp &= ~0x0f;}while(0)

//------------------------使掩码对应的小数点闪动------------------------
//void MenuMng_cbSetDotFlash(unsigned char Mask);
#define MenuMng_cbSetDotFlash(mask)  do{Led.DotFlash |= mask;}while(0)

//----------------------------使所有小数点停闪----------------------------
//void MenuMng_cbClrDotFlash(void);
#define MenuMng_cbClrDotFlash()  do{Led.DotFlash &= ~0x0f;}while(0)

//------------------------------强制更新以及更新闪动-----------------------
//更新强制标志,同时刷新显示屏
//形参为是否强制在消隐周期
//void MenuMng_cbForceFlash(unsigned char IsForceHidden);
#define MenuMng_cbForceFlash(isforcehidden)\
  do{if(isforcehidden) Led.Flag |= (LED_FLASH_NOW | LED_FLASH_DIS);\
     else Led.Flag |= (LED_FLASH_NOW); Led_Task();}while(0)

//--------------定义在MenuNote中定义的小数点位置掩码----------------------
#define MENU_MNG_CB_DOT_MASK   LED_SIGN_DOT

/***********************************************************************
                    回调函数
***********************************************************************/

//---------------------------退出菜单系统通报-----------------------
void MenuMng_cbExitNotify(void);


#endif //_MENU_MNG_H
