#include "Ab_asicLayer.h"
#include "packui\CharaS.h"
#include "Ab_Head.h"
#include "..\Corrode\Classes\sublayer\InfoTab.h"
#include "Ab_Ep.h"
#include "Ab_Button.h"
#include "Ab_Prac.h"
#include "Ab_Next.h"
#include "GameManager.h"
#include "ABScene.h"

#define ROOT_AB ((ABScene*) getParent())

ABasicLayer::ABasicLayer()
{
}

ABasicLayer::~ABasicLayer()
{
}

bool ABasicLayer::init()
{
	do 
	{
		setTouchEnabled(true);
		CCSprite* tcBackG = CCSprite::create("Images/UI/zb_bg.jpg");
		tcBackG->setAnchorPoint(CCPointZero);
		tcBackG->setPosition(CCPointZero);
		addChild(tcBackG,-1);

		mAbNext = new AbNext(83,74);
		mAbNext->setAnchorPoint(CCPointZero);
		mAbNext->setPosition(ccp(800,500));
		addChild(mAbNext,0);

		CCSize os = CCDirector::sharedDirector()->getVisibleSize();
		setContentSize(os);

		//////////////////////////////////////////////////////////////////////////
		float tPosY = 370;
		float tPoxX = -7;

		CCDictionary* tcda = CharaS::sharedCharaS()->m_caCharas;
		CCDictElement* tcde;
		CCDICT_FOREACH(tcda, tcde){
			Chara* tca = (Chara*) tcde->getObject();
			ABHead* tabh = new ABHead(tca->m_iCharaID, 250, 100);
			tabh->setAnchorPoint(CCPointZero);
			tabh->setPosition(tPoxX,tPosY);
			tabh->setactivator(this, menu_selector(ABasicLayer::HdBack));
			mvHeads.push_back(tabh);
			tabh->setScale(0.8);
			addChild(tabh,0);
			tPosY -= 90;
		}

		CCSprite* tcTitle = CCSprite::create("Images/UI/chapter_1.png");
		tcTitle->setAnchorPoint(CCPointZero);
		tcTitle->setPosition(ccp(67,475));
		addChild(tcTitle,0);

		AbEpisod* tabe = NULL;
		
		{
			tPoxX = 195;
			tPosY = 310;

			tabe = new AbEpisod(1,269,164);
			tabe->setAnchorPoint(CCPointZero);
			tabe->setPosition(tPoxX,tPosY);
			addChild(tabe);
		}

		{
			tPoxX = 455;
			tPosY = 310;

			tabe = new AbEpisod(0,269,164);
			tabe->setAnchorPoint(CCPointZero);
			tabe->setPosition(tPoxX,tPosY);
			addChild(tabe);
		}

		{
			tPoxX = 455;
			tPosY = 148;

			tabe = new AbEpisod(1,269,164);
			tabe->setAnchorPoint(CCPointZero);
			tabe->setPosition(tPoxX,tPosY);
			addChild(tabe);
		}

		{
			tPoxX = 195;
			tPosY = 148;

			tabe = new AbEpisod(0,269,164);
			tabe->setAnchorPoint(CCPointZero);
			tabe->setPosition(tPoxX,tPosY);
			addChild(tabe);
		}

		AbButton* tabb = NULL;

		tPoxX = 715;
		tPosY = 200;
		tabb = new AbButton("items", 191, 30);
		tabb->setAnchorPoint(CCPointZero);
		tabb->setPosition(tPoxX,tPosY);
		addChild(tabb);

		tPosY += 34;
		tabb = new AbButton("change", 191, 30);
		tabb->setAnchorPoint(CCPointZero);
		tabb->setPosition(tPoxX,tPosY);
		addChild(tabb);

		tPosY += 34;
		tabb = new AbButton("xx", 191, 30);
		tabb->setAnchorPoint(CCPointZero);
		tabb->setPosition(tPoxX,tPosY);
		addChild(tabb);

		tPosY = 10;
		tabb = new AbButton("sys", 191, 30);
		tabb->setAnchorPoint(CCPointZero);
		tabb->setPosition(tPoxX,tPosY);
		tabb->setactivator(this, menu_selector(ABasicLayer::SCBack));
		tabb->setTag(1);
		addChild(tabb);

		tPosY += 34;
		tabb = new AbButton("load", 191, 30);
		tabb->setAnchorPoint(CCPointZero);
		tabb->setPosition(tPoxX,tPosY);
		tabb->setactivator(this, menu_selector(ABasicLayer::SCBack));
		tabb->setTag(2);
		addChild(tabb);

		tPosY += 34;
		tabb = new AbButton("save", 191, 30);
		tabb->setAnchorPoint(CCPointZero);
		tabb->setactivator(this, menu_selector(ABasicLayer::SCBack));
		tabb->setPosition(tPoxX,tPosY);
		tabb->setTag(3);
		addChild(tabb);

		tPoxX = 505;
		tPosY = 10;
		tabb = new AbButton("shop", 191, 30);
		tabb->setAnchorPoint(CCPointZero);
		tabb->setPosition(tPoxX,tPosY);
		addChild(tabb);

		CCSprite* t_xl = CCSprite::create("Images/UI/xl.png");
		t_xl->setAnchorPoint(CCPointZero);
		t_xl->setPosition(ccp(435, 70));
		addChild(t_xl);

		int tiGold = CharaS::sharedCharaS()->m_iGold;
		if(tiGold<0){
			tiGold = 0;
			CharaS::sharedCharaS()->m_iGold = 0;
		}
		std::stringstream tss;
		tss << tiGold;
		mlbMoney = CCLabelBMFont::create(tss.str().c_str(), FNT_ENG_NUM);
		mlbMoney->setScale(1.4);
		mlbMoney->setAnchorPoint(CCPointZero);
		mlbMoney->setPosition(ccp(535,73));
		mlbMoney->setColor(ccYELLOW);
		this->addChild(mlbMoney,1);

		CCSprite* tcs_ps = CCSprite::create("Images/UI/prac_split.png");
		tcs_ps->setAnchorPoint(CCPointZero);
		tcs_ps->setPosition(ccp(200,114));
		addChild(tcs_ps);

		tPoxX = 220;
		tPosY = 10;
		ABPrac* tabp = NULL;
		tabp = new ABPrac(1,181,50);
		tabp->setAnchorPoint(CCPointZero);
		tabp->setPosition(tPoxX,tPosY);
		addChild(tabp,1);

		tPoxX = 220;
		tPosY = 61;
		tabp = new ABPrac(2,181,50);
		tabp->setAnchorPoint(CCPointZero);
		tabp->setPosition(tPoxX,tPosY);
		addChild(tabp,1);
		tabp->onDisable();

		return true;
	} while (false);
	return false;
}

