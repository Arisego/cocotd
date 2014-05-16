#include "hud/SkillList.h"

//////////////////////////////////////////////////////////////////////////
// SkillMeta - Basic
#include "Macros.h"
#include "DBUtil.h"
#include "GameManager.h"

static const char* tpath[] = {"skill_w.png","skill_s.png","skill_a.png","skill_d.png","skill_i.png","skill_k.png","skill_j.png","skill_l.png"};

SkillListMeta::SkillListMeta()
{
	mLT_Name = CCLabelTTF::create("",FNT_UI_TTF,15);
	mLT_Name->setAnchorPoint(ccp(0,0));
	mLT_Name->setPosition(ccp(37,2));
	mLT_Name->setColor(ccRED);
	addChild(mLT_Name,1);

	spriteNormal	= CCSprite::createWithSpriteFrameName("ui_tab_chara_sa_btn.png");
	spriteNormal->setAnchorPoint(CCPointZero);
	spriteNormal->setPosition(ccp(0,0));
	spriteNormal->setVisible(false);
	addChild(spriteNormal);

	spriteHover		= CCSprite::createWithSpriteFrameName("ui_tab_chara_sa_btnhover.png");
	spriteHover->setAnchorPoint(CCPointZero);
	spriteHover->setPosition(ccp(0,0));
	spriteHover->setVisible(false);
	addChild(spriteHover);

	spriteDisable	= NULL;

	//spriteSelected	= CCSprite::create("Images/ui_tab_chara_sa_button.png");
	//spriteSelected->setAnchorPoint(CCPointZero);
	//spriteSelected->setPosition(ccp(0,0));
	//spriteSelected->setVisible(false);
	//addChild(spriteSelected);
	spriteSelected = NULL;

	labelAtlas = CCLabelBMFont::create("", FNT_CHN);
	labelAtlas->setAnchorPoint(CCPointZero);
	labelAtlas->setPosition(ccp(0,0));
	labelAtlas->setColor(COLOUR_NORMAL);
	this->addChild(labelAtlas,1);

	m_obContentSize.width = 136;
	m_obContentSize.height = 20;

	m_bIsEnabled = true;
	scheduleUpdate();
}

void SkillListMeta::setSKLabel( const char* aname )
{
	mLT_Name->setString(aname);
}

void SkillListMeta::setValue( int ai )
{
	CCSprite* sv	= CCSprite::createWithSpriteFrameName(tpath[ai]);
	sv->setScale(0.7);
	sv->setAnchorPoint(CCPointZero);
	sv->setPosition(ccp(0,0));
	//sv->setVisible(false);
	addChild(sv,8);
}

void SkillListMeta::onSelect()
{
	if(C_STATE_SELECT == m_iState || !m_bIsEnabled) return;
	m_iState = C_STATE_SELECT;
	labelAtlas->setColor(COLOUR_SELECT);

	SpriteRefresh();
	onNormal();
	m_iState = C_STATE_HOVERD;
}


//////////////////////////////////////////////////////////////////////////
#define LINKPATH "Pac/skill_list_left"

bool SkillList::f_init()
{
	do 
	{
		CCSpriteFrameCache *cache = CCSpriteFrameCache::sharedSpriteFrameCache();
		CCSpriteBatchNode *sheet = CCSpriteBatchNode::create(CCString::createWithFormat("%s.png",LINKPATH)->getCString());
		cache->addSpriteFramesWithFile(CCString::createWithFormat("%s.plist",LINKPATH)->getCString());

		float ny = 0;
		SkillListMeta* tsm = NULL;
		for(int i = 0;i<8;++i){
			tsm = new SkillListMeta();
			tsm->onNormal();
			tsm->setValue(i);
			tsm->setTag(i);
			//tsm->setSKLabel(CCString::createWithFormat("SKILLTEST-%d",i)->getCString());
			tsm->setAnchorPoint(CCPointZero);
			tsm->setPosition(ccp(0,ny));
			tsm->setactivator(this,menu_selector(SkillList::SKM_Back));
			addChild(tsm,1);
			m_viSkmL.push_back(tsm);
			ny -= 30;
		}

		//addChild(sheet);

		
		return true;
	} while (0);
	return false;
}

