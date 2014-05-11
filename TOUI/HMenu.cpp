#include "HMenu.h"
#include "GameManager.h"
#include "hud\RightUpgr.h"
#include "RWL.h"
#include "SingleTon\BattleFiled.h"

#define RA_TAG 0x99
//////////////////////////////////////////////////////////////////////////
// Init

//HMenu *HMenu::mSharedHMenu = NULL;

HMenu *HMenu::sharedHMenu()
{
	if (mSharedHMenu == NULL)
	{
		mSharedHMenu = new HMenu();
		if (!mSharedHMenu || !mSharedHMenu->init())
		{
			CC_SAFE_DELETE(mSharedHMenu);
		}

	}

	return mSharedHMenu;

}


//void HMenu::purgeSharedHMenu()
//{
//	CC_SAFE_DELETE(mSharedHMenu);
//
//}

//////////////////////////////////////////////////////////////////////////


HMenu::HMenu()
	:mbShown(false)
{
	mRightUp = new RightUpgr();
	mRightUp->setAnchorPoint(CCPointZero);
	mRightUp->setPosition(ccp(555,0));
	mRightUp->f_init();
	addChild(mRightUp,1);
	mRightUp->setVisible(false);
	mRightUp->autorelease();
	init();
}

HMenu::~HMenu()
{
	CCLog(">[HMenu] ~HMenu() ");
}

void HMenu::f_open()
{
	if(!mbShown) return;
	GameManager::sharedLogicCenter()->ml->f_stopcontrol();
	setVisible(true);
}

void HMenu::f_pause()
{
	setVisible(false);
}

void HMenu::showLeftFM()
{
	CCLog(">[HMenu] showLeffm() || Begin.");
	miState = 0;
	mbShown = true;
	GameManager::sharedLogicCenter()->ml->f_stopcontrol();
	setVisible(true);
}

void HMenu::dissmissLeftFM()
{
	CCLog(">[HMenu] DismissLeffm() || Begin.");
	GameManager::sharedLogicCenter()->ml->f_resumecon();
	setVisible(false);
	//removeFromParentAndCleanup(false);
	mbShown = false;
}

void HMenu::showRightUpg(EChesses* aeTar)
{
	miState = 1;
	CCLog(">[HMenu] showRightUpg() || Begin.");
	mbShown = true;
	GameManager::sharedLogicCenter()->ml->f_stopcontrol();
	setVisible(true);
	mRightUp->showUpgr(aeTar);
	mRightUp->setVisible(true);
	BattleField::sharedBattleField()->ShowChess(NULL);
}

void HMenu::dissmissRightUpg()
{
	mRightUp->setVisible(false);
	mbShown = false;
	miState = -1;
	//GameManager::sharedLogicCenter()->ml->f_resumecon();
	
	//((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRelease();
}

void HMenu::dissMiss()
{
	switch (miState)
	{
	case 2:
		{
			break;
		}
	default:
		break;
	}
	miState = 0;
}

void HMenu::showSB()
{
	if(miState == 2) return;
	miState = 2;
	Rwl* tRwl = new Rwl(400, 400);
	tRwl->setAnchorPoint(CCPointZero);
	tRwl->setPosition(ccp(300,100));
	addChild(tRwl, 1, RA_TAG);
}
