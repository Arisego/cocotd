#ifndef __BATTLE_FIELD_H__
#define __BATTLE_FIELD_H__

#include "cocos2d.h"
#include "utils\EChess.h"
#include "utils\Scriptor.h"

/*
	<战场管理单例
	-<当标记被置为OVER时会导致ML结束BM，注意添加相关显示。
*/
class BattleField
{
public:
	static BattleField *sharedBattleField();
	static void purgeSharedBattleField();

	void Clean();

	bool IsOver();
	//////////////////////////////////////////////////////////////////////////
	// <战场控制
	map<pair<int,int>,EChesses*> mMapC;	// <战场位置登记
	void SetChess(EChesses* ae,int ax, int ay);			// <设置人物 | 初始化用
	void InitChessRefreh();				// <刷新人物 | 初始化用
	void ChessMoved(EChesses* ae, CCPoint astart, CCPoint aend); // <人物移动，调整统帅数值


	//////////////////////////////////////////////////////////////////////////
	// <战斗控制
	void SetUp(EChesses* aSrc, CCArray* aTar,Script* sp = NULL);		// <战斗设置接口
	void AMOver();									// <一次攻击结束后发起战场检查

protected:
	virtual bool init();
	void CheckOver();								// <检查是否满足结束条件
	void DerLead(int val,int cx, int cy);
	void UnDerLead(int val,int cx, int cy);

	void DepLead(int centx, int centy, int range, int val);
	void DepRemoveLead( int centx, int centy, int range, int val );
private:
	Entiles* meSrc;		// <攻击的发起者	| 被攻击对象在Chara内结算
	CCArray* meTar;		// <接收
	Script*	 mspVals;

	static BattleField *mSharedBattleField;
	bool mbIsOver;
	//int miEnemy;
};

#endif