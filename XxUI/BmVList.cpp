#include "BmVList.h"
#include "battle\BEquip.h"
#include "packui\TOEquips.h"
#include "..\Corrode\Classes\DBUtil.h"

//////////////////////////////////////////////////////////////////////////
//Macros placing num value Chara
#define PLACE_BMFNT(p,m,n)												\
	lbf = CCLabelBMFont::create("",FNT_ENG_NUM,300);					\
	lbf->setAnchorPoint(CCPointZero);									\
	lbf->setPosition(ccp(m,n));											\
	lbf->setColor(ccc3(215,169,119));									\
	mSp_av->addChild(lbf);												\
	m_cdBmNum->setObject(lbf,p);

bool BmVList::init()
{
	do 
	{
		m_cdEquips	= NULL;
		m_cdBmNum	= NULL;

		mSp_av = CCSprite::create("Images/ui_tab_chara_av.png");
		mSp_av->setAnchorPoint(CCPointZero);
		mSp_av->setPosition(ccp(0,0));
		addChild(mSp_av,13);

		CCSprite* m_SpFrame = CCSprite::create("Images/ui_tab_chara_ba.png");
		m_SpFrame->setAnchorPoint(CCPointZero);
		m_SpFrame->setTextureRect(CCRectMake(0,0,450,600));
		m_SpFrame->setPosition(ccp(0,0));
		addChild(m_SpFrame,-1);

		InitBmNums();

		return true;
	} while (false);
	return false;

}

void BmVList::InitBmNums()
{
	m_cdBmNum = new CCDictionary();
	CCLabelBMFont* lbf = NULL;

	int heit	= 36;
	int dheit	= 30;

	//////////////////////////////////////////////////////////////////////////
	PLACE_BMFNT("wlw"	, 125	, heit);
	PLACE_BMFNT("hx"	, 235	, heit);
	heit += (dheit + 2);

	PLACE_BMFNT("mz"	, 125	, heit);
	PLACE_BMFNT("hb"	, 235	, heit);
	//////////////////////////////////////////////////////////////////////////
	heit = 290;

	PLACE_BMFNT("mov"	,120	, heit);
	PLACE_BMFNT("sta"	,245	, heit);
	heit += dheit;

	PLACE_BMFNT("spd"	,120	, heit);
	PLACE_BMFNT("luk"	,245	, heit);
	heit += dheit;

	PLACE_BMFNT("hit"	,120	, heit);
	PLACE_BMFNT("avg"	,245	, heit);
	heit += dheit;

	PLACE_BMFNT("mag"	,120	, heit);
	PLACE_BMFNT("rst"	,245	, heit);
	heit += dheit;

	PLACE_BMFNT("atk"	,120	, heit);
	PLACE_BMFNT("def"	,245	, heit);
	heit += (dheit + 3);
	//////////////////////////////////////////////////////////////////////////
	PLACE_BMFNT("sou"	,140	,heit);
	heit += dheit;

	PLACE_BMFNT("hp"	,110	,heit);
	heit += dheit;

	PLACE_BMFNT("lv"	,110	,heit);
	PLACE_BMFNT("exp"	,250	,heit);
	//////////////////////////////////////////////////////////////////////////
	clt = CCLabelTTF::create("",FNT_UI_TTF , 29);
	clt->setAnchorPoint(CCPointZero);
	clt->setPosition(ccp(70,540));
	clt->setColor(ccc3(215,169,119));
	mSp_av->addChild(clt);
	//////////////////////////////////////////////////////////////////////////
	eq_mb = new BYLayerDescendant();
	eq_mb->autorelease();
	eq_mb->setAnchorPoint(ccp(0,0));
	eq_mb->setPosition(ccp(0,1));
	eq_mb->setContentSize(CCSizeMake(500,250));
	mSp_av->addChild(eq_mb);

	heit = 220;
	TOEquips* t_toe;
	for(int i=0;i<5;i++){
		t_toe = new TOEquips();
		t_toe->setAnchorPoint(CCPointZero);
		t_toe->setPosition(ccp(45,heit));
		t_toe->setTag(i);
		//t_toe->setactivator(this,menu_selector(TOChara::EquipClick));
		t_toe->setEnability(false);
		m_vEBtns.push_back(t_toe);
		eq_mb->addChild(t_toe);
		//t_toe->autorelease();

		heit -= 30;
	}
}

void BmVList::ShowContents()
{
	clt->setString(g_chara->m_sName.c_str());

	// <ÊôÐÔ
	CCDictElement*	 pCde = NULL;
	string			 t_name;
	CCLabelBMFont*	 t_clbm = NULL;

	CCDICT_FOREACH(m_cdBmNum,pCde){
		t_name = pCde->getStrKey();
		t_clbm = (CCLabelBMFont*) pCde->getObject();

		t_clbm->setString(CCString::createWithFormat("%d",g_chara->getvalue(t_name))->getCString());
	}

	// Equipment
	Equip* t_e = NULL;
	for(int i = 0;i<5;++i){
		t_e = (Equip*) m_cdEquips->objectForKey(g_chara->m_miiEquips[i]);
		if(t_e){
			t_e->lock = g_chara->m_viiELock[i];
			t_e->sum  = g_chara->m_viiESum[i];
		}

		m_vEBtns[i]->setcontent(t_e);
		m_vEBtns[i]->setEnability(false);
	}
}

void BmVList::setChara(Chara* ac)
{
	g_chara = ac;

	// <EqÊý¾Ý
	CC_SAFE_RELEASE_NULL(m_cdEquips);
	m_cdEquips = new CCDictionary();
	string t_sMask;
	vector<map<string,string>> vdata;
	if(g_chara->m_miiEquips.size() > 0) {
		for(map<int,int>::iterator it = g_chara->m_miiEquips.begin(); it != g_chara->m_miiEquips.end(); ++it){
			t_sMask +=  CCString::createWithFormat("%d,",it->second)->getCString();
		}
		t_sMask.erase(t_sMask.length()-1);

		CCString* t_csSql = CCString::createWithFormat("select * from equip_list where itemid IN (%s)",t_sMask.c_str());
		DBUtil::initDB("save.db");
		vdata = DBUtil::getDataInfo(t_csSql->getCString(),NULL);

		int m_number = vdata.size();
		for(int i = 0;i<m_number;i++){
			map<string,string> t_ssm = (map<string,string>) vdata.at(i);
			int item_id = stoi(t_ssm.at("itemid"));			//PrimaryKey:ItemID
			Equip* t_eq = new Equip();

			t_eq->id		=	item_id;
			t_eq->effect	=	t_ssm.at("effect");
			t_eq->name		=	t_ssm.at("name");
			t_eq->discript	=	t_ssm.at("discription");
			t_eq->position	=	stoi(t_ssm.at("position"));

			m_cdEquips->setObject(t_eq,item_id);
			t_eq->release();

			CCLOG(">Read for dquip with itemid:%d.", item_id);	
		}

		DBUtil::closeDB(); 
	}

	ShowContents();
}

BmVList::~BmVList()
{
	CC_SAFE_RELEASE_NULL(m_cdEquips);
	CC_SAFE_RELEASE_NULL(m_cdBmNum);
}

