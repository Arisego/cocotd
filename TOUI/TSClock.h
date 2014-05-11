#ifndef __TS_CLOCK_H__
#define __TS_CLOCK_H__

/*
	<TSClock
	<	��������й������ʾ��ʱ����
*/

#include "cocos2d.h"
#include "utils\ColorSprite.h"
USING_NS_CC;

class TSClock
{
public:
	virtual bool init();
	TSClock(CCNode* aParent);

	void tick();				// <��תʱ����Χ��ʾ
	void rotate();				// <��תʱ�ӱ���ת������
	
private:
	CCNode* mcParent;
	////////////////////////////////////////////////////////////////////////
	//	<ʱ����ʾȦ
	
	ColorSprite* m_csClock;		// <ʱ����Ȧ
	ColorSprite* m_csCLockB;		// <ʱ�ӵײ�Ȧ
	int m_iStep;				// <��Χ��ת��
	bool m_bRotate;
	
	////////////////////////////////////////////////////////////////////////
	// <UI�����δ����ʱ�Ӳ�ʵ��

public:
	//////////////////////////////////////////////////////////////////////////
	// <���ýӿ�

	void Dismiss();		// <ȡ����ʾ
	void Showup();		// <��ʾ
};


#endif