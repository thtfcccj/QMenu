/***********************************************************************
		             �˵�ϵͳ֮4λ�����-�߼�������ģʽʵ��
//���λ�߼�ֵ,λ�ڸ�λ��ʾ
//WrIndexλ��ֵ�ֱ�Ϊ:3,2,1,0, 7,6,5,4
//�Ծ͵���λ�ֱ�Ϊ:   0,1,2,3, 4,5,6,7
//�߼����Ŀɵ���λ��Maxλ����,��1ʱ��ʾ�ɵ���,����ֻ��
//��ģ��ΪMenuMng���ڲ�ʵ��,�Ƕ���ʹ��ģ��
***********************************************************************/

#include "Menu.h"
#include "MenuMng.h"
#include "MenuAdj.h"

//-----------------------�õ��¸��ɵ���λ---------------------------
//>= 8��δ�ҵ�
static unsigned char _GetNextWrIndex(unsigned char NextIndex)
{
  for(; NextIndex < 8; NextIndex++)
    if((1 << NextIndex) & MenuPara.Max) break;
  return NextIndex;
}

//-----------------------��ʼ������---------------------------
void MenuAdjLogic_Init(void)
{
  MenuMng.WrIndex = _GetNextWrIndex(0);
}

//-------------------------����������---------------------------
void MenuAdjLogic_Key(unsigned char Key)
{
  unsigned char Data;
  
  //ȷ�ϼ��л�����λ, MenuMng.WrIndex��ʾ��ǰ��������ֵλ
  if(Key & MENU_MNG_KEY_ENTER){
    Data = _GetNextWrIndex(MenuMng.WrIndex + 1);
    if(Data >= 8) Data = _GetNextWrIndex(0); //�ػ���
    MenuMng.WrIndex = Data;
    
    MenuMng.Flag |= MENU_MNG_FLASH_HIDDEN; //�л�ʱ���л�λ����
    return;
  }
  
  //���¼��л��߼�
  Data = 1 << MenuMng.WrIndex;
  if(MenuPara.Adj & Data) MenuPara.Adj &= ~Data;
  else MenuPara.Adj |= Data;
}

//-----------------������ʱ�����¼���ҳ������---------------------------
void MenuAdjLogic_RdPageKey(void)
{
  unsigned char Data = _GetNextWrIndex(MenuMng.WrIndex + 4);
  if(Data >= 8) Data = _GetNextWrIndex(0); //�ػ���
  MenuMng.WrIndex = Data;
}

//----------------------��ʾ���º���---------------------------
void MenuAdjLogic_Disp(void)
{
  unsigned char Pos,IsHi;
  unsigned char Data = MenuPara.Adj;
  
  if(MenuMng.WrIndex >= 4) Data >>= 4;//��ʾ��λֵ������
  
  //����С���������
  for(Pos = 0; Pos < 4; Pos++){ //�ж���ֵ
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
  MenuMng_cbSetDotDisp(IsHi); //��С�����ʾ�ڵ�2ҳ
  
  if(MenuMng.Layer == MENU_LAYER_WR)//д״̬�õ���λ����
    MenuMng_cbSetSegFlash(MenuMng_cbPos2Mask(3 - Pos));
}

