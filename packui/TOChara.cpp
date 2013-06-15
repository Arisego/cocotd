#include "packui/TOChara.h"
#include "Macros.h"

//////////////////////////////////////////////////////////////////////////
//Macros placing num value Chara
#define PLACE_BMFNT(p,m,n)												\
	lbf = CCLabelBMFont::create("",FNT_ENG_NUM,300);					\
	lbf->setAnchorPoint(CCPointZero);									\
	lbf->setPosition(ccp(m,n));											\
	lbf->setColor(ccc3(215,169,119));									\
	mSp_av->addChild(lbf);												\
	m_cdBmNum->setObject(lbf,p);

//////////////////////////////////////////////////////////////////////////

void TOChara::activate( CCObject* pSender )
{
	if (m_pListener && m_pfnSelector)
	{
		(m_pListener->*m_pfnSelector)(pSender);
	}
}

TOChara::TOChara( int a_iCharaID,CCObject* target, SEL_MenuHandler selector )
{
	m_iCharaID	= -1;
	m_iPage		= 0;
	mSpStand	= NULL;

	CCSprite* m_SpBack = CCSprite::create("Images/ui_tab_chara_ba.png");
	m_SpBack->setAnchorPoint(CCPointZero);
	m_SpBack->setPosition(ccp(0,0));
	m_SpBack->setOpacity(230);
	addChild(m_SpBack,-1);

	mSp_av = CCSprite::create("Images/ui_tab_chara_av.png");
	mSp_av->setAnchorPoint(CCPointZero);
	mSp_av->setPosition(ccp(403,0));
	mSp_av->setVisible(false);
	addChild(mSp_av);

	mSp_sa = CCSprite::create("Images/ui_tab_chara_sa.png");
	mSp_sa->setAnchorPoint(CCPointZero);
	mSp_sa->setPosition(ccp(80,0));
	mSp_sa->setVisible(false);
	addChild(mSp_sa);

	InitBmNums();

	CCSprite* m_SpFrame = CCSprite::create("Images/ui_tab_chara_bg.png");
	m_SpFrame->setAnchorPoint(CCPointZero);
	m_SpFrame->setPosition(ccp(0,0));
	addChild(m_SpFrame,11);

	RefreshData(a_iCharaID);
	RefreshView(1);
}

TOChara::~TOChara()
{
	CC_SAFE_RELEASE_NULL(m_cdBmNum);
}

void TOChara::RefreshView(int Page)
{
	if(m_iPage == Page) return;
	m_iPage = Page;

	switch (m_iPage)
	{
	case 1:
		mSp_av->setVisible(true);
		mSp_sa->setVisible(false);	
		ShowAV();
		break;
	default:
		CCLOG(">[TOChara]Error m_iPage value:%d",m_iPage);
		break;
	}
}

void TOChara::RefreshData( int Id )
{
	if(m_iCharaID == Id) return;
	CC_SAFE_RELEASE_NULL(mSpStand);
	m_iCharaID = Id;
	m_iPage = 0;

	g_chara = CharaS::sharedCharaS()->getchara(m_iCharaID);
}

void TOChara::ShowAV()
{
	CCDictElement*	 pCde = NULL;
	string			 t_name;
	CCLabelBMFont*	 t_clbm = NULL;

	CCDICT_FOREACH(m_cdBmNum,pCde){
		t_name = pCde->getStrKey();
		t_clbm = (CCLabelBMFont*) pCde->getObject();

		t_clbm->setString(CCString::createWithFormat("%d",g_chara->getvalue(t_name))->getCString());
	}

	clt->setString(g_chara->m_sName.c_str());
	
	CC_SAFE_RELEASE_NULL(mSpStand);
	mSpStand = CCSprite::create(CCString::createWithFormat("Images/stand_%d.png",g_chara->m_iCharaID)->getCString());
	mSpStand->setAnchorPoint(CCPointZero);
	mSpStand->setPosition(ccp(0,0));
	addChild(mSpStand,12);

}

void TOChara::InitBmNums()
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
}

