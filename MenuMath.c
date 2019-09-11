/***********************************************************************
		             菜单系统之4位数码管-公共部分实现
//此模块为所有调整实现的总接口
***********************************************************************/

#include "Menu.h"
#include "MenuMng.h"
#include "MenuAdj.h"

//-----------------------带符号数取绝对值函数--------------------
unsigned short MenuMath_Ss2Su(signed short Data)
{
  if(Data >= 0) return (unsigned short)Data;
  return (unsigned short)(0 - Data);
}

