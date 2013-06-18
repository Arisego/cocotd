#include "packui/TOChara.h"
#include "Macros.h"
#include "EventCenter.h"
#include "DBUtil.h"



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

TOChara::TOChara( int a_iCharaID,CCObject* target, SEL_MenuHandler selector )
{
	EventCenter::sharedEventCenter()->SetCharaTab(this);

	m_iCharaID	= -3;
	m_iPage		= 0;
	mSpStand	= NULL;
	m_cdEquips	= NULL;
	m_Topop		= NULL;
	m_iCurrentEuip	= -2;

	CCSprite* m_SpBack = CCSprite::create("Images/ui_tab_chara_ba.png");
	m_SpBack->setAnchorPoint(CCPointZero);
	m_SpBack->setPosition(ccp(0,0));
	m_SpBack->setOpacity(230);
	addChild(m_SpBack,-1);

	mSp_av = CCSprite::create("Images/ui_tab_chara_av.png");
	mSp_av->setAnchorPoint(CCPointZero);
	mSp_av->setPosition(ccp(403,0));
	mSp_av->setVisible(false);
	addChild(mSp_av,13);

	mSp_sa = CCSprite::create("Images/ui_tab_chara_sa.png");
	mSp_sa->setAnchorPoint(CCPointZero);
	mSp_sa->setPosition(ccp(0,0));
	mSp_sa->setVisible(false);
	addChild(mSp_sa);

	InitBmNums();

	CCSprite* m_SpFrame = CCSprite::create("Images/ui_tab_chara_bg.png");
	m_SpFrame->setAnchorPoint(CCPointZero);
	m_SpFrame->setPosition(ccp(0,0));
	addChild(m_SpFrame,14);

	setactivator(target,selector);

	RefreshData(a_iCharaID);
	RefreshView(1);
}

TOChara::~TOChara()
{
	CC_SAFE_RELEASE_NULL(m_cdBmNum);
	CC_SAFE_RELEASE_NULL(m_cdEquips);

	EventCenter::sharedEventCenter()->SetCharaTab(NULL);
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
	case 2:
		mSp_av->setVisible(false);
		mSp_sa->setVisible(true);
		ShowSa();
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

	// <Eq数据
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
}

/* <所有的控件将会被更新,注意添加更新保护以减少操作次数 */
void TOChara::ShowAV()
{

	f_refresh_cur_data();
	clt->setString(g_chara->m_sName.c_str());
	
	CC_SAFE_RELEASE_NULL(mSpStand);
	mSpStand = CCSprite::create(CCString::createWithFormat("Images/stand_%d.png",g_chara->m_iCharaID)->getCString());
	mSpStand->setAnchorPoint(CCPointZero);
	mSpStand->setPosition(ccp(50,0));
	addChild(mSpStand,12);

	//////////////////////////////////////////////////////////////////////////
	// Equipment
	Equip* t_e = NULL;
	for(int i = 0;i<5;++i){
		t_e = (Equip*) m_cdEquips->objectForKey(g_chara->m_miiEquips[i]);
		if(t_e){
			t_e->lock = g_chara->m_viiELock[i];
			t_e->sum  = g_chara->m_viiESum[i];
		}

		m_vEBtns[i]->setcontent(t_e);
	}
	

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
		t_toe->setactivator(this,menu_selector(TOChara::EquipClick));
		m_vEBtns.push_back(t_toe);
		eq_mb->addChild(t_toe);
		//t_toe->autorelease();

		heit -= 30;
	}
}

void TOChara::EquipClick( CCObject* pSender )
{
	if(RefreshEquip(((CCNode*) pSender)->getTag()))
		EquipPop();
}

bool TOChara::RefreshEquip( int anewi )
{
	if(anewi == m_iCurrentEuip) return false;
	m_iCurrentEuip = anewi;
	for(int i = 0;i<5;++i){
		if(i!=m_iCurrentEuip) m_vEBtns[i]->onNormal();
		else m_vEBtns[i]->onSelect();
	}	
	return true;
}

void TOChara::EquipPop()
{
	m_Topop = new TOPopup(305,235);
	m_Topop->autorelease();
	m_Topop->refresh_ldb(m_iCurrentEuip);
	m_Topop->setAnchorPoint(ccp(0,0));
	m_Topop->setPosition(ccp(130,25));
	m_Topop->setactivator(this,menu_selector(TOChara::EquipChange));
	addChild(m_Topop,20);
	eq_mb->m_bIsEnabled = false;

}
//////////////////////////////////////////////////////////////////////////

void TOChara::ShowSa()
{
	;
}



//////////////////////////////////////////////////////////////////////////
// <事件处理
void TOChara::s_press()
{
	if(m_iCurrentEuip<0) m_iCurrentEuip = 4;
	RefreshEquip((m_iCurrentEuip+1)%5);
}

void TOChara::w_press()
{
	if(m_iCurrentEuip<0) m_iCurrentEuip = 5;
	RefreshEquip((m_iCurrentEuip+4)%5);

}

void TOChara::EquipChange( CCObject* pSender )
{
	int titag = ((CCNode*) pSender)->getTag();
	switch (titag)
	{
	case -4:			// <卸掉装备
		break;
	default:			// <切换
		ItemCellData* ticd = (ItemCellData*) StateCenter::sharedStateCenter()->f_get_itemlist(4)->objectForKey(titag);
		TOEquips* ttoec = m_vEBtns[m_iCurrentEuip];
		
		//Tye to get the equip from m_cdEqups,if none,generate one.How ever the data should be flush into g_chara_m_iis.
		int t_newid = ticd->type_id;
		Equip* t_e = (Equip*) m_cdEquips->objectForKey(t_newid);
		if(!t_e){
			t_e = new Equip();
			m_Topop->inform(t_e);
		}
		//t_e->id = t_newid;
		t_e->lock = ticd->lock;
		t_e->sum = ticd->sum;

		g_chara->calDiffer(ttoec->eq->effect,m_Topop->m_sEffect);
		g_chara->aplyDiffer();

		ticd->type_id	= ttoec->eq->id;
		ticd->lock		= ttoec->eq->lock;
		ticd->sum		= ttoec->eq->sum;

		f_refresh_cur_data();
			
		g_chara->m_miiEquips[m_iCurrentEuip] = t_e->id;
		g_chara->m_viiELock[m_iCurrentEuip] = t_e->lock;
		g_chara->m_viiESum[m_iCurrentEuip] = t_e->sum;
		m_vEBtns[m_iCurrentEuip]->setcontent(t_e);





		//StateCenter::sharedStateCenter()->f_insert_item(titag,4,);
		break;
	}
}

void TOChara::f_refresh_cur_data()
{
	CCDictElement*	 pCde = NULL;
	string			 t_name;
	CCLabelBMFont*	 t_clbm = NULL;

	CCDICT_FOREACH(m_cdBmNum,pCde){
		t_name = pCde->getStrKey();
		t_clbm = (CCLabelBMFont*) pCde->getObject();

		t_clbm->setString(CCString::createWithFormat("%d",g_chara->getvalue(t_name))->getCString());
	}
}

