#ifndef __ACTIVATOR_H__
#define __ACTIVATOR_H__

#include "cocos2d.h"
USING_NS_CC;

/*
	Activator
	<��Яʽ�ص��ӿ�
*/
class Activator
{
public:
	void setactivator(CCObject* target, SEL_MenuHandler selector){
		m_pListener		 =	target;
		m_pfnSelector	 =	selector;
	};


protected:
	void activate(CCObject* pSender){
		if (m_pListener && m_pfnSelector)
		{
			(m_pListener->*m_pfnSelector)(pSender);
		}
	};	
	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;
};



#endif