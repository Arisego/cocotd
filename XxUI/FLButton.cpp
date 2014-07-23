#include "FLButton.h"
#include "XxTitle\FLLayer.h"

FLButton::FLButton(const char* aHover, const char* aSelect,int aw, int ah, bool aem)
	:HSButton(aHover,aSelect,aw,ah,aem)
{
	;
}

void FLButton::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	if(!isVisible()) return;
	if(checkTouch(pTouch)){
		onSelect();
		if(mbEmerge) activate();
		else runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0), CCCallFunc::create(this, callfunc_selector(Container::activate))));	

	}
	return;
}

void FLButton::update(float fDelta)
{
	if(!m_bIsEnabled) return;
	if(mLasto && m_iState != C_STATE_SELECT){
		if(checkTouch(mLasto)){
			onHover();
			((FlLayer*) m_pListener)->SetActiveBtn(this);
		}else{
			onNormal();
			CC_SAFE_RELEASE_NULL(mLasto);
			((FlLayer*) m_pListener)->UnSetActive(this);
		}
	}
}

