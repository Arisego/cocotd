#ifndef __TS_ROUND_H__
#define __TS_ROUND_H__

#include "cocos2d.h"
#include "utils\ColorSprite.h"
USING_NS_CC;

class TSRound : CCObject
{
public:
	TSRound(CCNode* aParent);
	void Finish();

	void show();
	virtual bool init();
private:
	CCNode* mParent;

	ColorSprite* mcsText;
	ColorSprite *mcsFrame;

};

#endif