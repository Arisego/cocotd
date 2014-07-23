#ifndef __FL_BUTTON_H__
#define __FL_BUTTON_H__

#include "HSBotton.h"
#include "json/json.h"
/*
	<HS_Button
	| < 只有Hover和Select的button
*/

class FLButton : public HSButton
{
private:
	bool	mbEmerge;	// <是否立即调用回调

public:
	Json::Value		mJsData;	/* <档案摘要 */

public:
	FLButton(const char* aHover, const char* aSelect, int aw, int ah, bool aem = true);

	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) override;
	virtual void update(float fDelta) override;
};

#endif