#ifndef __T_FOLLOW_H__
#define __T_FOLLOW_H__

#include "cocos2d.h"

USING_NS_CC;

class TFollow : public CCFollow
{
public:
	bool	mbLock;
	int		miID;

	static TFollow* TFollow::create(CCNode *pFollowedNode, const CCRect& rect/* = CCRectZero*/, bool alock = false);
	virtual void step(float dt);

	void FactMove();		// <Ö±½ÓÒÆ¶¯
private:
	virtual bool init();
};

#endif