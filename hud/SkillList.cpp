#include "hud/SkillList.h"

//////////////////////////////////////////////////////////////////////////
// SkillMeta - Basic
#include "Macros.h"
#include "DBUtil.h"

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

bool SkillList::setChara( Chara* ac )
{
	for(int i = 0;i<8;++i){
		m_viSkmL[i]->setVisible(false);
	}

	if(!ac) return false;
	if(m_pChara == ac) ;//return true;

	m_pChara = ac;

	vdata.clear();
	if(m_pChara->m_viSkills.size() == 0) return false;
	
	
	//CCDictionary* m_cid = new CCDictionary();
	string t_sMask;
	for(map<int,int>::iterator it = m_pChara->m_viSkills.begin(); it != m_pChara->m_viSkills.end(); ++it)
	{
		int t_id = it->second;
		t_sMask +=  CCString::createWithFormat("%d,",t_id)->getCString();
		//ItemCellData* t_icd	=	new ItemCellData(t_id,0,0);
		//m_cid->setObject(t_icd,t_id);
		//t_icd->autorelease();
	}
	t_sMask.erase(t_sMask.length()-1);
	CCString* t_csSql = CCString::createWithFormat("select * from skill_list where itemid IN (%s)",t_sMask.c_str());

	DBUtil::initDB("save.db");
	vdata = DBUtil::getDataInfo(t_csSql->getCString(),NULL);
	int m_number = vdata.size();
	DBUtil::closeDB(); 

	m_miiViDb.clear();
	for(int i = 0;i<m_number;i++){
		map<string,string> t_ssm = (map<string,string>) vdata.at(i);
		int item_id = stoi(t_ssm.at("itemid"));			//PrimaryKey:ItemID
		m_miiViDb[item_id] = i;							//方便查找

		CCLOG(">Read for item id:%d.", item_id);
	}

	RefreshSm();
	return true;
}

void SkillList::RefreshSm()
{
	int ti;
	for(int i = 0;i<m_miiViDb.size();++i){
		ti = m_pChara->m_viSkills[i];
		if(ti !=0 ){
			m_viSkmL[i]->setSKLabel(vdata.at(m_miiViDb[ti]).at("name").c_str());
			m_viSkmL[i]->setVisible(true);
		}else{
			m_viSkmL[i]->setVisible(false);
		}
	}
}

const char* SkillList::getval( const char* aname,int ait )
{
	return vdata.at(m_miiViDb[ait]).at(aname).c_str();
}

bool SkillList::setctns( Chara* ac, const char* sMask )
{
	for(int i = 0;i<8;++i){
		m_viSkmL[i]->setVisible(false);
	}

	if(!ac) return false;
	m_pChara = ac;

	vdata.clear();
	if(m_pChara->m_viSkills.size() == 0) return false;		// <只有装备了的技能才会被触发。

	std::string t_sMask;
	std::string t_ts = sMask;
	for(map<int,int>::iterator it = m_pChara->m_viSkills.begin(); it != m_pChara->m_viSkills.end(); ++it)
	{
		int t_id = it->second;
		if( t_ts.find(CCString::createWithFormat("%d",t_id)->getCString())<t_ts.length()){
			t_sMask += CCString::createWithFormat("%d,",t_id)->getCString();
		}
		
	}
	t_sMask.erase(t_sMask.length()-1);
	if(t_sMask.length() == 0) {
		CCLog("[CTN] None Skill Ctn avalible");
		return false;
	}
	CCString* t_csSql = CCString::createWithFormat("select * from skill_list where itemid IN (%s)",t_sMask.c_str());
	CCLog("[CTN] sql string:%s",t_csSql->getCString());

	DBUtil::initDB("save.db");
	vdata = DBUtil::getDataInfo(t_csSql->getCString(),NULL);
	int m_number = vdata.size();
	DBUtil::closeDB(); 

	m_miiViDb.clear();
	for(int i = 0;i<m_number;i++){
		map<string,string> t_ssm = (map<string,string>) vdata.at(i);
		int item_id = stoi(t_ssm.at("itemid"));			//PrimaryKey:ItemID
		m_miiViDb[item_id] = i;							//方便查找

		CCLOG(">Read for item id:%d.", item_id);
	}

	RefreshSm();
	return true;
}
