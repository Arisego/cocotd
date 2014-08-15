#include "ABScene.h"
#include "packui\ConfigManager.h"

ABScene::~ABScene()
{

}

bool ABScene::init()
{
	do 
	{
		mlBasic = ABasicLayer::create();
		addChild(mlBasic,0);
		AddState(mlBasic);

		e_State = 1;
		return true;
	} while (false);
	return false;

}

ABScene::ABScene(int zid)
{
	mZBId = zid;
	ReadConfig();
	init();
	autorelease();
}

void ABScene::ReadConfig()
{
	string ts = ConfigManager::sharedConfigManager()->getZBState(mZBId);
	CCLog(">[ABScene] ReadConfig:%s", ts.c_str());

	Json::Reader tjR;
	if (!tjR.parse(ts, mjvConfig)){
		exit(2002);
	}
}

void ABScene::GotoNext()
{
	// TODO<<<<
	exit(333);
}

