#ifndef __BATTLE_FIELD_H__
#define __BATTLE_FIELD_H__

#include "cocos2d.h"

class BattleField
{
public:
	static BattleField *sharedBattleField();
	static void purgeSharedBattleField();

	void Clean();
	bool CheckOver();

	void SetUp();

protected:
	virtual bool init();

private:
	static BattleField *mSharedBattleField;
	bool mbIsOver;
	int miEnemy;
};

#endif