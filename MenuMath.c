/***********************************************************************
		             �˵�ϵͳ֮4λ�����-��������ʵ��
//��ģ��Ϊ���е���ʵ�ֵ��ܽӿ�
***********************************************************************/

#include "Menu.h"
#include "MenuMng.h"
#include "MenuAdj.h"

//-----------------------��������ȡ����ֵ����--------------------
unsigned short MenuMath_Ss2Su(signed short Data)
{
  if(Data >= 0) return (unsigned short)Data;
  return (unsigned short)(0 - Data);
}

