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

#define PLACE_TOSKILL(w,h,g,i)											\
	t_toe = new TOSkill();												\
	t_toe->setAnchorPoint(CCPointZero);									\
	t_toe->setPosition(ccp(w,h));										\
	t_toe->setTag(i);													\
	t_toe->setactivator(this,menu_selector(TOChara::SkillClick));		\
	g.push_back(t_toe);													\
	sa_mb->addChild(t_toe);

//////////////////////////////////////////////////////////////////////////

TOChara::TOChara( int a_iCharaID,CCObject* target, SEL_MenuHandler selector )
{
	EventCenter::sharedEventCenter()->SetCharaTab(this);

	m_iCharaID	= -3;
	m_iPage		= 0;
	mSpStand	= NULL;
	m_cdEquips	= NULL;
	m_cdSkills	= NULL;
	m_cdPopSkils = NULL;
	m_bPopup	= false;
	
	m_Topop = new TOPopup(305,235);
	m_Topop->autorelease();
	m_Topop->setAnchorPoint(ccp(0,0));
	m_Topop->setPosition(ccp(130,25));
	m_Topop->setactivator(this,menu_selector(TOChara::TOPopBack));
	m_Topop->clear();
	addChild(m_Topop,20);

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
	InitSaBtns();

	CCSprite* m_SpFrame = CCSprite::create("Images/ui_tab_chara_bg.png");
	m_SpFrame->setAnchorPoint(CCPointZero);
	m_SpFrame->setPosition(ccp(0,0));
	addChild(m_SpFrame,14);

	setactivator(target,selector);

	RefreshData(a_iCharaID);
	RefreshView(2);
}

TOChara::~TOChara()
{
	CC_SAFE_RELEASE_NULL(m_cdBmNum);
	CC_SAFE_RELEASE_NULL(m_cdEquips);
	CC_SAFE_RELEASE_NULL(m_cdSkills);
	CC_SAFE_RELEASE_NULL(m_cdPopSkils);

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

	// <Eq����
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
	/* <��ȡ�������� */
	CC_SAFE_RELEASE_NULL(m_cdSkills);
	m_cdSkills = new CCDictionary();
	if(g_chara->m_viSkills.size()>0)
	{
		string t_sMask;
		int t_it;
		for(map<int,int>::iterator it = g_chara->m_viSkills.begin(); it != g_chara->m_viSkills.end(); ++it)
		{
			t_it = it->second;
			t_sMask +=  CCString::createWithFormat("%d,",t_it)->getCString();
		}
		t_sMask.erase(t_sMask.length()-1);
		CCString* t_csSql = CCString::createWithFormat("select * from skill_list where itemid IN (%s)",t_sMask.c_str());
		vector<map<string,string>> vdata;
		DBUtil::initDB("save.db");
		vdata = DBUtil::getDataInfo(t_csSql->getCString(),NULL);

		int m_number = vdata.size();
		for(int i = 0;i<m_number;i++){
			map<string,string> t_ssm = (map<string,string>) vdata.at(i);
			int sk_id = stoi(t_ssm.at("itemid"));			//PrimaryKey:ItemID
			SkillMeta* t_sk = new SkillMeta();

			t_sk->type_id		=	sk_id;
			t_sk->name		=	t_ssm.at("name");
			t_sk->discription	=	t_ssm.at("discription");
			t_sk->zbtype		=	stoi(t_ssm.at("usecase"));

			m_cdSkills->setObject(t_sk,sk_id);
			t_sk->release();

			CCLOG(">Read for Skill with id:%d.", sk_id);	
		}

		DBUtil::closeDB(); 
	}
}

/* <���еĿؼ����ᱻ����,ע����Ӹ��±����Լ��ٲ������� */
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
	if(m_Topop->refresh_ldb(m_iCurrentEuip)){
		eq_mb->m_bIsEnabled = false;
		m_Topop->setVisible(true);
		m_bPopup = true;
	}

}

//////////////////////////////////////////////////////////////////////////
// <�¼�����
void TOChara::s_press()
{
	if(m_bPopup){
		m_Topop->s_press();
	}else{
		if(m_iCurrentEuip<0) m_iCurrentEuip = 4;
		RefreshEquip((m_iCurrentEuip+1)%5);
	}

}

