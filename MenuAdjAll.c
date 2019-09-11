/***********************************************************************
		             �˵�ϵͳ֮4λ�����-�������ģʽʵ��
//��ģ��ΪMenuMng���ڲ�ʵ��,�Ƕ���ʹ��ģ��
***********************************************************************/

#include "Menu.h"
#include "MenuMng.h"
#include "MenuAdj.h"

//-------------------------����������---------------------------
void MenuAdjAll_Key(unsigned char Key)
{
  //ȷ�ϼ���Ч
  if(Key & MENU_MNG_KEY_ENTER) return;
  //�ϼ�����
  if(Key & MENU_MNG_KEY_UP){
    if(MenuPara.Adj < MenuPara.Max) MenuPara.Adj++;
    else MenuPara.Adj = MenuPara.Min;//�ػ���
    return;
  }
  //�¼�����
  if(MenuPara.Adj > MenuPara.Min) MenuPara.Adj--;
  else MenuPara.Adj = MenuPara.Max;//�ػ���
}

//----------------------��ʾ���º���---------------------------
void MenuAdjAll_Disp(void)
{
  MenuAdj_DispVol(); //�ȸ���ֵ��ʾ
  if(MenuMng.Layer == MENU_LAYER_WR){//д״̬��ȫ������λ����
    MenuMng_cbSetSegFlash(0xff);
    MenuMng_cbSetDotFlash(MenuPara.Cfg2 & 0x0f);//��С����һ����Ϊ����
  }
}

