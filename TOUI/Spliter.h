#ifndef __SPLITER_H__
#define __SPLITER_H__

/*
	<对HUD进行分层管理 - 层002
	<	- 天相时钟
	<	- 轮回切换
*/

#include "cocos2d.h"
#include "TSClock.h"
#include "TSRound.h"

USING_NS_CC;

class Spliter : public CCLayer
{
public:
	Spliter();
	~Spliter();

	/*static */Spliter *sharedSpliter();
	//static void purgeSharedSpliter();

private:
	/*static */Spliter* mSharedSpliter;
	bool init();	// <进行初始化载入工作
	virtual void update(float delta);

	TSClock* mTSClock;
	int miUpdateCount;
	bool mbOpened;		// <锁状态，如果Spliter层处于关闭状态，拒绝一切接口调用

	TSRound* mTSRound;

public:
	bool mbShowed;		// <Round只显示一次

	void f_open();
	void f_pause();

	void ShowClock();
	void DisClock();

	void ShowRound();
	void RoundOver();
};


#endif