#ifndef __MAP_LAYER_H__
#define __MAP_LAYER_H__

#include "cocos2d.h"

#include "utils/Controller.h"
#include "utils/Scriptor.h"
#include "utils/MouseMenu.h"
#include "TileMap.h"
#include "sublayer/BattleMap.h"
#include "sublayer/WalkMap.h"
#include "utils/States.h"
#include "hud/LTLifeBlock.h"
#include "packui/InterfaceEffect.h"
#include "hud/BattleMenu.h"
#include "hud/LinePrioer.h"
#include "hud/ElementTicker.h"
#include "packui/ListDBView.h"

#include "hud/LStatBar.h"

#include "Component/MapLayerComp.h"
#include "../hud/RStatBar.h"

#include "hud/SkillList.h"
#include "hud/LeftMenu.h"

USING_NS_CC;
using namespace std;

#define MENUTAG 3000


/*
	Map layer
	<负责对tilemap以及hud进行控制和管理 
	<[TODO] 添加战役地图模式支持

*/
class MapLayer : public BYLayerAncestor, public StateMachine, public InterfaceEffect, public Cake
{

public:
	bool m_bSnap;
	void snap();
	void beforesnap();
	void aftersnap();

	~MapLayer();

	void switch_to_battle(string s);		//Switch to battle.
	void switch_to_walk();

	TileMap* tm;
	WalkMap* wm;
	BattleMap* bm;

	BattleMenu* t_bm;
	int m_curtag;

	virtual void Pause();
	virtual void Resume();
	virtual void Close();

	void LoadMap(const char* pszN, const char* pscR, Script* tp);
	void openTileMap(const char* pszN, const char* pscR);			//Map path & Script Path
	void openBattleMap(const char* pszN, int i_enemygr);			//Battle Start with map path & enemy list.
	void openBattleMap(const char* pszN, vector<int> vi_enemes);	//TODO: generate yourself. Not implemented.
	void closeBattleMap();
	void closeTileMap();

	void f_init();
	void f_stopcontrol();
	void f_pauseall();
	void f_resumeall();
	void f_resumecon();

	void f_init_battle();
	void f_refresh_battle();

	void item_use(CCObject* pSender);
	void menu_back(CCObject* pSender);				// Pop up menu callback.
	//////////////////////////////////////////////////////////////////////////
	// <EffectCenter调用接口      <[TODO]EffectCenter将被作为Script的交接者，在完成交接后失效，所有播放效果由各EChess的Component负责。
	virtual void get_target();
	virtual void effect_over();

	virtual void show_text(string s);	// <Interface的实现者必然知道应该如何显示文本，否则它就不应该实现这个Interface
	virtual void refresh_view();		// <刷新显示。
	virtual void handlesp(Script* sp);
	//
	//////////////////////////////////////////////////////////////////////////
	void RePack();						//  <第三次攻击
protected:
	//////////////////////////////////////////////////////////////////////////
	//
	vector<map<string,string>> vdata;	//	<数据库中介 -- 攻击类型多是固定的，将不会被保存在数据库中。
	Scriptor* BFsp;

	
	//	<攻击
	//////////////////////////////////////////////////////////////////////////

	// <删除 08.24
	//LinePrior* m_lpJudgement;
	//ElementTicker* m_etClock;
	//LinePin* m_sCon;

	//void TickBack(CCObject* pSender);
	virtual void update(float fDelta);

	int m_iCSum, m_iCCur;		// sum of control arrive;
	
	void switch_control();

	//void registerWithTouchDispatcher();
	int m_iSUseCase;

	ListDBView<ItemCell>* m_ldb;
	int m_iMLState;				//MapLayerState || -1 Init | 1 WalkMap With Hud | 2 Battle Map
	int m_iFuncType;

	void click_act();			// <根据具体的功能状态相应鼠标点击事件
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

	ItemCellData* m_sIcd;

	LTLifeBlock* m_ltb;
	ItemPicker* m_IP;
	//////////////////////////////////////////////////////////////////////////
	// <SkillList
	SkillList* m_SkillList ;
	void show_skill_list();

	//////////////////////////////////////////////////////////////////////////
	// <主菜单
	LeftMenu* m_lfm;
	void showLeftFm();
	void dissmissLeftFm();	// <涉及到可能的解锁操作
	void lfmenu_back(CCObject* pSender);				// <菜单返回
	bool m_bLfMenu;

public:
	//////////////////////////////////////////////////////////////////////////
	// <BattleMap HUD
	LStatBar* m_lsb;
	RStatBar* m_rsb;
	void init_Stat();
	void DismissBHUD();

public:
	void show_hud();
	void close_hud();

	void ItemBack(CCObject* pSender);
	void show_menu();			//For battle map only.
	void give_control();		//For battle map only.
	void script_over();			//The animation is over.
	int m_iItem;				// Store which skill or item is being waiting. It's not only for item.

	virtual void right_click();			//EventCenter::sharedEventCenter()->setBmCake(NULL);should be called .

	int m_iMaxGroup;			// <最大的组别编号
	int m_iCurGroup;			// <当前拥有控制权的组别
	int m_iTurn;



};

#endif	// __MAP_LAYER_H__