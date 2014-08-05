#include "Ab_asicLayer.h"
#include "packui\CharaS.h"
#include "Ab_Head.h"
#include "..\Corrode\Classes\sublayer\InfoTab.h"

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

		float tPosY = 400;
		float tPoxX = 10;

		CCDictionary* tcda = CharaS::sharedCharaS()->m_caCharas;
		CCDictElement* tcde;
		CCDICT_FOREACH(tcda, tcde){
			Chara* tca = (Chara*) tcde->getObject();
			ABHead* tabh = new ABHead(tca->m_iCharaID, 200, 100);
			tabh->setAnchorPoint(CCPointZero);
			tabh->setPosition(tPoxX,tPosY);
			tabh->setactivator(this, menu_selector(ABasicLayer::HdBack));
			mvHeads.push_back(tabh);
			tabh->setScale(0.8);
			addChild(tabh,0);
			tPosY -= 90;
		}

		CCSize os = CCDirector::sharedDirector()->getVisibleSize();
		setContentSize(os);
		//RegisteTou
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
}

void ABasicLayer::Close()
{
	setTouchEnabled(false);
}

void ABasicLayer::Resume()
{
	setTouchEnabled(true);
	for(std::vector<ABHead*>::iterator it = mvHeads.begin(); it != mvHeads.end(); ++it){
		(*it)->onNormal();
	}
}

void ABasicLayer::PopupBack(CCObject* sender)
{
	;
}