void TOChara::w_press()
{
	if(m_bPopup){
		m_Topop->w_press();
	}else{
		if(m_iCurrentEuip<0) m_iCurrentEuip = 5;
		RefreshEquip((m_iCurrentEuip+4)%5);
	}
}


void TOChara::z_press()
{
	if(m_bPopup){
		m_Topop->z_press();
	}else{
		EquipPop();
	}
}


void TOChara::x_press()
{
	if(m_bPopup){
		m_Topop->x_press();
	}else{

	}
}


void TOChara::EquipChange( int aid )
{
	switch (aid)
	{
	case -2:			// <Do Nothing
		{
			break;
		}
	case -1:			// <ж��װ��
		{
			TOEquips* ttoec = m_vEBtns[m_iCurrentEuip];
			ItemCellData* ticd = new ItemCellData(ttoec->eq->id,ttoec->eq->sum,ttoec->eq->lock);
			StateCenter::sharedStateCenter()->f_insert_item(ttoec->eq->id,4,ticd);


			g_chara->m_miiEquips.erase(m_iCurrentEuip);//[m_iCurrentEuip] = t_e->id;
			g_chara->m_viiELock[m_iCurrentEuip] = 0;
			g_chara->m_viiESum[m_iCurrentEuip] = 0;

			g_chara->calDiffer(ttoec->eq->effect,"");
			g_chara->aplyDiffer();
			ttoec->setcontent(NULL);

			break;
		}
	default:			// <�л�
		{
			ItemCellData* ticd = (ItemCellData*) StateCenter::sharedStateCenter()->f_get_itemlist(4)->objectForKey(aid);
			TOEquips* ttoec = m_vEBtns[m_iCurrentEuip];

			//Tye to get the equip from m_cdEqups,if none,generate one.How ever the data should be flush into g_chara_m_iis.
			int t_newid     = ticd->type_id;
			int t_newlock   = ticd->lock;
			int t_newsum	= ticd->sum;
			Equip* t_e = (Equip*) m_cdEquips->objectForKey(t_newid);
			if(!t_e){
				t_e = new Equip();
				m_cdEquips->setObject(t_e,t_newid);
				t_e->autorelease();
				m_Topop->inform(t_e);
			}

			if(ttoec->eq)
				g_chara->calDiffer(ttoec->eq->effect,m_Topop->m_sEffect);
			else
				g_chara->calDiffer("",m_Topop->m_sEffect);
			g_chara->aplyDiffer();

			ticd->type_id	= ttoec->eq->id;
			ticd->lock		= ttoec->eq->lock;
			ticd->sum		= ttoec->eq->sum;

			f_refresh_cur_data();

			t_e->sum	 = t_newsum;
			t_e->lock	 = t_newlock;
			g_chara->m_miiEquips[m_iCurrentEuip] = t_e->id;
			g_chara->m_viiELock[m_iCurrentEuip] = t_e->lock;
			g_chara->m_viiESum[m_iCurrentEuip] = t_e->sum;
			m_vEBtns[m_iCurrentEuip]->setcontent(t_e);
			//StateCenter::sharedStateCenter()->f_insert_item(atag,4,);
			break;
		}

	}
}


