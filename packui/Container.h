/*
|	UI容器类
|
|
*/
#ifndef __CONTAINER_H__
#define __CONTAINER_H__

#include "byeven/BYCocos.h"
#include "cocos-ext.h"

#include "utils/SpriteTextMenuItem.h"
USING_NS_CC_EXT;
using namespace std;

//Button容器基类
//State: Normal, Hover, Selected, Disable
//BYLayer机制已经更改，可以接受事件
//[注意！！！] Container不会将事件传递给子控件
class Container : public BYLayer
{
private:
	CCNode* spriteSelected;
	CCNode* spriteHover;
	CCNode* spriteNormal;
	CCNode* spriteDisable;
	bool m_bIsEnabled;
	CCTouch* mLasto;

protected:

	enum ContainerState
	{
		C_STATE_NORMAL = 0,
		C_STATE_HOVERD,
		C_STATE_SELECT,
		C_STATE_DISABLE
	};

	int m_iState;
	

	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;

	void activate();

public:
	CCLabelBMFont* labelAtlas;

	Container();
	~Container();

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void onExit();

	virtual void update(float fDelta);
	virtual bool checkTouch(CCTouch *pTouch);

	void initwithPsz(const char* s_MenuItem,const char* s_Title,float width,float height, CCObject* target, SEL_MenuHandler selector);
	void initString(const char* s_Title,float width,float height,string c9img, CCObject* target, SEL_MenuHandler selector);
	void initwithPre(string, float, float);
	void setactivator(CCObject* target, SEL_MenuHandler selector);

	virtual void onHover();
	virtual void onSelect();
	virtual void onNormal();

	void onDisable();
	void SpriteRefresh();

	void setEnability(bool tar);
	void settouchstate(bool tar);				//change m_bISenable. IN:setEnability also change this.

	virtual void setOpacity(GLubyte opacity);
	virtual void setstringnull();
	void setstring(string s);

};

#endif