void SkillList::setactivator( CCObject* target, SEL_MenuHandler selector )
{
	m_pListener = target;
	m_pfnSelector = selector;
	m_bIsEnabled = true;
}


void SkillList::activate()
{
	if (m_bIsEnabled)
	{
		if (m_pListener && m_pfnSelector)
		{
			(m_pListener->*m_pfnSelector)(this);
		}
	}
}

void SkillList::SKM_Back( CCObject* to )
{
	int ti = ((CCNode*) to)->getTag();
	ti = m_pChara->m_viSkills[ti];
	this->setTag(ti);
	CCLog(">[SKM_Back]:%d",ti);
	activate();
	
}

bool SkillList::setChara( Chara* ac, int aiFlag )
{
	m_siSkillEn.clear();
	for(int i = 0;i<8;++i){
		m_viSkmL[i]->setVisible(false);
	}

	if(!ac) return false;
	miFlag = aiFlag;
	//if(m_pChara == ac) ;//return true;

	m_pChara = ac;

	vdata.clear();
	if(m_pChara->m_viSkills.size() == 0) return false;

	RefreshSm();
	return true;
}

void SkillList::RefreshSm()
{
	int ti;
	CCLog(">[SkillList] RefreshSm()");
	for(int i = 0;i<8;++i){
		
		ti = m_pChara->m_viSkills[i];
		if(m_siSkillEn.count(ti) == 0 && m_siSkillEn.size()>0) continue;			//	<不在使用列表中的技能将会被禁用

		if(ti !=0 ){
			if( !(m_pChara->mSkillList->getSkill(ti)->miPhase & miFlag) ) continue;
			m_viSkmL[i]->setSKLabel(m_pChara->mSkillList->getSkill(ti)->msName.c_str());
			m_viSkmL[i]->setVisible(true);
			m_viSkmL[i]->onNormal();
			m_viSkmL[i]->SpriteRefresh();
			CCLog(">[SkillList] RefreshSm() | ID: %d", i);
		}else{
			m_viSkmL[i]->setVisible(false);
		}
	}
}

BSkill* SkillList::getval( int ait )
{
	return m_pChara->mSkillList->getSkill(ait);
}

bool SkillList::setctns( Chara* ac, const char* sMask )
{
	m_siSkillEn.clear();
	for(int i = 0;i<8;++i){
		m_viSkmL[i]->setVisible(false);
	}
	if(strlen(sMask) == 1) {
		CCLog(">[SkillList] Trick. One word mask to normal");
		return setChara(ac);
	}

	if(!ac) return false;
	m_pChara = ac;

	vdata.clear();
	if(m_pChara->m_viSkills.size() == 0) return false;		// <只有装备了的技能才会被触发。

	std::string t_ts = sMask;
	for(map<int,int>::iterator it = m_pChara->m_viSkills.begin(); it != m_pChara->m_viSkills.end(); ++it)
	{
		int t_id = it->second;
		if( t_ts.find(CCString::createWithFormat("%d",t_id)->getCString())<t_ts.length()){
			m_siSkillEn.insert(t_id);
			CCLog(">[SkillList] Ctn:%d",t_id);
		}
		
	}

	if(m_siSkillEn.size() == 0) return false;
	RefreshSm();
	return true;
}

void SkillList::randomact()
{
	runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.0), CCCallFunc::create( this,callfunc_selector(SkillList::randomactfc))));
}

void SkillList::randomactfc()
{
	setVisible(true);
	
	int ti = m_siSkillEn.size();
	bool rt = (CCRANDOM_0_1() < 0.35);

	if(rt){
		
		float tf = floor(ti * (CCRANDOM_0_1()));
		ti = tf;
		//ti = 5;	// <调试
		CCLog(">[SkillList] randomactfc() | ti:%d", ti);

		for(int i = 0; i<8; ++i){
			if(m_viSkmL[i]->isVisible()){
				--ti;
				if(ti<=0){
					m_viSkmL[i]->activate();
					break;
				}
			}
		}

		if(GameManager::sharedLogicCenter()->ml->mbIsDirect){
			GameManager::sharedLogicCenter()->ml->right_click();
			GameManager::sharedLogicCenter()->ml->right_click();
		}
		
	}else{
		GameManager::sharedLogicCenter()->ml->right_click();
	}
}
