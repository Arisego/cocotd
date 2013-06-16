#ifndef __TO_TAB_ORDER_H__
#define __TO_TAB_ORDER_H__

/* <�����ӿڣ���Ҫ�����ͱ�־λ */
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
	<���̲����Ŀؼ�˳���ϵ����
	<��Ϊ�ӿ�
	<1  2  3  4
	<�� �� �� ��
	<���ڽ��渴��ʱʹ��
*/
class TOTabOrder
{
public:
	TOTabOrder* tab_up;
	TOTabOrder* tab_down;
	TOTabOrder* tab_left;
	TOTabOrder* tab_right;

	/* <��õ�һ������Disable�ı��� */
	virtual TOTabOrder* getcurrent(int aorder) = 0;
};

#endif