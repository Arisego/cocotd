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

	~BattleField();
	void Clean();

	bool IsOver();
	//////////////////////////////////////////////////////////////////////////
	// <战场控制
	map<pair<int,int>,EChesses*> mMapC;	// <战场位置登记
	void SetChess(EChesses* ae,int ax, int ay);			// <设置人物 | 初始化用
	void InitChessRefreh();				// <刷新人物 | 初始化用
	void ChessMoved(EChesses* ae, CCPoint astart, CCPoint aend); // <人物移动，调整统帅数值
	void Judge();						// <判定

	bool HasChess(int ax, int ay);		// <返回一个点是否存在人物

	//////////////////////////////////////////////////////////////////////////
	// <战斗控制
	void SetUp(EChesses* aSrc, CCArray* aTar,Script* sp = NULL);		// <战斗设置接口
	void AMOver();														// <一次攻击结束后发起战场检查

	void SetSrc(EChesses* aSrc);
	void SetTars(CCArray* aTar);

	void PlayEffectSp(const char* asname, CCPoint end);					// <生成一个Sprite，并移动到指定的点。

	//////////////////////////////////////////////////////////////////////////
	// <再次攻击
	EChesses* BackChess1;
	EChesses* BackChess2;
	EChesses* BackChess3;

	void CheckBackCh();		// <从EChessTar中读取可以反击的三个单位
	bool TestBackCh(EChesses* atar);		// <检测是否可以反击并进行反击

	bool LogicContinue();
	bool NormalAttackC();	// <普通攻击的检测
	int	 miState;			// <战斗阶段标志位 0 - 等待生成反击组 || 1 - 对反击组进行遍历询问 || 2 - 反击组二次反击 

	//////////////////////////////////////////////////////////////////////////
	// <动态伤害预判
	void setBattle(bool ab);
	bool mbIsInBattle;		// <是否需要进行战斗预判

	void PreJudge(EChesses* atar);

	//////////////////////////////////////////////////////////////////////////
	// <地图脚本
	void ClearBF();				// <结束后打扫战场
	void InitBfSp(const char* pSz);

	Scriptor* mSpLinker;	// <解析器
	void ShowChess(EChesses* atar);

protected:
	virtual bool init();
	void CheckOver();								// <检查是否满足结束条件

	void DepRemoveLead( int centx, int centy, int range, int val );
	void DepLead( int centx, int centy, int range, int val );

	void SetLead(EChesses* te);
	int DerLead( int val,int cx, int cy,int dx, int dy );	// <根据val的值来判定得到的统帅加成
	void DerLead( int val,int cx, int cy );					// <单体扩散
	void UnDerLead( int val,int cx, int cy );				// <移除自身的影响
	
	//////////////////////////////////////////////////////////////////////////
	// <战斗显示
	int miSFlag;	// <标记战斗运行状态 || 0 - Src未未设置 | 1 -Src已经设置 等待可能的开始和判定 | 2 - 战斗经过第一次判定 <<< 以后的判定可以和mistate结合
	
public:
	bool mbIsMagic; // <是否是魔法攻击
	void RefreshStats();	// <更新两边显示
	EChesses* meTarget;		// <当前被用于显示的右边单位

	void ActionFac();		// <动作被实际执行
	bool CalRate(EChesses* tar_o,int hit_rate_base);
	//void CheckBlood();		// <对血量进行检查

private:
	EChesses* meOrig;		// <保存必要的指针
	EChesses* meSrc;		// <攻击的发起者	| 被攻击对象在Chara内结算
	CCArray* meTar;			// <接收
	Script*	 mspVals;		// <单次战斗的额为数据入口

	CCDictionary* mSpEvList;// <事件列表

	static BattleField *mSharedBattleField;
	bool mbIsOver;
	//int miEnemy;
	void setMeTar(CCArray* atar); // <meTar的封装接口
};

#endif