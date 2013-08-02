#include "SingleTon/BattleFiled.h"
#include "packui/CharaS.h"


BattleField *BattleField::mSharedBattleField = NULL;


BattleField *BattleField::sharedBattleField()
{
	if (mSharedBattleField == NULL)
	{
		mSharedBattleField = new BattleField();
		if (!mSharedBattleField || !mSharedBattleField->init())
		{
			CC_SAFE_DELETE(mSharedBattleField);
		}

	}

	return mSharedBattleField;

}


void BattleField::purgeSharedBattleField()
{
	CC_SAFE_DELETE(mSharedBattleField);

}

bool BattleField::init()
{
	do 
	{


		Clean();
		return true;
	} while (false);
	return false;
}

void BattleField::Clean()
{
	mbIsOver = false;
	miEnemy = 0;
}

bool BattleField::CheckOver()
{
	do 
	{
		CC_BREAK_IF(mbIsOver);
		//CC_BREAK_IF(miEnemy == 0);


		return false;
	} while (false);
	return true;
}

void BattleField::SetUp()
{
	Clean();

}
