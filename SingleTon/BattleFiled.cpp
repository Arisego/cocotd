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
		mspVals = NULL;
		meSrc = NULL;
		meTar = NULL;
		Clean();
		return true;
	} while (false);
	return false;
}

void BattleField::Clean()
{
	mbIsOver = false;

	mspVals = NULL;
	meSrc = NULL;
	meTar = NULL;
	//miEnemy = 0;
}

void BattleField::CheckOver()
{
	if(!meTar) return;
	do 
	{
		CC_BREAK_IF(meTar->count() == 0);


		mbIsOver = false;
		return;
	} while (false);
	mbIsOver = true;
}

void BattleField::SetUp(Entiles* aSrc, CCArray* aTar, Script* sp)
{
	Clean();
	meSrc = aSrc;
	meTar = aTar;
	mspVals = sp;
	CCLog(">[BF]Battle Field Receive the battle:%d",aTar->count());
	if(sp=NULL) 
		CCLog(">[BF]Battle with an empty values....");
}

bool BattleField::IsOver()
{
	return mbIsOver;
}

void BattleField::AMOver()
{
	CheckOver();
}
