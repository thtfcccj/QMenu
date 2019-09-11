/***********************************************************************

//		             �˵�ϵͳ֮4λ�����ʱ�Ĺ�����ʵ��

***********************************************************************/

#include "Menu.h"
#include "MenuMng.h"
#include "MenuAdj.h"

#include <string.h>

struct _MenuPara MenuPara;  //�û��ο�ʵ��
struct _MenuMng MenuMng;   //ֱ�Ӿ�̬����


/***********************************************************************
                    ��غ���ʵ��
***********************************************************************/

//-------------------MenuPara��ʼ������--------------------------
static void _MenuParaInit(void)
{
  MenuPara.Adj = 0;      //��ǰ����ֵ
  MenuPara.Min = 0;      //����ֵ����Сֵ,��  
  MenuPara.Max = 9999;   //����ֵ�����ֵ,��
  MenuPara.Cfg = 0;     //��������,ÿ�˵����������,������
  MenuPara.Cfg2 = 0;    //����������,����ҪʱΪ0,������
}


//------------------�ж��Ƿ���ʵʱ��ʾ״̬------------------------
signed char MenuMng_IsRealDisp(void)
{
  if((MenuPara.Cfg & MENU_CFG_REAL_RD) && 
     (MenuMng.Layer == MENU_LAYER_RD)) 
   return -1;
  return 0;
}

//--------------------------������--------------------------
//���˺�������ϵͳ250mS������,���ڸ���ʵʱֵ����ʾ
void MenuMng_Task(void)
{
  if(MenuMng_IsRealDisp()){
    MenuFun[MenuMng_GetId()].GetData(MENU_CFG_REAL_RD); //��ʼʵʱ����
    MenuMng_UpdateDisp(); //������ʾ
  }
}

//----------------------����˵���ʼ������--------------------------
//��������׼������˵�ʱ���ùҽӵĲ˵����ұ�����ڲ���ʼ��
void MenuMng_EnterInit(const unsigned char *pLUT)
{
  memset(&MenuPara, 0, sizeof(struct _MenuPara));
  memset(&MenuMng, 0, sizeof(struct _MenuMng));
  MenuMng.pLUT = pLUT;
}

//-------------------------�õ���ǰ�˵�ID��--------------------
unsigned char MenuMng_GetId(void)
{
  return  *(MenuMng.pLUT + MenuMng.Index + 1);
}

//----------------------���²˵���ʾ����--------------------------
//���ô˺����������½�����ʾ
void MenuMng_UpdateDisp(void)
{
  unsigned char Id, Data,Pos;
  
  //ͳһ��ʼ����ʾ����(��������ܵ���ʾ)
  MenuMng_cbClrSegFlash();
  MenuMng_cbClrDotFlash();
  MenuMng_cbClrDotDisp();  
  
  if(MenuMng.Layer == MENU_LAYER_SEL){//ֱ���ò˵�����Ÿ�����ʾ(��С����)
    Id = MenuMng_GetId();
    //���θ�����ʾ
    for(Pos = 0; Pos < 4; Pos++){
      Data = MenuNote[Id][Pos];
      MenuMng_cbSetSegDisp(Pos, Data); 
      if(Data & MENU_MNG_CB_DOT_MASK) 
        MenuMng_cbSetDotDisp(MenuMng_cbPos2Mask(Pos));
    }
  }
  else{
    //���ݵ�����ʽ�������ģ����ʾ
    switch(MenuPara.Cfg & MENU_CFG_ADJ_MASK){
    case MENU_CFG_ADJ_BIT: 
      MenuAdjBit_Disp(); 
      MenuMng_cbSetDotDisp(MenuPara.Cfg2 & MENU_CFG2_DOT_MASK);//�ֶ�����С����
      break; 
    case MENU_CFG_ADJ_ALL: 
    case MENU_CFG_ADJ_CMD:       
      MenuAdjAll_Disp(); 
      MenuMng_cbSetDotDisp(MenuPara.Cfg2 & MENU_CFG2_DOT_MASK);//�ֶ�����С����
      break;
    //case MENU_CFG_ADJ_LOGIC:
    default:MenuAdjLogic_Disp(); break;
    }
  }
  MenuMng_cbForceFlash(MenuMng.Flag & MENU_MNG_FLASH_HIDDEN); //ǿ�Ƹ���
  MenuMng.Flag &= ~MENU_MNG_FLASH_HIDDEN;
}

/***********************************************************************
                    ����������غ���ʵ��
***********************************************************************/

