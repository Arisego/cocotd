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
	virtual void get_target();
	virtual void effect_over();

	virtual void show_text(string s);	//Interface的实现者必然知道应该如何显示文本，否则它就不应该实现这个Interface
	virtual void refresh_view();		//刷新显示。
	virtual void handlesp(Script* sp);
	//////////////////////////////////////////////////////////////////////////
protected:
	LinePrior* m_lpJudgement;
	ElementTicker* m_etClock;
	void TickBack(CCObject* pSender);
	virtual void update(float fDelta);

	int m_iCSum, m_iCCur;		// sum of control arrive;
	LinePin* m_sCon;
	void switch_control();

	//void registerWithTouchDispatcher();
	int m_iSUseCase;

	ListDBView* m_ldb;
	int m_iMLState;				//MapLayerState || -1 Init | 1 WalkMap With Hud | 2 Battle Map
	int m_iFuncType;

	void draw_func();			//Run the specified function.
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

	ItemCellData* m_sIcd;

	LTLifeBlock* m_ltb;
	ItemPicker* m_IP;
public:
	void show_hud();
	void close_hud();

	void ItemBack(CCObject* pSender);
	void show_menu();			//For battle map only.
	void give_control();		//For battle map only.
	void script_over();			//The animation is over.
	int m_iItem;				// Store which skill or item is being waiting. It's not only for item.

	virtual void right_click();			//EventCenter::sharedEventCenter()->setBmCake(NULL);should be called .

};

#endif	// __MAP_LAYER_H__