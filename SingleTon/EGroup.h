#ifndef __E_ROUND_H__
#define __E_ROUND_H__

#include "cocos2d.h"
#include "utils\Scriptor.h"
USING_NS_CC;


/*
	< EGroup | 势力信息管理类 - EChess
		< 负责查询所属势力的详细信息
		< 负责经验值的赋予
		< 负责伤害的属性计算
		< 不保存数据，只为统一计算方法
*/

class EGroup
{
public:
	EGroup();
	~EGroup();

	static EGroup *sharedEGroup();
	static void purgeSharedEGroup();

	void	 FactDamage(CCObject* aSrc, CCObject* aTar, int adamv, int atype, int mtype = 0);		// <伤害计算初步完成后进行进一步叠加演算
	bool	 CheckGrp(CCObject* aTar);																// <检查是否为玩家控制势力 | 实际入参 EChess* | 势力初始化将由脚本完成 | == 是否需要经验值演算 == 是否AI
	void	 ExpHunp(CCObject* aSrc, int aiV);														// <消耗魂魄引起的经验
	void	 ExpDead(CCObject* aSrc);																// <单位死亡追加经验 || 注意留心伤害来源死亡的情况
	bool	 IsEnemy(CCObject* aSrc, CCObject* aTar);	// <Src是否敌视Tar|实际入参 EChess*		

private:
	static EGroup* mSharedEGroup;
	virtual bool init();

	CCObject* meSrc;
	CCObject* meTar;
public:
	bool	 ValChange(Script* asp);																	// <变更数值
	bool	 AtkPlay(Script* asp, CCObject* aesrc = NULL, CCObject* aetar = NULL);						// <播放攻击|额外的|连续
	bool	 AtkSkill(Script* asp, CCObject* aesrc = NULL, CCObject* aetar = NULL);						// <播放技能|额外的|连续

	int		 getRound();																				// <获得当前回合数
	int		 getTurn();

	bool	 ManuAlrange(Script* asp, CCObject* aSrc, CCObject* aTar = NULL);												// <通用的简化测量函数
	
	int		miDame;	// <攻击伤害数值 | 
	bool	MakDeath(Script* asp);	// <强制致死调用

	CCObject* meLast;		// <调用单位用来缓存自己的机制，使用完后就作废，所以可以随时使用。
};

#endif