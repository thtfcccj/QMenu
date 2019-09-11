/***********************************************************************

//		             �˵�ϵͳ֮4λ�����ʱ�Ĺ�����ʵ��

***********************************************************************/
#ifndef _MENU_MNG_H
#define	_MENU_MNG_H

/***********************************************************************
                            ˵��
***********************************************************************/
//�˵���������:
//��һ��Ϊ�˵�ѡ��,��ʾһ������ַ���ʾ,��ʱ��ѡ�������˳�,
//�ڶ���Ϊ������ʾ,��һ���Զ�����ʾֵ,Ҳ��ʵʱ������ʾֵ
//������Ϊ��������:���ڶ����ֻ��ʱ,��ȷ�ϼ�����
//�ڶ�,���������������п���
//�˵�ϵͳ����Ӧ����,�˳�,��,�¼��Ķ̰�,����,����ʱ��Ӧ
/***********************************************************************
                    �������
***********************************************************************/


/***********************************************************************
                    ��ؽṹ-�˵�������
//���û�����������Menu.h�ﶨ��
***********************************************************************/


//���ڲ˵�ϵͳ���й���,�û���ֱֹ�ӷ���
struct _MenuMng{
  //��ǰ������,������0:���ڲ˵�,������1,2,3�˵���
  unsigned char Layer;
  //��ǰ���ڵڼ����˵�λ��
  unsigned char Index;
  //���������ֵ,���ڼ���ϵͳǶ�׵���
  unsigned char Key;
  //��ر�־,������
  unsigned char Flag;
  
  //��������ʱ,�������ڲ�����:
  unsigned char WrIndex;//������ֵʱ��λ��
  unsigned char WrMaxPos;//������ֵʱ�����Χ,��
  signed short OrgAdj;  //�������ʱ�ĵ�����ԭʼֵ
  
   //�û��ҽӵĲ˵����ұ�,��λΪ���ұ��С. ��ʹ�ô˻���ʵ��Ȩ�޷���
  const unsigned char *pLUT;   
};

//����,�˵���μ�Menu�еĲ�ζ���:

//��ر�־����Ϊ:
//����λǿ��������ʾ��־,���򰴼���ʹ����λǿ����ʾ,�˱�־�ڵ���DISP���Զ�ȡ��
#define MENU_MNG_FLASH_HIDDEN   0x01  

extern struct _MenuMng MenuMng; //ֱ�Ӿ�̬����

/***********************************************************************
                        ��غ���
***********************************************************************/

//--------------------------������--------------------------
//���˺�������ϵͳ250mS������,���ڸ���ʵʱֵ����ʾ
void MenuMng_Task(void);

//----------------------���²˵���ʾ����--------------------------
//���ô˺����������½�����ʾ
void MenuMng_UpdateDisp(void);

//----------------------����˵���ʼ������--------------------------
//��������׼������˵�ʱ���ùҽӵĲ˵����ұ�����ڲ���ʼ��
//��pLUT != NULL �������MenuMng_Key(MENU_MNG_KEY_ENTER)��ʽ����
void MenuMng_EnterInit(const unsigned char *pLUT);

//----------------------�˳��˵�����--------------------------
//�ڲ˵�״̬,�ⲿǿ���˳��˵�ʱ���ô˺���
void MenuMng_Exit(void);
                  
//---------------------��ʼ������----------------------------
//����ʱ����
//void MenuMng_Init(void);
#include <string.h>
#define MenuMng_Init() do{MenuMng_EnterInit(NULL);}while(0)

//----------------------�Ӽ�������--------------------------
//�β�Key��λ����Ϊ:
#define MENU_MNG_KEY_MASK   0x0f  //��ֵ��������
#define MENU_MNG_KEY_MODE   0x70  //������ʽ����
#define MENU_MNG_KEY_UP     0x01  //�ϼ�
#define MENU_MNG_KEY_DOWN   0x02  //�¼�
#define MENU_MNG_KEY_ENTER  0x04  //�����
#define MENU_MNG_KEY_EXIT   0x08  //�˳���
#define MENU_MNG_KEY_SHORT  0x10  //�̰���
#define MENU_MNG_KEY_LONG   0x20  //������
#define MENU_MNG_KEY_KEEP   0x40  //���ְ���(��ס����)
//void MenuMng_Key(unsigned char Key);
#define MenuMng_Key(key) do{MenuMng.Key = key;}while(0)

//--------------------------����������--------------------------
//���˺�������ϵͳ���ٽ�����,���ڴ����ֵ
void MenuMng_FastTask(void);