//---------------------�����д״̬��ʼ������--------------------------
void _EnterRdWr(unsigned char IsWr)
{
  if(IsWr){
    MenuMng.Layer = MENU_LAYER_WR;
  }
  else MenuMng.Layer = MENU_LAYER_RD;
  
  MenuMng.WrIndex = 0;
  MenuMng.OrgAdj = MenuPara.Adj;
  
  //���ݵ�����ʽ��ʼ�����ģ��
  switch(MenuPara.Cfg & MENU_CFG_ADJ_MASK){
    case MENU_CFG_ADJ_BIT: MenuAdjBit_Init(); break;
    case MENU_CFG_ADJ_CMD: MenuMng.OrgAdj++;//������ģʽ��д
    case MENU_CFG_ADJ_ALL: MenuAdjAll_Init(); break;
    //case MENU_CFG_ADJ_LOGIC:
    default:MenuAdjLogic_Init(); break;
  }
}
//---------------------�˳�д״̬������--------------------------
//�β�Ϊд״̬:0:��ֹд,����:���ݸ�".SetData"���β�
void _ExitWr(unsigned char WrState)
{
  //�����˳�ʱ,������δ�ı�,��������˳������һ������
  if((WrState == MENU_LAYER_WR) && (MenuMng.OrgAdj == MenuPara.Adj)){
    if(MenuPara.Cfg2 & MENU_CFG2_QUIT_SAVE) //�˳�ʱҲ����ʱ,�����˳�����
      WrState = MENU_CFG2_QUIT_SAVE;
    else WrState = 0;//�˳�������
  }
  if(WrState != 0){//������ʱ
    unsigned char Id = MenuMng_GetId(); //PIC��Ҫ�Ź���
    MenuFun[Id].SetData(WrState); //д����    
  }  
  if(MenuPara.Cfg2 & MENU_CFG2_QUIT){ //ֱ���˳��˵�
    MenuMng.Layer = MENU_LAYER_NULL;
    return;
  }
  
  MenuFun[MenuMng_GetId()].GetData(MENU_LAYER_SEL);//����һ�����ݻ��д��������
  if(MenuPara.Cfg & MENU_CFG_WR) //ֻдʱֱ������ѡ��״̬
    MenuMng.Layer = MENU_LAYER_SEL;
  else
    _EnterRdWr(0); //�˳�����״̬����ʼ��
}

//---------------------�˵�ѡ��״̬����������--------------------------
void _SelKey(unsigned char Key)
{
  //��Ӧ��������
  if(Key & MENU_MNG_KEY_LONG) return; //����Ӧ������
  
  if(Key == (MENU_MNG_KEY_EXIT | MENU_MNG_KEY_SHORT)){//�̰��˳��˵�
    MenuMng.Layer = MENU_LAYER_NULL;
    return;
  }
  
  if(Key == (MENU_MNG_KEY_ENTER | MENU_MNG_KEY_SHORT)){//�̰�����˵�
    if(MenuPara.Cfg & MENU_CFG_WR){ //ֱ�ӽ���д״̬
      MenuFun[MenuMng_GetId()].GetData(MENU_LAYER_WR);//д����׼��
      _EnterRdWr(1);//����д״̬
    }
    else _EnterRdWr(0);//�����״̬
    return;
  }
  //�˵���Ӽ�
  if(Key & MENU_MNG_KEY_UP){//�˵����
    MenuMng.Index++;
    if(MenuMng.Index >= *MenuMng.pLUT) MenuMng.Index = 0;//�ػ���
  }
  else if(Key & MENU_MNG_KEY_DOWN){//�˵����
    if(!MenuMng.Index) MenuMng.Index = *MenuMng.pLUT - 1;//�ػ���
    else MenuMng.Index--;
  }
  //else return; //�������˳�����ʱ
  
  _MenuParaInit(); //���³�ʼ��
  MenuFun[MenuMng_GetId()].GetData(MenuMng.Layer);//�˵�ѡ��״̬Ԥ������׼��
}

//---------------------�˵�������ʾ(��)״̬����������--------------------------
void _RdKey(unsigned char Key)
{
  /*/ֻ��ʱ,�������,���û��б��溯��,����øú���ִ�б��湦��
  if((Key & (MENU_MNG_KEY_LONG | MENU_MNG_KEY_ENTER)) ==
     (MENU_MNG_KEY_LONG | MENU_MNG_KEY_ENTER)){
    if(MenuPara.Cfg & MENU_CFG_RD){//ֻ��ʱ�������   
      if(MenuFun[MenuMng_GetId()].SetData != NULL){
        MenuFun[MenuMng_GetId()].SetData(MENU_CFG_WR);//д����
        MenuFun[MenuMng_GetId()].GetData(MENU_LAYER_SEL);//����һ�����ݻ��д���
        MenuMng.Layer = MENU_LAYER_SEL;//�˵��ϲ�˵�
      }
    }
    return;
  }*/ //���� MENU_CFG_ADJ_CMD���ܴ���
  //����Ӧ�����뱣�ְ���
  if(Key & (MENU_MNG_KEY_LONG | MENU_MNG_KEY_KEEP)) return;

  if(Key & MENU_MNG_KEY_ENTER){//�̰�ȷ�ϼ�����д״̬
    if(!(MenuPara.Cfg & MENU_CFG_RD)){//��ֻ��ʱ�������
      MenuFun[MenuMng_GetId()].GetData(MENU_LAYER_WR);//д����׼��
      _EnterRdWr(1);//����д״̬
    }
  }
  else if(Key & MENU_MNG_KEY_EXIT){//�̰��˳����˳���ʾ״̬
    MenuMng.Layer = MENU_LAYER_SEL;
  }
  else{ //���¼�
    //���߼�ʱ�����¼�����Ϊ��ҳ��ʾ
    if((MenuPara.Cfg & MENU_CFG_ADJ_MASK) >= MENU_CFG_ADJ_LOGIC){
      MenuAdjLogic_RdPageKey();
    }
  }
}

