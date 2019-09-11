/***********************************************************************
		             �˵�ϵͳ֮4λ�����-��������ʵ��
//��ģ��Ϊ���е���ʵ�ֵ��ܽӿ�
***********************************************************************/

#include "Menu.h"
#include "MenuMng.h"
#include "MenuAdj.h"
#include "MenuMath.h"

//----------------------����������λ�ú���---------------------------
//���ھ�����ʾ����λ
void MenuAdj_UpdateMaxPos(void)
{
  unsigned char Pos;
  //�Ծ���ֵ��Ϊ����
  unsigned short Max,Min; 
  Max = MenuMath_Ss2Su(MenuPara.Max);
  Min = MenuMath_Ss2Su(MenuPara.Min);
  if(Min > Max) Max = Min;
  //Min = MenuMath_Ss2Su(MenuPara.Adj); 
  //if(Min > Max) Max = Min;
  
  if(Max >= 1000) Pos = 3; //���4λ
  else{
    if(Max >= 100) Pos = 2;
    else if(Max >= 10) Pos = 1;
    else Pos = 0;
    if(MenuPara.Cfg2 & MENU_CFG2_NEGATIVE) Pos++;//��������
  }
  MenuMng.WrMaxPos = Pos;
}


//---------------------�����趨����ֵ����-----------------------
void MenuAdj_SetValue(signed short Value)
{
  if(Value < MenuPara.Min) Value = MenuPara.Min;
  else if(Value > MenuPara.Max) Value = MenuPara.Max;
  MenuPara.Adj = Value;          
}

//----------------------��ʾ��ֵ����---------------------------
//��������ֵ��ʾ,������С����,����λ����
#include "LedNum4.h" //LedNum4_Disp();
void MenuAdj_DispVol(void)
{
  unsigned char Flag = MenuMng.WrMaxPos; //С����
  if(MenuPara.Cfg2 & MENU_CFG2_NEGATIVE){
    Flag--;//����λС���㲻��
    Flag |= 0x04;
    //����ģʽʱ,����ҲҪ������
    if(MenuMng.Layer == MENU_LAYER_WR)
      Flag |= 0x08;      
  }
  
  LedNum4_Disp(MenuPara.Adj, Flag);  //���ø�������
}

