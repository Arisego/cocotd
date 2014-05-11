#ifndef __TS_CLOCK_H__
#define __TS_CLOCK_H__

/*
	<TSClock
	<	对天相进行管理和显示的时钟类
*/

#include "cocos2d.h"
#include "utils\ColorSprite.h"
USING_NS_CC;

class TSClock
{
public:
	virtual bool init();
	TSClock(CCNode* aParent);

	void tick();				// <旋转时钟外围显示
	void rotate();				// <旋转时钟本身，转换天相
	
private:
	CCNode* mcParent;
	////////////////////////////////////////////////////////////////////////
	//	<时钟显示圈
	
	ColorSprite* m_csClock;		// <时钟外圈
	ColorSprite* m_csCLockB;		// <时钟底部圈
	int m_iStep;				// <外围旋转类
	bool m_bRotate;
	
	////////////////////////////////////////////////////////////////////////
	// <UI和设计未定，时钟不实现

public:
	//////////////////////////////////////////////////////////////////////////
	// <调用接口

	void Dismiss();		// <取消显示
	void Showup();		// <显示
};


#endif