//---------------------�˵���������(д)״̬����������-------------------------
void _WrKey(unsigned char Key)
{
  if(Key & MENU_MNG_KEY_LONG){//������
    if(Key & MENU_MNG_KEY_ENTER) _ExitWr(MENU_LAYER_WR);//����ȷ�ϼ����沢�˳�
    return;
  }
  if(Key & MENU_MNG_KEY_EXIT){//�̰��˳��˵�
    if(Key & MENU_MNG_KEY_SHORT){
      if(MenuPara.Cfg2 & MENU_CFG2_QUIT_SAVE) //�˳�ʱҲ����
        _ExitWr(MENU_CFG2_QUIT_SAVE);
      else _ExitWr(0);//�˳�ʱ������
    }
    return;
  }
  
  //�����������ݵ�����ʽ����
  switch(MenuPara.Cfg & MENU_CFG_ADJ_MASK){
    case MENU_CFG_ADJ_BIT: MenuAdjBit_Key(Key); break;
    case MENU_CFG_ADJ_ALL: MenuAdjAll_Key(Key); break;
    case MENU_CFG_ADJ_LOGIC: MenuAdjLogic_Key(Key); break;
    //case MENU_CFG_ADJ_CMD:      //���Ұ�����Ч
    default: break;
  }
  if(MenuPara.Cfg & MENU_CFG_REAL_WR){//ʵʱдʱ,���ݱ仯д����
    if(Key & (MENU_MNG_KEY_UP | MENU_MNG_KEY_DOWN)){
      unsigned char Id = MenuMng_GetId(); //PIC��Ҫ�Ź���
      MenuFun[Id].SetData(MENU_CFG_REAL_WR); //ʵʱд����
    }
  }
}

//--------------------------����������--------------------------
//���˺�������ϵͳ���ٽ�����,���ڴ����ֵ
void MenuMng_FastTask(void)
{
  unsigned char Key = MenuMng.Key;
  if(!Key) return; //�޼�ֵ
  MenuMng.Key = 0;
  
  unsigned char PrvLayer = MenuMng.Layer;
  switch(MenuMng.Layer){
  case MENU_LAYER_NULL: //���ڲ˵�ϵͳ
    if(Key != MENU_MNG_KEY_ENTER) break; //
    MenuMng.Layer++; //����
    Key = 0;//�������´���
  case MENU_LAYER_SEL: //�˵�ϵͳѡ��:����Ӧ������
    _SelKey(Key);
    break;
  case MENU_LAYER_RD: //�˵���ʾ����״̬
   _RdKey(Key);
    break;
  case MENU_LAYER_WR: //�˵���������״̬
   _WrKey(Key);
    break;
  default: //�쳣�˳�
    MenuMng.Layer = MENU_LAYER_NULL;
    break;
  }
  if(MenuMng.Layer != MENU_LAYER_NULL) MenuMng_UpdateDisp();//������ʾ
  else if(PrvLayer != MENU_LAYER_NULL){//�˳��˵�ϵͳ
    MenuMng_cbExitNotify(); //����˳��˵�ϵͳͨ��
  }
}

//----------------------�˳��˵�����--------------------------
//�ڲ˵�״̬,�ⲿǿ���˳��˵�ʱ���ô˺���
void MenuMng_Exit(void)
{
  if(MenuMng.Layer == MENU_LAYER_NULL) return; 
  
  if(MenuPara.Cfg2 & MENU_CFG2_QUIT_SAVE){ //�˳�ʱҲ����
    MenuPara.Cfg2 |= MENU_CFG2_QUIT;//����ǰ��ʱ��ǿ���˳�״̬
    _ExitWr(MENU_CFG2_QUIT_SAVE);
  }
  MenuMng_cbExitNotify(); //����˳��˵�ϵͳͨ��
}  
