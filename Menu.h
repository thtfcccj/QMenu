/***********************************************************************

		             菜单系统之4位数码管时的用户交互及实现接口

***********************************************************************/
#ifndef _MENU_H
#define	_MENU_H

//菜单的用户接口由参数与函数接口共同组成:


/***********************************************************************
		             与用户交互的参数接口
***********************************************************************/
struct _MenuPara{
  signed short Adj;      //当前调整值
  signed short Min;      //调整值的最小值,含  
  signed short Max;      //调整值的最大值,含
  unsigned char Cfg;     //主配置字,每菜单项均需配置,见定义
  unsigned char Cfg2;    //附加配置字,不需要时为0,见定义
};

//主配置字定义为:
#define MENU_CFG_RD        0x80    //菜单为只读状态,即不能过入第三层
#define MENU_CFG_WR        0x40    //菜单为只写状态,即直接进入第三层调整
#define MENU_CFG_REAL_RD   0x20    //菜单显示时需实时读数刷新
#define MENU_CFG_REAL_WR   0x10    //菜单调整时需实时写刷新,在数值改变时即更新]
//注:只读时允许SetData函数为空.若不为空,在用户长按确认键后将调用此函数后退出

#define MENU_CFG_ADJ_MASK   0x03  //菜单项的调整方式:
#define MENU_CFG_ADJ_BIT    0x00  //按位调节模式
#define MENU_CFG_ADJ_ALL    0x01  //整体调节模式(最大最小值与当前值相同时不调整数值)
#define MENU_CFG_ADJ_LOGIC  0x02  //逻辑数调节模式
#define MENU_CFG_ADJ_CMD    0x03  //命令模式,仅允许按确认键

//附加配置字定义为:
#define MENU_CFG2_QUIT       0x80    //调整完成后退出菜单标志
#define MENU_CFG2_QUIT_SAVE  0x40    //响应退出时保存,可于不重要参数的保存,或恢复现场
#define	MENU_CFG2_NEGATIVE   0x20    //调整项为负数显示或调整,即可显示负值

#define	MENU_CFG2_DOT_MASK   0x0f    //最低4位用于控制显示及调整时的小数点位显示
//#define	MENU_CFG2_DOT0       0x01    //从右向左:第1个小数点显示
#define	MENU_CFG2_DOT1       0x02    //从右向左:第2个数码管显示
#define	MENU_CFG2_DOT2       0x04    //从右向左:第3个数码管显示
#define	MENU_CFG2_DOT3       0x08    //从右向左:第4个数码管显示

extern struct _MenuPara MenuPara;  //直接实现,用户可直接使用

/***********************************************************************
		             与用户交互的函数接口
***********************************************************************/

//表述菜单层次的定义:
#define MENU_LAYER_NULL   0 //不在菜单中
#define MENU_LAYER_SEL    1 //在选择菜单状态
#define MENU_LAYER_RD     2 //在查看(读)状态
#define MENU_LAYER_WR     3 //在修改(写)状态

struct _MenuFun{
  //读取参数至MenuPara,形参:
  //MENU_CFG_REAL_RD: 为实时读取状态位置,实时读打开时调用
  //MENU_CFG_REAL_WR: 为通讯层调用,写缓冲区数据
  //其它定义为菜单层次,表示进入该层菜单时调用,可根据情况进行MenuPara初始化:
  void (*GetData)(unsigned char Type);
  //MenuPara.Adj完成,保存数据,形参:
  //MENU_CFG_REAL_WR: 为实时写状态位置,实时写打开且数值变化时调用
  //MENU_CFG2_QUIT_SAVE: 为退出时保存,退出保存功能打开,在退出强制调用(不管值是否变化)
  //MENU_LAYER_WR,表示退出写状态时调用,保存键且数值变化时正常写数据
  void (*SetData)(unsigned char Type);
};

extern __flash const struct _MenuFun MenuFun[];  //直接声明并由菜单管理器使用,由用户实现

/***********************************************************************
		                      菜单选择里的提示项(含小数点)
***********************************************************************/

extern const __flash unsigned char MenuNote[][4];

#endif
