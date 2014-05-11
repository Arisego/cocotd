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
#include "TOUI/Spliter.h"
#include "TOUI/HMenu.h"
#include "TOUI/LgStat.h"

USING_NS_CC;
using namespace std;

#define MENUTAG 3000


/*
	Map layer
	<�����tilemap�Լ�hud���п��ƺ͹��� 
	<[TODO] ���ս�۵�ͼģʽ֧��

*/
class MapLayer : public BYLayerAncestor, public StateMachine, public InterfaceEffect, public Cake
{

public:
	bool m_bSnap;
	void snap();
	void beforesnap();
	void aftersnap();

	~MapLayer();
	MapLayer();

	void switch_to_battle(string s);		//Switch to battle.
	void switch_to_walk();

	TileMap* tm;
	WalkMap* wm;
	BattleMap* bm;

	BattleMenu* m_BattleMenu;
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
	// <EffectCenter���ýӿ�      <[TODO]EffectCenter������ΪScript�Ľ����ߣ�����ɽ��Ӻ�ʧЧ�����в���Ч���ɸ�EChess��Component����
	
	virtual void get_target();
	virtual void effect_over();

	virtual void show_text(string s);	// <Interface��ʵ���߱�Ȼ֪��Ӧ�������ʾ�ı����������Ͳ�Ӧ��ʵ�����Interface
	virtual void refresh_view();		// <ˢ����ʾ��
	virtual void handlesp(Script* sp);
	//
	//////////////////////////////////////////////////////////////////////////
	//void RePack();						//  <�����ι��� | ���������ڵĻ���
public:
	//////////////////////////////////////////////////////////////////////////
	//
	vector<map<string,string>> vdata;	//	<���ݿ��н� -- �������Ͷ��ǹ̶��ģ������ᱻ���������ݿ��С�
	Scriptor* BFsp;

private:
	//	<����
	//////////////////////////////////////////////////////////////////////////

	// <ɾ�� 08.24
	//LinePrior* m_lpJudgement;
	//ElementTicker* m_etClock;
	//LinePin* m_sCon;

	//void TickBack(CCObject* pSender);
	virtual void update(float fDelta);

	int m_iCSum, m_iCCur;		// sum of control arrive;

public:	
	void switch_control();
	ListDBView<ItemCell>* m_ldb;

	int m_iMLState;				//MapLayerState || -1 Init | 1 WalkMap With Hud | 2 Battle Map | 3 The world rotating for next control group
	int m_iFuncType;
	int m_iSUseCase;
protected:
	void click_act();			// <���ݾ���Ĺ���״̬��Ӧ������¼�
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

	ItemCellData* m_sIcd;

	LTLifeBlock* m_ltb;
	ItemPicker* m_IP;
	//////////////////////////////////////////////////////////////////////////
	// <SkillList
	SkillList* m_SkillList ;
	bool show_skill_list();

	//////////////////////////////////////////////////////////////////////////
	// <���˵�
	LeftMenu* m_lfm;
	void showLeftFm();
	void dissmissLeftFm();								// <�漰�����ܵĽ�������
	void lfmenu_back(CCObject* pSender);				// <�˵�����
	bool m_bLfMenu;

public:
	//////////////////////////////////////////////////////////////////////////
	// <Spliter��HUD
	Spliter* m_Spliter;

	//////////////////////////////////////////////////////////////////////////
	// <HMenu��HUD
	HMenu* m_Hmenu;

	//////////////////////////////////////////////////////////////////////////
	// <BattleMap HUD
	RStatBar* m_lsb;
	LStatBar* m_rsb;
	void init_Stat();
	void DismissBHUD();

	CCSprite* m_upArrow;
	CCSprite* m_bottomArrow;
	void Show_Arrows(int aiu, int aib);			// <ˢ�¼�ͷ����ʾ
	void Dissmiss_Arrows();

	//////////////////////////////////////////////////////////////////////////
	// <�ⲿ���ü��� | ������
	bool SC_Popup(const char* asName, int aiFlag = 0x01);		// <�����ɽ����ļ��ܽ���ѡ���޷������򷵻�false;
	
private:
	bool popup_ctn(const char* sMask, int aiFlag);	// <�����������ܵ��õĹ��ܺ���
	void ReleaseCLock();				// <�������ѡ�������״̬
	void CtnSkill();					// <������������

public:
	void show_hud();
	void close_hud();

	void ItemBack(CCObject* pSender);
	void show_menu(int aiFlag = 0xff);			//For battle map only.
	void give_control();		//For battle map only.
	void script_over();			//The animation is over.
	int m_iItem;				// Store which skill or item is being waiting. It's not only for item.

	virtual void right_click();			//EventCenter::sharedEventCenter()->setBmCake(NULL);should be called .
	
	int m_iMaxGroup;			// <���������
	int m_iCurGroup;			// <��ǰӵ�п���Ȩ�����
	int m_iTurn;
	
	/* <AI���ƽӿ� */
	void releaseControl();

	bool mbAIControl;
	void AILock();
	void AIUnLock();

	/* <�ƶ���� || ���� */
	void BMAtDrawMove( string &tsAddition );
	void BMSkDrawMove( string tsAddition );		/* <���ܵķ�ΧȦ�����ݣ�����ǲ���Ҫѡ��Χ�ļ������ִ�� */

	bool mbAutoLogic;		// <�Զ��߼����� || ���ƻغ��Ƿ��ܲ���

	/* <�������� */
	void showUpgrd(EChesses* aeTar);

	/* <BattleMap�������Ҫ������ */
	void clearBattle();

	/* <�����ýӿ� */
	void LoadBattle(Script* ascp);
	void ActSkill();		// <ʹ�ü���
	void CountBack( CCObject* pSender );	// <����ʱ����

	bool mbIsDirect;		// <�Ƿ���ֱ���ͼ��� || ��SkillJudge���ж��Ƕ�ȡ
	//void PunpOut();			// <����BF�е�PunpOut

private:
	LgStat* mlgStat;

public:
	bool mbNeedScroll;		// <�Ƿ���Ҫ����

	void FastShift();		// <���ٵ�λ�л�
	bool mbSBProtect;		// <��ֹ�������Ƶ��µĲ˵���ʧ
	bool mbCheckLock;		// <ֻ�Ǽ�飬��ֹ��Ŀ�꼼���ڼ��ʱ��ֱ���ͷ�

private:
	void BufferSkill(int aiSid, CCObject* aSrc);		// <������Buffer�༼��

public:
	void DelaySound(Script* ascp, CCObject* ae);		// <���Ŵ���ͣ�ӳٵ�����
	CCObject* mDSEChess;								// <������
	void ReleaseSound();				// <����|DelaySound()

};

#endif	// __MAP_LAYER_H__