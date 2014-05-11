#ifndef __MOUSEMENU_H__
#define __MOUSEMENU_H__



#include "cocos2d.h"


USING_NS_CC;

class MouseMenu : public cocos2d::CCMenu
{
private:
	CCTouch* lastto;

	bool mbEnabled;
public:
	MouseMenu();

	 void registerWithTouchDispatcher();
	
	virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
	virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
    virtual void ccTouchCancelled(CCTouch *touch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);

	//////////////////////////////////////////////////////////////////////////	
	void onExit();

	static MouseMenu*  menuWithItems(CCMenuItem* item, ...);
	static MouseMenu* menuWithItem(CCMenuItem* item);
	static MouseMenu* createWithArray(CCArray* pArrayOfItems);
	//////////////////////////////////////////////////////////////////////////
	
	void update(float dt);
	void f_init();

	void ReSume();
};

#endif