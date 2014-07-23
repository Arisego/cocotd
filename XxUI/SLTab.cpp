#include "SLTab.h"

#include <fstream>
#include <iostream>

#define SL_WIDTH 370
#define SL_HEIGHT 62
//////////////////////////////////////////////////////////////////////////
// SLCell

SLCell::SLCell()
{
	Container::Container();

	miSlState = 0;

	spriteNormal	= NULL;
	spriteDisable	= NULL;
	spriteSelected	= NULL;
	spriteHover		= CCScale9Sprite::create("Images/config_back.png"); 
	spriteHover->setContentSize(CCSizeMake(SL_WIDTH, SL_HEIGHT));
	spriteHover->setAnchorPoint(ccp(0,0));
	((CCScale9Sprite*) spriteHover)->setOpacity(88);
	addChild(spriteHover,-1);

	labelAtlas = CCLabelBMFont::create("E M P T Y", FNT_CHN);
	labelAtlas->setScale(1.8);
	labelAtlas->setAnchorPoint(ccp(0.5,0.5));
	labelAtlas->setPosition(SL_WIDTH/2, SL_HEIGHT/2 - 3);
	addChild(labelAtlas);

	mSpCellBg = CCSprite::create("Images/UI/sl_cell.png");
	mSpCellBg->setAnchorPoint(CCPointZero);
	mSpCellBg->setPosition(ccp(0,0));
	mSpCellBg->setVisible(false);
	addChild(mSpCellBg);
	
	mLbTIme = CCLabelBMFont::create("",FNT_CHN);
	mLbTIme->setPosition(ccp(240,36));
	mLbTIme->setScale(0.6);
	mLbTIme->setAnchorPoint(CCPointZero);
	mSpCellBg->addChild(mLbTIme);


	m_obContentSize.width = SL_WIDTH;
	m_obContentSize.height = SL_HEIGHT;

	m_iState = C_STATE_NORMAL;
	scheduleUpdate();
}

void SLCell::initWithJSon(Json::Value ajv)
{
	ChangeState(1);
	mLbTIme->setString(ajv["time"].asCString());
}

void SLCell::ChangeState(int ai)
{
	
	switch (ai)
	{
	case(0):
		labelAtlas->setVisible(true);
		break;
	case(1):
		labelAtlas->setVisible(false);
		mSpCellBg->setVisible(true);
		break;
	}
	miSlState = ai;
}

void SLCell::update(float fDelta)
{
	if(mLasto && m_iState != C_STATE_SELECT){
		if(checkTouch(mLasto)){
			onHover();
		}else{
			onNormal();
			CC_SAFE_RELEASE_NULL(mLasto);
			labelAtlas->setColor(COLOUR_NORMAL);
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// SLTab

SLTab::SLTab()
{
	iSet.clear();
	m_iCurTab = 0;
	m_oLockSave = NULL;

	m_vSLs.assign(3,NULL);

	cm = ConfigManager::sharedConfigManager();
	mTabName = new Container();
	mTabName->initwithPsz("Images/tab_head.png",cm->GetConfigS("save").c_str(),100,30,this,menu_selector(SLTab::buttonback));
	mTabName->setAnchorPoint(ccp(0,0));
	mTabName->setPosition(ccp(0,500));
	mTabName->setEnability(false);
	addChild(mTabName,1,0);

	//////////////////////////////////////////////////////////////////////////
	/* <SL Cells */
	mb = new BYLayerDescendant();
	mb->autorelease();
	mb->setAnchorPoint(ccp(0,0));
	mb->setPosition(ccp(130,168)); 
	addChild(mb,1);

	CCSprite* tcBack = CCSprite::create("Images/UI/sl_back.png");
	tcBack->setAnchorPoint(CCPointZero);
	tcBack->setPosition(CCPointZero);
	mb->addChild(tcBack,-1);

	m_fSWidth = 0;
	m_fSHeight = 128;
	for(int i = 0; i < 3; ++i){
		Add_Button(i);
	}

}

SLTab::~SLTab(){
	CCLog(">[SLTab] Deco~");
}

void SLTab::CleanStates(){
	removeAllChildren();
}

void SLTab::selqueryback(CCObject* pSender)
{
	int itag = ((Container *) pSender)->getTag();
	CCLog(">[SLTab] selqueryback()| Tag:%d",itag);
	if(itag == 1)
		savetofile();
}

void SLTab::savetofile(){
	if(!m_oLockSave){
		CCLog(">[SLTab] selqueryback()|Invalid Save Object.");
		return;
	}
	int t_t = ((Container *) m_oLockSave)->getTag();

	StateCenter::sharedStateCenter()->f_save_file(CCString::createWithFormat("save_%d",t_t)->getCString());		
	((Container *) m_oLockSave)->removeFromParent();
	m_oLockSave = NULL;
	Add_Button(t_t);
}

void SLTab::buttonback(CCObject* sender){
	int itag = ((Container *) sender)->getTag();
	switch(m_iCurTab){
	case 0:
		{
			CC_SAFE_RELEASE_NULL(m_oLockSave);
			m_oLockSave = sender;

			if(iSet.count(itag) == 0){
				savetofile();
			}else{
				vector<string> sl;
				sl.push_back("yes");
				sl.push_back("no");

				InfoTab::sharedInfoTab()->showselect("pre_rewrite", sl, this, menu_selector(SLTab::selqueryback));
			}
			break;
		}
	case 1:
		{
			StateCenter::sharedStateCenter()->f_load_file(CCString::createWithFormat("save_%d",itag)->getCString());
			break;
		}
	}
	CCLog(">[SLTab] sl:%d",itag);
}

void SLTab::ShowTab(int itab){
	m_iCurTab = itab;
	switch(m_iCurTab){
	case 0:
		CCLOG("Save Tab Showing.");
		mTabName->labelAtlas->setString(cm->GetConfigS("save").c_str());
		break;
	case 1:
		mTabName->labelAtlas->setString(cm->GetConfigS("load").c_str());
		break;
	}
	mTabName->setEnability(false);

}

void SLTab::Add_Button(int tag){
	SLCell* tabc = new SLCell();

	CCString* fullPath = CCString::createWithFormat("save/save_%d.tos",tag);
	bool tbSave = CCFileUtils::sharedFileUtils()->isFileExist(fullPath->getCString());

	tabc->setPosition(ccp(m_fSWidth,m_fSHeight - tag*(SL_HEIGHT+2)));
	tabc->setAnchorPoint(CCPointZero);
	tabc->setactivator(this, menu_selector(SLTab::buttonback));
	tabc->setTag(tag);

	if(tbSave){
		ifstream b_file(fullPath->getCString());
		std::stringstream buffer;  
		buffer << b_file.rdbuf();  
		std::string contents(buffer.str()); 
		CCLog(">[SLTab] Get the tos:%s", contents.c_str());

		Json::Value		tjs_Save;
		Json::Reader	tjsReader;
		if(tjsReader.parse(contents,tjs_Save)){
			tabc->initWithJSon(tjs_Save);
		}
	}

	mb->addChild(tabc);
	m_vSLs[tag] = tabc;

}