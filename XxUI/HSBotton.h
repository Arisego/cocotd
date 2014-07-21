#ifndef __HS_BUTTON_H__
#define __HS_BUTTON_H__

#include "packui\Container.h"
/*
	<HS_Button
	| < 只有Hover和Select的button
*/

class HSButton : public Container
{
private:
	bool	mbEmerge;	// <是否立即调用回调

public:
	HSButton(const char* aHover, const char* aSelect, int aw, int ah, bool aem = true);

	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) override;
};

#endif