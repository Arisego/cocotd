#ifndef __MOUSEMENU_H__
#define __MOUSEMENU_H__



#include "cocos2d.h"
#include "Controller.h"


USING_NS_CC;

class MouseMenu : public cocos2d::CCMenu, Controller
{
private:
	CCTouch* lastto;

	bool mbEnabled;
	int	 miHovered, miOver;	/* <��ǰѡ�еĲ˵�|��ǰ������ڲ˵� */
	int  miNums;			/* <��ǰ�˵����� */

	int  miAlignType;		/* <�������� | 1-��ֱ 2-ˮƽ */
	bool mbRegisted;		/* <�Ƿ��Ѿ�ע���¼��ҹ� */

public:
	MouseMenu();

	void UnRegist();		/* <ע���¼��ҹ� */
	void Regist();			/* <ע���¼��ҹ� */

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

	void f_setaligntype(int ai);	/* <�趨��������|���̲����� */
public:
	virtual void w_press(bool up = false);	
	virtual void s_press(bool up = false);	
	virtual void a_press(bool up = false);	
	virtual void d_press(bool up = false);	
	virtual void z_press(bool up = false);	
};

#endif