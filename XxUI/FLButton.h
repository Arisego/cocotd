#ifndef __FL_BUTTON_H__
#define __FL_BUTTON_H__

#include "HSBotton.h"
#include "json/json.h"
/*
	<HS_Button
	| < ֻ��Hover��Select��button
*/

class FLButton : public HSButton
{
private:
	bool	mbEmerge;	// <�Ƿ��������ûص�

public:
	Json::Value		mJsData;	/* <����ժҪ */

public:
	FLButton(const char* aHover, const char* aSelect, int aw, int ah, bool aem = true);

	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) override;
	virtual void update(float fDelta) override;
};

#endif