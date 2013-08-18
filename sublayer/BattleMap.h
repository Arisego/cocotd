#ifndef __BATTLE_MAP_H__
#define __BATTLE_MAP_H__

#include "TileMap.h"

#include "AppMacros.h"
#include "utils/EChess.h"

using namespace std;

struct StepNode{
	int x,y,G,status;
	float H;

	float getF(){
		return G+H;
	}

};

class BattleMap : public TileMap{
public:
	~BattleMap();

	CCPoint m_touchpoint;
	CCTouch* m_touch;
	int b_battle;			//battlemap��state��� || 1 - �ȴ������˵� 2 - �˵��������ܾ��¼� 3 - �ȴ���ͼ����ѡ�񲢷��ظ�draw_func();
	int m_mi,m_mj;
	set<pair<int,int>> cs_y,cs_b,cs_r,cs_block,cs_dis,cs_hit_block;			//��¼��ά����ļ��� cs_block - ZOC���� cs_dis - �޷�վ�������� cs_hit_block - ���ƶ���Χ�ڵ�zoc
	vector<CCPoint> vc_path;
	unsigned int c_r,c_b,c_y;
	int max_x,max_y;
	set<pair<int,int>> ts_last;

	//vector<int> m_viEnenmis;
	map<int,int> m_miEnemis;
	map<pair<int,int>,string> m_mpiisCoor;					//������ٲ���ĳ��������Entile����

	void f_generateEnemy(int i);

	EChesses* m_eCurMouse;

	BattleMap(const char *a){
		map_path = a;
		m_eCurMouse = NULL;
		m_itemlist = NULL;
	}

	virtual bool init();

	void update(float dt);
	void f_decide(int i,int j);
	
	void f_load_chara();		//load chara from charas.
	virtual Script* f_scrfind(string gn, int t);

	void checkpoint(CCTouch* ct);	
	void draw_moving_tile();				//draw the moving range out.
	void draw_moving_block();				//generate block and disable tiles.
	void draw_skill_range(int a_type, vector<int> a_ran);				//draw the range of skill and item use;[IN] a_type && paralist int
	void set_mouse_range(int a_type, vector<int> a_ran);				// <��������ʵʱ����ͼ�� 0 - �� 1 - ������
	void draw_mouse_range(CCPoint a_cp);

	void dps_ring(CCPoint a_cp, set<pair<int,int>> &a_dt, int a_max);			// Generate a set for ring. ʮ�ּ�
	void dps_rect(CCPoint a_cp, set<pair<int,int>> &a_dt, int a_max);			//������
	void dps_range(CCPoint a_cp , set<pair<int,int>> &a_dt, int a_max);	

	void imply_set(set<pair<int,int>> a_dt, unsigned int d_c,bool ab_clean = false);		//Draw the specify set with specify color.
	CCPoint m_mou_cur;			//Current mouse point.
	bool move_control();		//trying to move the controller.
	void clean_cs();			// clean states;
	void a_star();				//A*

	int	 miRangeType;
	bool arange_target(int a_type);			//Test if target is in ts_last and produce the target this.

	CCArray* m_caTarget;					//Array for target;
	CCArray* m_caTarCharas;					//Array for target Charas(), generate it so we do not to calculate it every time.
	int m_mouse_type;

	void show_text(string s);
	void show_text(EChesses* a_ec,string s);

	bool m_bAnimateOver;					//Whether the animate is over.
	void control_switch();					//called when control is switched.

	virtual void clearcontrol();
	bool arange_targetwithtest( int a_type );
	//////////////////////////////////////////////////////////////////////////
	//	<����
	virtual void HandleScriptor(Scriptor* asp);
	int center_i,center_j;

	bool ArrangePoint(CCPoint a);
protected:
	CCPoint m_con_cur;			//Current controller point; inner use only.

	
	vector<int> m_mouse_arrs;

	virtual void ccTouchMoved(CCTouch *touch, CCEvent * pEvent);

	bool f_load_entile();		//inform chara-entile to map; called inside init;
	
	
	float getNodeH(CCPoint to, StepNode& node);
	StepNode getNodeChild(StepNode sn, int i);
	list<StepNode> getSearchPath(CCPoint startPos, CCPoint to);
	bool as_checkpoint( int x, int y );
	bool as_checkNpcMove( int x, int y, int m, int n );
	
};

#endif