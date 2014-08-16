#include "ABScene.h"
#include "packui\ConfigManager.h"
#include "StateCenter.h"
#include "GameManager.h"

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
	StateCenter::sharedStateCenter()->f_LoadOver();
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
	GameManager::sharedGameManager()->ChangeScene(GameManager::SCENE_PLAY, mjvConfig["next_map"].asCString());
}

