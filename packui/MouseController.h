/*
|	ControllerÀàÖØÔØ
|
|
*/
#ifndef __MOUSE_CONTROLLER_H__
#define __MOUSE_CONTROLLER_H__

#include "byeven/BYCocos.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;

class mControlStepper : public CCControlStepper
{
	bool m_bIsMouseDown;

	virtual void onEnter(){
		m_bIsMouseDown = false;
		CCControlStepper::onEnter();
	}

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
		m_bIsMouseDown = CCControlStepper::ccTouchBegan(pTouch,pEvent);
		return m_bIsMouseDown;
	}

	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent){
		if(m_bIsMouseDown)
			CCControlStepper::ccTouchMoved(pTouch,pEvent);
	}

	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent){
		m_bIsMouseDown = false;
		CCControlStepper::ccTouchEnded(pTouch,pEvent);
	}

	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent){
		m_bIsMouseDown = false;
		CCControlStepper::ccTouchCancelled(pTouch,pEvent);
	}
};

class mControlSwith : public CCControlSwitch
{
	bool m_bIsMouseDown;

	virtual void onEnter(){
		m_bIsMouseDown = false;
		CCControlSwitch::onEnter();
	}

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
		m_bIsMouseDown = CCControlSwitch::ccTouchBegan(pTouch,pEvent);
		return m_bIsMouseDown;
	}

	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent){
		if(m_bIsMouseDown)
			CCControlSwitch::ccTouchMoved(pTouch,pEvent);
	}

	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent){
		m_bIsMouseDown = false;
		CCControlSwitch::ccTouchEnded(pTouch,pEvent);
	}

	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent){
		m_bIsMouseDown = false;
		CCControlSwitch::ccTouchCancelled(pTouch,pEvent);
	}


};

class mPotentionMeter : public CCControlPotentiometer
{
	bool m_bIsMouseDown;

	virtual void onEnter(){
		m_bIsMouseDown = false;
		CCControlPotentiometer::onEnter();
	}

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
		m_bIsMouseDown = CCControlPotentiometer::ccTouchBegan(pTouch,pEvent);
		return m_bIsMouseDown;
	}

	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent){
		if(m_bIsMouseDown)
			CCControlPotentiometer::ccTouchMoved(pTouch,pEvent);
	}

	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent){
		m_bIsMouseDown = false;
		getThumbSprite()->setColor(ccWHITE);
		setSelected(false);
		sendActionsForControlEvents(CCControlEventValueChanged);   
	}

	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent){
		m_bIsMouseDown = false;
		CCControlPotentiometer::ccTouchCancelled(pTouch,pEvent);
	}
public:
	static mPotentionMeter* create(const char* backgroundFile, const char* progressFile, const char* thumbFile){
		mPotentionMeter* pRet = new mPotentionMeter();
		if (pRet != NULL)
		{
			// Prepare track for potentiometer
			CCSprite *backgroundSprite      = CCSprite::create(backgroundFile);

			// Prepare thumb for potentiometer
			CCSprite *thumbSprite           = CCSprite::create(thumbFile);

			// Prepare progress for potentiometer
			CCProgressTimer *progressTimer  = CCProgressTimer::create(CCSprite::create(progressFile));
			//progressTimer.type              = kCCProgressTimerTypeRadialCW;
			if (pRet->initWithTrackSprite_ProgressTimer_ThumbSprite(backgroundSprite, progressTimer, thumbSprite))
			{
				pRet->autorelease();
			}
			else
			{
				CC_SAFE_DELETE(pRet);
			}
		}
		return pRet;
	}

	 virtual void sendActionsForControlEvents(CCControlEvent controlEvents){
		 if(controlEvents == CCControlEventValueChanged){
			if(CCControl::isSelected()) return;
		 }
		 CCControl::sendActionsForControlEvents(controlEvents);
	 }

};

class mControlSlider : public CCControlSlider
{
	bool m_bIsMouseDown;

	virtual void onEnter(){
		m_bIsMouseDown = false;
		CCControlSlider::onEnter();
	}

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
		m_bIsMouseDown = CCControlSlider::ccTouchBegan(pTouch,pEvent);
		return m_bIsMouseDown;
	}

	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent){
		if(m_bIsMouseDown)
		{
			CCPoint location = locationFromTouch(pTouch);
			sliderMoved(location);
		}
	}

	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent){
		m_bIsMouseDown = false;
		CCControlSlider::ccTouchEnded(pTouch,pEvent);
	}

	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent){
		m_bIsMouseDown = false;
		CCControlSlider::ccTouchCancelled(pTouch,pEvent);
	}
public:
	static mControlSlider* mControlSlider::create(const char* bgFile, const char* progressFile, const char* thumbFile)
	{
		mControlSlider* pRet = new mControlSlider();

		CCSprite *backgroundSprite      = CCSprite::create(bgFile);
		CCSprite *progressSprite        = CCSprite::create(progressFile);
		CCSprite *thumbSprite           = CCSprite::create(thumbFile);
		pRet->initWithSprites(backgroundSprite, progressSprite, thumbSprite);

		return pRet;
	}

	virtual void setValue(float value){
		// set new value with sentinel
		if (value < m_minimumValue)
		{
			value = m_minimumValue;
		}

		if (value > m_maximumValue) 
		{
			value = m_maximumValue;
		}

		m_value = value;

		this->needsLayout();

		if(!m_bIsMouseDown) this->sendActionsForControlEvents(CCControlEventValueChanged);
	}
};

#endif