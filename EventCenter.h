/*	�¼����ƹ�����
|	-����Ҽ��ͼ��̵�windows�����¼��Ĵ�����
|
*/
#include "cocos2d.h"
#include "utils/Controller.h"

#ifndef __EVENT_CENTER_H__
#define __EVENT_CENTER_H__

class EventCenter 
{
public:
	static EventCenter *sharedEventCenter();
	bool init();
	static void ehandler(UINT message,WPARAM wParam, LPARAM lParam );
	void setController(Controller* c = NULL);	//���ÿ������������ÿ�
	void setScroller(Scroller* s = NULL);
	void setSelector(Selector* s = NULL);

	void setBmCake(Cake* s = NULL);

	static void handlekeydown(WPARAM wParam, LPARAM lParam);
	static void handlekeyup(WPARAM wParam, LPARAM lParam);
	static void handlerightm(WPARAM wParam, LPARAM lParam);

	~EventCenter();

private:
	static EventCenter* mSharedEventCenter;
	static Controller* mController;
	static Scroller* mScroller;
	static Selector* mSelector;

	static Cake* m_bm_cake;

};


#endif