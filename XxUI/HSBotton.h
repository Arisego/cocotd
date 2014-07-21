#ifndef __HS_BUTTON_H__
#define __HS_BUTTON_H__

#include "packui\Container.h"
/*
	<HS_Button
	| < ֻ��Hover��Select��button
*/

class HSButton : public Container
{
private:
	bool	mbEmerge;	// <�Ƿ��������ûص�

public:
	HSButton(const char* aHover, const char* aSelect, int aw, int ah, bool aem = true);

	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) override;
};

#endif