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
	int	 miHovered, miOver;	/* <当前选中的菜单|当前鼠标所在菜单 */
	int  miNums;			/* <当前菜单总数 */

	int  miAlignType;		/* <排列类型 | 1-竖直 2-水平 */
	bool mbRegisted;		/* <是否已经注册事件挂钩 */

public:
	MouseMenu();

	void UnRegist();		/* <注销事件挂钩 */
	void Regist();			/* <注册事件挂钩 */

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

	void f_setaligntype(int ai);	/* <设定排列类型|键盘操作用 */
public:
	virtual void w_press(bool up = false);	
	virtual void s_press(bool up = false);	
	virtual void a_press(bool up = false);	
	virtual void d_press(bool up = false);	
	virtual void z_press(bool up = false);	
};

#endif