void TOChara::TOPopBack( CCObject* pSender )
{
	int titag = ((CCNode*) pSender)->getTag();

	switch (m_iPage)
	{
	case 1:
		EquipChange(titag);
		eq_mb->m_bIsEnabled = true;
		break;
	case 2:
		SkillChange(titag);
		sa_mb->m_bIsEnabled = true;
		break;
	default:
		break;
	}

	m_Topop->clear();
	m_bPopup = false;

	//m_iCurrentEuip = -2;
	
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

void TOChara::InitSaBtns()
{


	//////////////////////////////////////////////////////////////////////////
	sa_mb = new BYLayerDescendant();
	sa_mb->autorelease();
	sa_mb->setAnchorPoint(ccp(0,0));
	sa_mb->setPosition(ccp(0,0));
	sa_mb->setContentSize(CCSizeMake(900,600));
	mSp_sa->addChild(sa_mb,-1);

	//////////////////////////////////////////////////////////////////////////

	TOSkill* t_toe;
	PLACE_TOSKILL(280,379,m_vtsWsad,0);
	PLACE_TOSKILL(302,342,m_vtsWsad,1);
	PLACE_TOSKILL(302,243,m_vtsWsad,2);
	PLACE_TOSKILL(280,207,m_vtsWsad,3);

	PLACE_TOSKILL(484,379,m_vtsIkjl,4);
	PLACE_TOSKILL(462,342,m_vtsIkjl,5);
	PLACE_TOSKILL(462,243,m_vtsIkjl,6);
	PLACE_TOSKILL(484,207,m_vtsIkjl,7);
	
	//////////////////////////////////////////////////////////////////////////
	float t_oa_x = 329;
	TOASkill* t_taoe;
	for(int i=0;i<4;++i){
		t_taoe = new TOASkill();
		t_taoe->setAnchorPoint(CCPointZero);
		t_taoe->setPosition(t_oa_x,301);
		t_taoe->setTag(i+8);
		t_taoe->setactivator(this,menu_selector(TOChara::SkillClick));
		m_vasEight.push_back(t_taoe);
		sa_mb->addChild(t_taoe);
		//t_toe->autorelease();
		t_oa_x += 64;		
	}

	t_oa_x = 329;
	for(int i =4;i<8;++i){
		t_taoe = new TOASkill();
		t_taoe->setAnchorPoint(CCPointZero);
		t_taoe->setPosition(t_oa_x,279);
		t_taoe->setTag(i+8);
		t_taoe->setactivator(this,menu_selector(TOChara::SkillClick));
		m_vasEight.push_back(t_taoe);
		sa_mb->addChild(t_taoe);
		//t_toe->autorelease();
		t_oa_x += 64;
	}
	//////////////////////////////////////////////////////////////////////////
	t_oa_x = 426;
	TOTama* ttota;
	for(int i = 0;i<6;++i){
		ttota = new TOTama();
		ttota->setAnchorPoint(CCPointZero);
		ttota->setPosition(195,t_oa_x);
		ttota->setTag(i);
		ttota->setactivator(this,menu_selector(TOChara::EquipClick));
		m_vasEight.push_back(t_taoe);
		sa_mb->addChild(ttota);
		//t_toe->autorelease();
		t_oa_x += 20;
	}
	//////////////////////////////////////////////////////////////////////////
	t_oa_x = 430;
	TOVoice* tov;
	for(int i = 0;i<5;++i){
		tov = new TOVoice();
		tov->setAnchorPoint(CCPointZero);
		tov->setPosition(520,t_oa_x);
		tov->setTag(i);
		tov->setactivator(this,menu_selector(TOChara::EquipClick));
		m_vtvFive.push_back(tov);
		sa_mb->addChild(tov);
		//t_toe->autorelease();
		t_oa_x += 20;
	}
	//////////////////////////////////////////////////////////////////////////
	m_ltDisc = CCLabelTTF::create("----",FNT_UI_TTF,20, CCSize(631, 100), kCCTextAlignmentLeft);
	m_ltDisc->setAnchorPoint(ccp(0,0));
	m_ltDisc->setPosition(ccp(140,48));
	m_ltDisc->setColor(COLOUR_HOVER);
	sa_mb->addChild(m_ltDisc);

	m_ltName = CCLabelTTF::create("",FNT_UI_TTF,20);
	m_ltName->setAnchorPoint(CCPointZero);
	m_ltName->setPosition(ccp(347,548));
	m_ltName->setColor(COLOUR_HOVER);
	sa_mb->addChild(m_ltName);

	m_ltPro = CCLabelTTF::create("",FNT_UI_TTF,20);
	m_ltPro->setAnchorPoint(CCPointZero);
	m_ltPro->setPosition(ccp(585,548));
	m_ltPro->setColor(COLOUR_HOVER);
	sa_mb->addChild(m_ltPro);

	m_ltCV = CCLabelTTF::create("",FNT_UI_TTF,20);
	m_ltCV->setAnchorPoint(CCPointZero);
	m_ltCV->setPosition(ccp(576,524));
	m_ltCV->setColor(COLOUR_HOVER);
	sa_mb->addChild(m_ltCV);

}

//////////////////////////////////////////////////////////////////////////

void TOChara::ShowSa()
{
	m_ltName->setString(g_chara->m_sName.c_str());
	m_ltPro->setString("NONE");
	m_ltCV->setString("SAMPLE");

	m_iSkilId = -1;

	SkillMeta* ttosm = NULL;
	for(int i =0;i<4;++i){
		ttosm = (SkillMeta*) m_cdSkills->objectForKey(g_chara->m_viSkills[i]);
		m_vtsWsad[i]->setcontent(ttosm);
	}
	for(int i =4;i<8;++i){
		ttosm = (SkillMeta*) m_cdSkills->objectForKey(g_chara->m_viSkills[i]);
		m_vtsIkjl[i-4]->setcontent(ttosm);
	}

	for(int i =8;i<16;++i){
		ttosm = (SkillMeta*) m_cdSkills->objectForKey(g_chara->m_viSkills[i]);
		m_vasEight[i-8]->setcontent(ttosm);
	}
}


void TOChara::SkillClick( CCObject* pSender )
{
	Container* tc = (Container*) pSender;
	int titag = tc->getTag();
	CCLOG(">[TOCHARA]click is back:%d",titag);
	m_iSkilId = titag;
	popup_fx = 305;
	popup_fy = 200;

	titag = g_chara->m_viSkills[titag];
	SkillMeta* tsm = (SkillMeta*) m_cdSkills->objectForKey(titag);
	if(tsm){
		m_ltDisc->setString(tsm->discription.c_str());
	}else{
		m_ltDisc->setString("");
	}

	RefreshSkills();
	m_CurContainer->onHover();
	SkillPopup();
}

void TOChara::SkillPopup()
{
	if(m_iSkilId<0) return;

	int t_zbtype = 0;
	if(m_iSkilId<8) t_zbtype = 1;

	CC_SAFE_RELEASE_NULL(m_cdPopSkils);
	m_cdPopSkils = new CCDictionary();

	string t_sMask;
	SkillMeta* t_sm;

	for(map<int,int>::iterator it = g_chara->m_viSkills.begin(); it != g_chara->m_viSkills.end(); ++it)
	{
		if(it->first == it->second){
			t_sm = (SkillMeta*) m_cdSkills->objectForKey(it->first);
			if(t_sm->zbtype != t_zbtype) continue;
			t_sMask +=  CCString::createWithFormat("%d,",it->second)->getCString();
			m_cdPopSkils->setObject(m_cdSkills->objectForKey(it->first),it->second);
		}
		
	}

	CCString* t_csSql;
	if(t_sMask.size()<1) return;
	t_sMask.erase(t_sMask.length()-1);

	t_csSql = CCString::createWithFormat("select * from skill_list where itemid IN (%s) and usecase = %d",t_sMask.c_str(),t_zbtype);

	if(m_Topop->refresh_sks(t_csSql->getCString(),m_cdPopSkils)){
		sa_mb->m_bIsEnabled = false;
		m_Topop->setPosition(popup_fx,popup_fy);
		m_Topop->setAnchorPoint(ccp(1,0));
		m_Topop->setVisible(true);
		m_bPopup = true;
	}
	m_CurContainer->onSelect();
}

void TOChara::SkillChange( int sid )
{
	int old_id = g_chara->m_viSkills[m_iSkilId];
	if(old_id>0){
		g_chara->m_viSkills.insert(make_pair(old_id,old_id));
	}
	g_chara->m_viSkills[m_iSkilId] = sid;
	g_chara->m_viSkills.erase(sid);

	SkillMeta* t_sm = (SkillMeta*) m_cdSkills->objectForKey(sid);

	if(m_iSkilId<4){
		m_vtsWsad[m_iSkilId]->setcontent(t_sm);
	}else if(m_iSkilId<8){
		m_vtsIkjl[m_iSkilId-4]->setcontent(t_sm);
	}else{
		m_vasEight[m_iSkilId-8]->setcontent(t_sm);
	}
	m_ltDisc->setString(t_sm->discription.c_str());
	m_CurContainer->onHover();
}

void TOChara::RefreshSkills()
{
	int i = 0;
	for(;i<4;++i){
		m_vtsWsad[i]->onNormal();
	}
	for(;i<8;++i){
		m_vtsIkjl[i-4]->onNormal();
	}
	for(;i<16;++i){
		m_vasEight[i-8]->onNormal();
	}

	if(m_iSkilId<4){
		m_CurContainer = m_vtsWsad[m_iSkilId];
	}else if(m_iSkilId<8){
		m_CurContainer = m_vtsIkjl[m_iSkilId-4];
	}else{
		m_CurContainer = m_vasEight[m_iSkilId-8];
	}
}

