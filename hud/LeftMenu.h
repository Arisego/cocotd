#ifndef __LEFT_MENU_H__
#define __LEFT_MENU_H__

#include "cocos2d.h"
#include "packui\Container.h"
#include "byeven\BYCocos.h"

USING_NS_CC;

// <������ť
class LFBtn : public Container{
public:
	LFBtn(const char* at);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
};

//Left Status Bar
class LeftMenu : public BYLayerDescendant
{
public:
	int bitLock;

	bool f_init();
	void show();
	void dismiss();

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
		return BYLayerDescendant::ccTouchBegan(pTouch,pEvent);
	}

	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent){
		BYLayerDescendant::ccTouchEnded(pTouch,pEvent);
	}


private:
	void Add_Button(const char* name,int tag);
	std::vector<LFBtn*> m_vBtns;
	float m_fHeight,m_fWidth;


	void buttonback(CCObject* pSender);

	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;
	void activate(CCObject* pSender);


public:
	void setactivator(CCObject* target, SEL_MenuHandler selector);
	bool m_bIsEnabled;
	void Refresh_Button();
	int miFlag;					//The Select one.

};


#endif