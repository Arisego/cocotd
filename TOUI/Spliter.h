#ifndef __SPLITER_H__
#define __SPLITER_H__

/*
	<��HUD���зֲ���� - ��002
	<	- ����ʱ��
	<	- �ֻ��л�
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
	bool init();	// <���г�ʼ�����빤��
	virtual void update(float delta);

	TSClock* mTSClock;
	int miUpdateCount;
	bool mbOpened;		// <��״̬�����Spliter�㴦�ڹر�״̬���ܾ�һ�нӿڵ���

	TSRound* mTSRound;

public:
	bool mbShowed;		// <Roundֻ��ʾһ��

	void f_open();
	void f_pause();

	void ShowClock();
	void DisClock();

	void ShowRound();
	void RoundOver();
};


#endif