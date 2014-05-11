/*	<�¼����ƹ�����
|	- <����Ҽ��ͼ��̵�windows�����¼��Ĵ�����
|
*/
#include "cocos2d.h"
#include "utils/Controller.h"
#include "TOUI/LgStat.h"
#include <queue>
#include <utility>
#include "Box2D/Dynamics/b2Body.h"

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
	void setController(Controller* c = NULL);	// <���ÿ������������ÿ�
	void setScroller(Scroller* s = NULL);
	void setSelector(Selector* s = NULL);
	void SetCharaTab(CharaTab* s = NULL);

	void setBmCake(Cake* s = NULL);

	static void handlekeydown(WPARAM wParam, LPARAM lParam);
	static void handlekeyup(WPARAM wParam, LPARAM lParam);
	static void handlerightm(WPARAM wParam, LPARAM lParam);

	~EventCenter();
	void unsetBmCake(Cake* s);

	vector<Scroller*> m_vScrolls;

private:
	static EventCenter* mSharedEventCenter;
	static Controller* mController;
	static Scroller* mScroller;
	static Selector* mSelector;

	static Cake* m_bm_cake;

	static CharaTab* mCharaTab;
	
	static void DefendOn();
	static void DefendOff();
	static LgStat* m_lgstat;
public:
	static bool mbDefendLock;		// <�Ƿ����˷�����
	void SetDefndLck(LgStat* alg = NULL);

private:
	void ShiftPush();	// <�����˻�����

public:
	std::queue<std::pair<b2Body*, std::pair<float, float>>> mQueuedTrans;

};


#endif