/*	事件机制管理类
|	-鼠标右键和键盘等windows独有事件的处理类
|
*/
#include "cocos2d.h"
#include "utils/Controller.h"

#ifndef __EVENT_CENTER_H__
#define __EVENT_CENTER_H__

USING_NS_CC;
using std::vector;

class EventCenter 
{
public:
	static EventCenter *sharedEventCenter();
	static void purgeSharedEventCenter();

	bool init();
	static void ehandler(UINT message,WPARAM wParam, LPARAM lParam );
	void setController(Controller* c = NULL);	//设置控制器，调空置空
	void setScroller(Scroller* s = NULL);
	void setSelector(Selector* s = NULL);
	void SetCharaTab(CharaTab* s = NULL);

	void setBmCake(Cake* s = NULL);

	static void handlekeydown(WPARAM wParam, LPARAM lParam);
	static void handlekeyup(WPARAM wParam, LPARAM lParam);
	static void handlerightm(WPARAM wParam, LPARAM lParam);

	~EventCenter();

	vector<Scroller*> m_vScrolls;

private:
	static EventCenter* mSharedEventCenter;
	static Controller* mController;
	static Scroller* mScroller;
	static Selector* mSelector;

	static Cake* m_bm_cake;

	static CharaTab* mCharaTab;

};


#endif