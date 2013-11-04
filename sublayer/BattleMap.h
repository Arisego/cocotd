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
private:
	int b_battle;			//battlemap用state标记 || 1 - 等待弹出菜单 2 - 菜单弹出，拒绝事件 3 - 等待地图对象选择并返回给draw_func(); || 4 || 5 || 6 技能接续 || 7 技能接续选择中
public:
	void set_bbattle(int i);
	int get_bbattle();
	int m_mi,m_mj;
	set<pair<int,int>> cs_y,cs_b,cs_r,cs_block,cs_dis,cs_hit_block;			//记录二维数组的集合 cs_block - ZOC区域 cs_dis - 无法站立的区域 cs_hit_block - 在移动范围内的zoc
	set<pair<int,int>> cs_cy;	// <cs_cy - 额外的显示黄色的区域
	vector<CCPoint> vc_path;
	unsigned int c_r,c_b,c_y;
	int max_x,max_y;
	set<pair<int,int>> ts_last;

	//vector<int> m_viEnenmis;
	map<int,int> m_miEnemis;
	//map<pair<int,int>,string> m_mpiisCoor;					//方便快速查找某个区域内Entile对象。

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
	
	void f_load_chara();		// <自动从BattleField获取列表
	virtual Script* f_scrfind(string gn, int t);

	void checkpoint(CCTouch* ct);	
	void draw_moving_tile();				//draw the moving range out.
	void draw_moving_block();				//generate block and disable tiles.
	void draw_skill_range(int a_type, vector<int> a_ran);				//draw the range of skill and item use;[IN] a_type && paralist int
	void set_mouse_range(int a_type, vector<int> a_ran);				// <设置鼠标的实时绘制图形 0 - 点 1 - 正方形
	void draw_mouse_range(CCPoint a_cp);								// <进行鼠标实时绘制的点的集合的生成

private:
	void dps_ring(CCPoint a_cp, set<pair<int,int>> &a_dt, int a_max);			// Generate a set for ring. 十字架
	void dps_rect(CCPoint a_cp, set<pair<int,int>> &a_dt, int a_max);			//正方形
	void dps_range(CCPoint a_cp , set<pair<int,int>> &a_dt, int a_max);	

	void dps_szd(CCPoint a_cp, set<pair<int,int>> &a_dt, int a_min , int a_max);		// <生成图形：十字架段
	void dps_rectangle(CCPoint a_cp, set<pair<int,int>> &a_dt, CCPoint a1, CCPoint a2);  // <生成图形： 从a1到a2的矩形

	void imply_set(set<pair<int,int>> a_dt, unsigned int d_c,bool ab_clean = false);		//Draw the specify set with specify color.

public:
	CCPoint m_mou_cur;			//Current mouse point.
	bool move_control();		//trying to move the controller.
	void clean_cs();			// clean states;
	void a_star();				//A*

	int	 miRangeType;			// <传递usecase的变量
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
	//	<攻击
	virtual void HandleScriptor(Scriptor* asp);
	int center_i,center_j;

	bool ArrangePoint(CCPoint a);
	//////////////////////////////////////////////////////////////////////////
	// <技能是否满足点击发动的条件
private:
	int miLink;		// <技能生效类型
	bool mbLink;	// <技能生效的标志位
	void calLink(CCPoint a_cp/* <当前鼠标所在的点*/);	// <进行技能生效演算
public:
	void setLink(int aiLink);	// <设置生效类型
	bool testLink();	// <对生效标志位进行判定


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

	//////////////////////////////////////////////////////////////////////////
	// <移动和UI
	CCNode* mFolCamara;
	void fAutoCamara();

public:
	bool f_RangeTest(int a_type, vector<int> a_ran, CCPoint a_cp,CCPoint a);
	bool f_Arange(int a_type,CCObject* atar);
	void find_target_arrage(int atype);				// <寻找范围内的单位的实际执行体
	//////////////////////////////////////////////////////////////////////////
	// <技能中单位的移动和操作
	void moveChess(EChesses* at, CCPoint ta, bool isRelate = true);		// <移动指定的单位
	void moveChessConcru(EChesses* at);				// <移动指定的单位到当前鼠标的位置

	void ChangeFace(EChesses* asrc, EChesses* atar);	// <改变单位的朝向 | 前面的朝向后面的
	void ChangeFaceConcur(EChesses* atar);					// <使得m_controller朝向指定的单位

	void ChangeAllFace();

	void PauseAllActs();		// <停止所有需要停止的子节点
	void ResumeAllActs();		// <恢复
};

#endif