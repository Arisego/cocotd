#ifndef __BATTLE_FIELD_H__
#define __BATTLE_FIELD_H__

#include "cocos2d.h"
#include "utils\Entiles.h"
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

	void SetUp(Entiles* aSrc, CCArray* aTar,Script* sp = NULL);		// <战场设置接口
	void AMOver();									// <一次攻击结束后发起战场检查

protected:
	virtual bool init();
	void CheckOver();								// <检查是否满足结束条件

private:
	Entiles* meSrc;		// <攻击的发起者	| 被攻击对象在Chara内结算
	CCArray* meTar;		// <接收
	Script*	 mspVals;

	static BattleField *mSharedBattleField;
	bool mbIsOver;
	//int miEnemy;
};

#endif