#ifndef __TO_TAB_ORDER_H__
#define __TO_TAB_ORDER_H__

/* <操作接口，需要类名和标志位 */
#define TO_TAB_GETCU(classname,bflag)							\
	virtual TOTabOrder* classname::getcurrent(int aorder)		\
	{															\
		if(!bflag){												\
			TOTabOrder* ret;									\
			switch (aorder)										\
			{													\
			case 1:ret = tab_left;break;						\
			case 2:ret = tab_right;break;						\
			case 3:ret = tab_up;break;							\
			case 4:ret = tab_down;break;						\
			default:ret = NULL;break;							\
			}													\
																\
			if(!ret) ret = ret->getcurrent(aorder);				\
			return ret;											\
		}else{													\
			return this;										\
		}														\
	}
/*
	TOTabOrder
	<键盘操作的控件顺序关系控制
	<作为接口
	<1  2  3  4
	<左 右 上 下
	<仅在界面复杂时使用
*/
class TOTabOrder
{
public:
	TOTabOrder* tab_up;
	TOTabOrder* tab_down;
	TOTabOrder* tab_left;
	TOTabOrder* tab_right;

	/* <获得第一个不是Disable的变量 */
	virtual TOTabOrder* getcurrent(int aorder) = 0;
};

#endif