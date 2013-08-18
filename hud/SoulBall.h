#ifndef __SOUL_BALL_H__
#define __SOUL_BALL_H__

#include "cocos2d.h"

USING_NS_CC;

class SoulBall : public CCLayer
{
public:
	virtual bool init();
	void setSum(int ai);	// <设置总值
	void setAli(int ai);	// <设置活跃个数

	void GenerateSB();
	CREATE_FUNC(SoulBall);

private:
	int miSSum;
	std::vector<CCSprite*> mics_SBList;

	int miALive;
};

#endif