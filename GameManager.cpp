#include "cocos2d.h"


#include "System.h"

#include "SoundManager.h"
#include "GameManager.h"
#include "EventCenter.h"

#include "TileMap.h"
#include "GameScene.h"
#include "MenuScene.h"

#include "sublayer/InfoTab.h"


#include "CCSpriterX.h"

using namespace cocos2d;

struct ScripFile{
	char* filename;
	char* scripalname;
	char* password;			
};

GameManager *GameManager::mSharedGameManager = NULL;
GameScene *GameManager::mCurrentStage = NULL;
bool GameManager::mSceneChanged = false;

ModelLayer* GameManager::mdl = NULL;


/** 后台管理类 */
GameManager *GameManager::sharedGameManager()
{
	if (mSharedGameManager == NULL)
	{
		mSharedGameManager = new GameManager();
		if (!mSharedGameManager || !mSharedGameManager->init())
		{
			CC_SAFE_DELETE(mSharedGameManager);
		}

	}

	return mSharedGameManager;

}

GameScene *GameManager::sharedLogicCenter()
{
	if(!mCurrentStage) 
	{
		GameScene* cur = (GameScene*) CCDirector::sharedDirector()->getRunningScene();
		mCurrentStage = cur;
		CCLOG("<<<<<< PB:GameScene is not requested normaly,the ptr may be a wrong one.");
		
	}
	return mCurrentStage;
}


void GameManager::purgeSharedGameManager()
{
	//purgeSharedLogicCenter();
	CCDirector::sharedDirector()->end();
    CC_SAFE_DELETE(mSharedGameManager);

}

void GameManager::purgeSharedLogicCenter()
{
	CC_SAFE_DELETE(mCurrentStage);
}



bool GameManager::init()
{
	bool bRet = false;
    do 
    {
		//系统变量的初始化
		System::Init();

		/** 加密策略：在这里读入未加密的初始化脚本 */
		
		//[todo list]

		EventCenter::sharedEventCenter();
        bRet = true;
		mSceneChanged = false;
		m_bInfo = false;
		NextPara.clear();

    } while (0);

    return bRet;
}



GameManager::GameManager()
{
	cs = NULL;
}


GameManager::~GameManager()
{
	if(mdl) CC_SAFE_RELEASE_NULL(mdl);
	InfoTab::purgeSharedInfoTab();

	SoundManager::purgeSharedSoundManager();	
	ConfigManager::purgeSharedConfigManager();
	
	EffectControler::purgeSharedEffectControler();
	CharaS::purgeSharedCharaS();
	StateCenter::purgeSharedStateCenter();

	EventCenter::purgeSharedEventCenter();
}


void GameManager::runSceneWithId(SceneId id)
{

	CCScene *newScene = NULL;
	CCLOG("Prepare to create the new scene.");
	mCurrentStage = NULL;

	ALSingle::sharedALSingle()->StopAll();
	

	switch (id)
	{
	case SCENE_MENU:
		SoundManager::sharedSoundManager()->StopMusic();
		newScene = MenuScene::create();
		break;

	case SCENE_PLAY:
		mCurrentStage = GameScene::create();
		mCurrentStage->scpfile = "script/init.xml";
		newScene = mCurrentStage;
		break;

	case SCENE_GAMEOVER:
//		newScene = GameOver::scene();
		break;
	
	}
	CCLOG("Prepare to show the new scene.");
	if (newScene)
	{
		CCSpriteFrameCache::sharedSpriteFrameCache()->removeUnusedSpriteFrames();
		CCTextureCache::sharedTextureCache()->removeAllTextures();
		ALSingle::sharedALSingle()->KillALLoadedData();
		

		//mdl->noConfig(0);
		if(mdl){
			mdl->m_clock = false;
		}
		CC_SAFE_RELEASE_NULL(mdl);
		ALSingle::sharedALSingle()->sc = newScene;
		cs = newScene;
		state = id;
		if (CCDirector::sharedDirector()->getRunningScene() == NULL)
			CCDirector::sharedDirector()->runWithScene(newScene);
		else
		{
			CCDirector::sharedDirector()->replaceScene(CCTransitionFade::transitionWithDuration(0.1f, newScene));

		}
		
	}

}

void GameManager::showinfo(){
	if(!cs) return;
	if(cs){
		StatesManager* scs = (dynamic_cast<StatesManager*> (cs));
		if(!scs) return; 
		if(scs->miFlag == 0 || scs->e_State == -1) return;
		scs->PreQuit();
		m_bInfo = true;
		cs->addChild(InfoTab::sharedInfoTab(),98);
		if(mdl) mdl->setTouchEnabled(false);
	}
}

void GameManager::dismissinifo(){
	m_bInfo = false;
	noQuit();
}

void GameManager::preQuit(){
	if(!cs) return;
	if(cs){
		(dynamic_cast<StatesManager*> (cs))->PreQuit();

		ModelLayer* md = new ModelLayer();
		md->autorelease();
		cs->addChild(md,99);

		if(md->m_qlock){
			md->removeFromParent();
		}else{
			md->preQuit();
		}
		
		if(mdl) mdl->setTouchEnabled(false);
		if(m_bInfo) {
			InfoTab::sharedInfoTab()->m_bIsEnabled = false;
			InfoTab::sharedInfoTab()->setTouchEnabled(false);
		}
			
	}
}

void GameManager::noQuit(){
	if(!cs) return;
	if(cs){
		if(!mdl) {
			if(!m_bInfo)
				(dynamic_cast<StatesManager*> (cs))->NoQuit();
		}
		else {
			if(m_bInfo) {
				InfoTab::sharedInfoTab()->m_bIsEnabled = true;
				InfoTab::sharedInfoTab()->setTouchEnabled(true);
			}
			else mdl->setTouchEnabled(true);
		}

		
	}	
}

void GameManager::goConfig(){
	if(!cs) return;
	if(cs){
		if(!mdl) {
			StatesManager* scs = (dynamic_cast<StatesManager*> (cs));
			if(!scs) return; 
			if(scs->miFlag == 0 || scs->e_State == -1) return;
			scs->PreQuit();
			
			mdl = new ModelLayer();
			cs->addChild(mdl,98);
			mdl->preConfig(scs->miType,scs->miFlag,scs->miTab);
		}
		else noConfig();
	}
}

void GameManager::preConfig(int type, int flag, int tab){
	if(!cs) return;
	if(cs){
		(dynamic_cast<StatesManager*> (cs))->PreQuit();

		if(!mdl) {
			mdl = new ModelLayer();
//			mdl->autorelease();				//Manual Control is implied.
			cs->addChild(mdl);
		}
		mdl->preConfig(type,flag,tab);


	}
}

void GameManager::noConfig(){
	if(!mdl) return;
	//mdl->noConfig();
	mdl->m_clock = false;
	mdl->removeFromParent();
	CC_SAFE_RELEASE_NULL(mdl);
	if(!cs) return;
	if(m_bInfo) return;
	if(cs){
		(dynamic_cast<StatesManager*> (cs))->NoQuit();


	}
}

void GameManager::prePareSave()
{
	if(mdl) mdl->setVisible(false);
	if(m_bInfo) InfoTab::sharedInfoTab()->setVisible(false);
}

void GameManager::finisSave()
{
	if(mdl)	mdl->setVisible(true);
	if(m_bInfo) InfoTab::sharedInfoTab()->setVisible(true);
}

void GameManager::fullPathFromRelativePath( const char *pszRelativePath,string &psz )
{
	
	do 
	{
		psz.clear();
		psz = CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(pszRelativePath);
	} while (psz.length() == 0);
}