//-------------------------�õ���ǰ�˵�ID��--------------------
unsigned char MenuMng_GetId(void);

//-------------------------�õ�����ǰ��ֵ-----------------------
//signed short MenuMng_GetOrgAdj(void);
#define MenuMng_GetOrgAdj() (MenuMng.OrgAdj)

//-------------------------�õ���ǰ����λ------------------------
//unsigned char MenuMng_GetWrIndex(void);
#define MenuMng_GetWrIndex() (MenuMng.WrIndex)

//------------------�ж��Ƿ���ʵʱ��ʾ״̬------------------------
signed char MenuMng_IsRealDisp(void);

/*********************************************************************
                    ��Դ����ص����������趨������
*********************************************************************/
//��ģ����ʾ���ֶ�����ϵͳ,��Ҫ4��"8.8.8.8"�Ķ�����ʾ,�û���ʵ���нӿ�
//��8.8.8.8���ⲿ��������:
//1.ÿ��8�ж�������ʾ����,��0~3��ʾ(˳���ָ��)
//2.ÿ��8�ж�������������,������������������,��λ��������Ӧ�þ���
//3.ÿ��С�������ʾ�������ɶ�������,��������������

#include "Led.h"
#include "LedSign.h"
//---------------------�ɵ���λ�õõ���ʾλ������----------------------
//MenuMng�鶨���Ҳ�λ��ΪPos0,ͨ����ת��Ϊ��Ӧ����ܻ�С����λ��
//unsigned char MenuMng_cbPos2Mask(unsigned char Pos);
#define MenuMng_cbPos2Mask(pos) (1 << pos)

//---------------------��ʮ������ֵ�õ�������ʾ----------------------
//unsigned char MenuMng_cbVol2Seg(unsigned char Vol);
#define MenuMng_cbVol2Seg(vol) LED_SIGN_NumLut[vol]

//---------------------�õ����ŵĶ����ʾ----------------------
//unsigned char MenuMng_cbGetNegaSeg(void);
#define MenuMng_cbGetNegaSeg() (LED_SIGN__)

//------------------------����ĳ����ʾ��������ʾ------------------------
//void MenuMng_cbSetSegDisp(unsigned char Pos, unsigned char Seg);
#define MenuMng_cbSetSegDisp(pos, seg) do{Led.SegDisp[pos] = seg;}while(0)

//------------------------ʹ�����Ӧ�����������------------------------
//void MenuMng_cbSetSegFlash(unsigned char Mask);
#define MenuMng_cbSetSegFlash(mask)  do{Led.SegFlash |= mask;}while(0)

//----------------------------ʹ���������ͣ��--------------------------
//void MenuMng_cbClrSegFlash(void);
#define MenuMng_cbClrSegFlash()  do{Led.SegFlash &= ~0x0f;}while(0)

//------------------------ʹ�����Ӧ��С������ʾ------------------------
//void MenuMng_cbSetDotDisp(unsigned char Mask);
#define MenuMng_cbSetDotDisp(mask)  do{Led.DotDisp |= mask;}while(0)

//------------------------ʹ����С����ͣ��------------------------
//void MenuMng_cbClrDotDisp(void);
#define MenuMng_cbClrDotDisp()  do{Led.DotDisp &= ~0x0f;}while(0)

//------------------------ʹ�����Ӧ��С��������------------------------
//void MenuMng_cbSetDotFlash(unsigned char Mask);
#define MenuMng_cbSetDotFlash(mask)  do{Led.DotFlash |= mask;}while(0)

//----------------------------ʹ����С����ͣ��----------------------------
//void MenuMng_cbClrDotFlash(void);
#define MenuMng_cbClrDotFlash()  do{Led.DotFlash &= ~0x0f;}while(0)

//------------------------------ǿ�Ƹ����Լ���������-----------------------
//����ǿ�Ʊ�־,ͬʱˢ����ʾ��
//�β�Ϊ�Ƿ�ǿ������������
//void MenuMng_cbForceFlash(unsigned char IsForceHidden);
#define MenuMng_cbForceFlash(isforcehidden)\
  do{if(isforcehidden) Led.Flag |= (LED_FLASH_NOW | LED_FLASH_DIS);\
     else Led.Flag |= (LED_FLASH_NOW); Led_Task();}while(0)

//--------------������MenuNote�ж����С����λ������----------------------
#define MENU_MNG_CB_DOT_MASK   LED_SIGN_DOT

/***********************************************************************
                    �ص�����
***********************************************************************/

//---------------------------�˳��˵�ϵͳͨ��-----------------------
void MenuMng_cbExitNotify(void);


#endif //_MENU_MNG_H