void ABasicLayer::registerWithTouchDispatcher()
{
	CCDirector* pDirector = CCDirector::sharedDirector();
	pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

void ABasicLayer::HdBack(CCObject* sender)
{
	miCid = ((ABHead*) sender)->miCID;
	for(std::vector<ABHead*>::iterator it = mvHeads.begin(); it != mvHeads.end(); ++it){
		if((*it)->miCID != miCid) (*it)->onNormal();
	}


	InfoTab::sharedInfoTab()->showcharasav(miCid,this,menu_selector(ABasicLayer::PopupBack));
}

void ABasicLayer::Pause()
{
	setTouchEnabled(false);
	mAbNext->Pause();
	
}

void ABasicLayer::Close()
{
	setTouchEnabled(false);
	mAbNext->Pause();
}

void ABasicLayer::Resume()
{
	setTouchEnabled(true);
	mAbNext->Resume();
	for(std::vector<ABHead*>::iterator it = mvHeads.begin(); it != mvHeads.end(); ++it){
		(*it)->onNormal();
	}
}

void ABasicLayer::PopupBack(CCObject* sender)
{
	;
}

void ABasicLayer::SCBack(CCObject* sender)
{
	int tiTag = ((CCNode*) sender)->getTag();
	switch (tiTag)
	{
	case 1:
		ROOT_AB->miFlag	= 0x0010;
		GameManager::sharedGameManager()->goConfig();
		break;
	case 2:
		GameManager::sharedGameManager()->preConfig(0x1f,0x08,1);
		break;
	case 3:
		GameManager::sharedGameManager()->preConfig(0x1f,0x08,0);
		break;
	default:
		break;
	}
}


