#include "hud/SkillList.h"

//////////////////////////////////////////////////////////////////////////
// SkillMeta - Basic
#include "Macros.h"

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
	m_obContentSize.height = 40;

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
			tsm->setSKLabel(CCString::createWithFormat("SKILLTEST-%d",i)->getCString());
			tsm->setAnchorPoint(CCPointZero);
			tsm->setPosition(ccp(0,ny));
			tsm->setactivator(this,menu_selector(SkillList::SKM_Back));
			addChild(tsm,1);
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
	CCLog(">[SKM_Back]:%d",((CCNode*) to)->getTag());
}
