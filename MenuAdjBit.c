/***********************************************************************
		             �˵�ϵͳ֮4λ�����-��λ����ʵ��
//��ģ��ΪMenuMng���ڲ�ʵ��,�Ƕ���ʹ��ģ��
***********************************************************************/

#include "Menu.h"
#include "MenuMng.h"
#include "MenuAdj.h"
#include "MenuMath.h"

//----------------�õ�ָ��λ����ֵ����------------------------
unsigned char _GetDigitalInPos(unsigned short Value,
                               unsigned char Pos)
{
  while(Pos--){
    Value /= 10;
  }
  return Value % 10;
}

//-----------------------��ʼ������---------------------------
void MenuAdjBit_Init(void)
{
  MenuAdj_UpdateMaxPos();
  MenuMng.WrIndex = MenuMng.WrMaxPos; //��������λ
}

//�ɵ�����õ�����
static __flash const unsigned short _Bit2Muti[4] = {1,10,100,1000};

//-------------------------����������---------------------------
void MenuAdjBit_Key(unsigned char Key)
{
  unsigned short Value, Muti;
  unsigned char Vol;
  
  //ȷ�ϼ��ڵ���λ���л�
  if(Key & MENU_MNG_KEY_ENTER){
    if(MenuMng.WrIndex) MenuMng.WrIndex--;
    else MenuMng.WrIndex = MenuMng.WrMaxPos; //�ػ������λ��
    MenuMng.Flag |= MENU_MNG_FLASH_HIDDEN; //�л�ʱ���л�λ����
    return;
  }
  //���¼�����λ����
  if((MenuPara.Cfg2 & MENU_CFG2_NEGATIVE) && (MenuMng.WrIndex == MenuMng.WrMaxPos)){
    MenuPara.Adj = 0 - MenuPara.Adj; //��ת
    return;
  }
  //���¼�����
  Value = MenuMath_Ss2Su(MenuPara.Adj);
  Muti = _Bit2Muti[MenuMng.WrIndex];//������
  Vol = _GetDigitalInPos(Value,MenuMng.WrIndex);  //�õ�ָ��λ��ֵ
  Value -= Muti * Vol; //ȥ����λ��
  if(Key & MENU_MNG_KEY_UP){
    if(Vol < 9) Vol++;
    else Vol = 0;
  }
  else{
    if(Vol) Vol--;
    else Vol =  9;
  }
  Value += Vol * Muti; //�µĸ�λ��ֵ
  
  //�趨����ֵ
  if(MenuPara.Adj < 0) MenuAdj_SetValue(0 - Value);
  else MenuAdj_SetValue(Value);
}

//----------------------��ʾ���º���---------------------------
void MenuAdjBit_Disp(void)
{
  MenuAdj_DispVol(); //�ȸ���ֵ��ʾ
  if(MenuMng.Layer == MENU_LAYER_WR){//д״̬�����ڵ���λ����
    MenuMng_cbSetSegFlash(MenuMng_cbPos2Mask(MenuMng.WrIndex));
  }
}

