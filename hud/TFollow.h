#ifndef __T_FOLLOW_H__
#define __T_FOLLOW_H__

#include "cocos2d.h"

USING_NS_CC;

class TFollow : public CCFollow
{
public:
	static TFollow* TFollow::create(CCNode *pFollowedNode, const CCRect& rect/* = CCRectZero*/);
	virtual void step(float dt);
};

#endif