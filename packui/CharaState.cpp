#include "CharaState.h"

#include "packui/ConfigManager.h"
#include "utils/EffectControler.h"
#include "packui/CharaListView.h"


CharaState::~CharaState(){
//	CC_SAFE_RELEASE_NULL(m_caTList);
	//CC_SAFE_RELEASE_NULL(m_ldbEquList);
	CC_SAFE_RELEASE_NULL(m_cdEquips);
}

void CharaState::update(float fDelta){
	if(m_iChState>-1){
		Container::update(fDelta);
	}
	if(m_bKill3333){
		m_bKill3333 = false;
		removeChildByTag(0x3333);
	}
}

void CharaState::setcontent(Chara* apChara){
	if(!apChara) return;
	mpChara = apChara;
	f_init();
}

bool CharaState::f_init(){
	do 
	{
		//NULL for CC_SAFE_RELEASE_NULL in multi use of some component.
		CCLOG(">f_init.");
		m_bKill3333 = false;
		mpChraSprite = NULL;
		m_iChState = -1;
		m_iGroup = -1;
		m_iItem = -1;
		m_cdEquips = NULL;
		r_mb = NULL;
		m_ldbEquList = NULL;
		m_tDis = NULL;
		m_cbIClick = NULL;
		m_cbSChange = NULL;
		m_cbSPackD = NULL;
		r_inBox = NULL;

		//scheduleUpdate();
		return true;
	} while (0);
	return false;
}

void CharaState::setsprite(){
	if(mpChraSprite) return;

	mpChraSprite = CCSprite::create(mpChara->m_sPsz.c_str());
	mpChraSprite->setAnchorPoint(CCPointZero);
}

void CharaState::show_long(){			//显示长条的接口
	settouchstate(true);
	setContentSize(CCSizeMake(140,450));
	if(m_iChState > 0) {
		m_iChState = 1;	
		if(r_mb) {
			CCLOG(">Right Exits, setfalse;%d",m_iChState);
			r_mb->setVisible(false);
		}
		return;				//long should always be on. Small might be 0.
	}
	initwithPre("Images/button",140,450);
	setsprite();

	show_content();
}

void CharaState::show_mini(){
	if(mpChraSprite) return;
	settouchstate(true);
	setContentSize(CCSizeMake(140,140));

	initwithPre("Images/button",140,140);
	
	if(!mpChraSprite){
		mpChraSprite = CCSprite::create(mpChara->m_sPsz.c_str(),CCRectMake(0,50,140,140));
		mpChraSprite->setAnchorPoint(CCPointZero);
	}

	show_content();
}

void CharaState::onSelect(){
	CCLOG(">CS CLICKED");
}

void CharaState::refresh_equlist() 
{
	do 
	{
		for(vector<Container*>::iterator it = m_vEBtns.begin(); it != m_vEBtns.end(); ++it)
		{
			(*it)->removeFromParent();
		}

		//Prepare data.
		string t_sMask;

		CC_SAFE_RELEASE_NULL(m_cdEquips);
		m_cdEquips = new CCDictionary();

		if(mpChara->m_miiEquips.size() > 0) {
			for(map<int,int>::iterator it = mpChara->m_miiEquips.begin(); it != mpChara->m_miiEquips.end(); ++it){
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

		//Showing.
		m_x = 5;
		m_y = 210;
	//	m_ltEquips = CCArray::createWithCapacity(6);
	//	m_ltEquips->retain();
		m_vEBtns.clear();
		for(int i = 0; i<6; ++i){
			Container* t_ct = new Container();
			t_ct->initwithPre("Images/Button",130,20);
			t_ct->setactivator(this,menu_selector(CharaState::eqBack));
			t_ct->setAnchorPoint(CCPointZero);
			t_ct->setPosition(ccp(m_x,m_y));
			t_ct->setTag(i);
			t_ct->setstringnull();
			t_ct->setOpacity(100);
			m_vEBtns.push_back(t_ct);

			r_mb->addChild(t_ct,2);

			Equip* t_e = (Equip*) m_cdEquips->objectForKey(mpChara->m_miiEquips[i]);
			CCLabelTTF* t_clt;
			if(t_e)
				t_clt = CCLabelTTF::create(t_e->name.c_str(),FNT_UI_LABEL,18);
			else
				t_clt = CCLabelTTF::create("NONE",FNT_UI_LABEL,18);

			t_clt->setAnchorPoint(CCPointZero);
			t_clt->setPosition(ccp(1,1));
			t_ct->addChild(t_clt);
	//		m_ltEquips->insertObject(t_clt,i);

			m_y -= 20;
		}
	} while (0);
}

void CharaState::show_all()		//显示所有内容
{
	settouchstate(false);
	setContentSize(CCSizeMake(700,500));
	if(r_mb) {
		r_mb->setVisible(true);
		show_listndis();				//Value may change due to other page.
		return;
	}
	if(m_iChState == 2) return;			//PE:Simply Return Here. But if m_iChState comes with a null r_mb, There might be some error with init code.
	m_iChState = 2;
	CCLOG(">Prepare to show all.%s",mpChara->m_sName.c_str());	
	
	r_mb = new BYLayerDescendant();
	r_mb->autorelease();
	r_mb->setAnchorPoint(ccp(0,0));
	r_mb->setPosition(ccp(0,1));
	r_mb->setContentSize(CCSizeMake(700,500));
	addChild(r_mb,1);

	Container* t_abc = new Container();
	t_abc->initString("Back",102,23,"Images/Button",this,menu_selector(CharaState::useItemB));
	t_abc->setTag(-1);
	t_abc->setAnchorPoint(CCPointZero);
	t_abc->setPosition(ccp(36,450));
	r_mb->addChild(t_abc);

	//////////////////////////////////////////////////////////////////////////
	// showing attr table

	refresh_attrtable();

	m_testdeffer = CCLabelBMFont::create("[Test Differ OUT]",  "fonts/CocoTd.fnt");
	m_testdeffer->setAnchorPoint(CCPointZero);
	m_testdeffer->setPosition(160,m_y -30);
	m_testdeffer->setVisible(false);
	m_testdeffer->setColor(ccRED);
	r_mb->addChild(m_testdeffer);

	m_tDis = CCLabelTTF::create();
	m_tDis->setAnchorPoint(ccp(0,1));
	m_tDis->setFontName( FNT_UI_LABEL);
	m_tDis->setDimensions(CCSizeMake(200,0));
	m_tDis->setFontSize(20);
	m_tDis->setPosition(ccp(151,280));
	r_mb->addChild(m_tDis);

	show_listndis();
		//////////////////////////////////////////////////////////////////////////
	// Equ list

	/*string test = "atk 1 def 2 mag 3";
	stringstream teststream;
	teststream<<test;
	string s;
	int k;
	for(int i = 0;i<2;i++){
		teststream>>s;
		teststream>>k;
		CCLOG("Read out :%s.%d.:",s.c_str(),k);
	}*/
	


	refresh_equlist();

	

	CCScale9Sprite* nback = CCScale9Sprite::create("Images/life_bar1.png"); 
	nback->setAnchorPoint(ccp(0,0));
	nback->setContentSize(CCSize(200,280));
	nback->setOpacity(40);
	nback->setPosition(ccp(151,1));
	r_mb->addChild(nback,-1);

}

void CharaState::show_listndis(){				//every equip has a maskbit perpoty. Its for further disign.
	if(!r_inBox){
		
		r_inBox = new BYLayerDescendant();
		r_inBox->autorelease();
		r_inBox->setAnchorPoint(ccp(0,0));
		r_inBox->setPosition(ccp(353,1));
		r_inBox->setContentSize(CCSizeMake(301,281));

		CCScale9Sprite* nback = CCScale9Sprite::create("Images/lifebarbac.png"); 
		nback->setAnchorPoint(ccp(0,0));
		nback->setContentSize(CCSize(300,280));
		nback->setOpacity(110);
		r_inBox->addChild(nback,-1);
		r_mb->addChild(r_inBox);
	}

	if(m_ldbEquList) {
		m_ldbEquList->removeFromParent();		//m_ldbEquList should be reffreshed.
		m_ldbEquList = NULL;
	}
	switch(m_iGroup){						//m_iCurE is now duplicate with m_iGroup, but some further differ might be needed by both side.
	case(-1):
		{
			//m_iGroup = -1;
			setscclicks(ConfigManager::sharedConfigManager()->GetConfigS("view_skill"));
			m_cbSChange->setVisible(false);
			m_cbSChange->onNormal();

			if(m_cbSPackD){
				m_cbSPackD->setVisible(false);
				m_cbSPackD->onNormal();
			}

			if(mpChara->m_viSkills.size() > 0){			//TODO: Not Showing skill-list/m_ldbEquList if there is no skill, modify it while needed.
				

				CCDictionary* m_cid = new CCDictionary();
				string t_sMask;
				for(vector<int>::iterator it = mpChara->m_viSkills.begin(); it != mpChara->m_viSkills.end(); ++it)
				{
					t_sMask +=  CCString::createWithFormat("%d,",*it)->getCString();
					ItemCellData* t_icd	=	new ItemCellData(*it,0,0);
					m_cid->setObject(t_icd,*it);
					t_icd->release();
				}
				t_sMask.erase(t_sMask.length()-1);
				CCString* t_csSql = CCString::createWithFormat("select * from skill_list where itemid IN (%s)",t_sMask.c_str());

				
				CC_SAFE_RELEASE_NULL(m_ldbEquList);
				m_ldbEquList = new ListDBView<ItemCell>(300,250, t_csSql->getCString(),m_cid, this,menu_selector(CharaState::ItemBack),1);
				if(m_ldbEquList->init()){
					m_ldbEquList->setAnchorPoint(ccp(0,1));
					m_ldbEquList->setPosition(ccp(2,0));
					m_ldbEquList->setContentSize(CCSizeMake(300,250));
					r_inBox->addChild(m_ldbEquList);
					m_ldbEquList->release();
				}else{
					CC_SAFE_RELEASE_NULL(m_ldbEquList);
				}

				m_tDis->setString("Click & The Skill will show.");

				setmbclicks(ConfigManager::sharedConfigManager()->GetConfigS("use"));
				m_cbIClick->setactivator(this,menu_selector(CharaState::useItemB));
				m_cbIClick->setVisible(true);

			}

			break;
		}
	default:			// 0~5 for EQU || 5
		{
			//Equip* t_e = (Equip*) m_cdEquips->objectForKey(mpChara->m_miiEquips[m_icurE]);	//Get Current Equip
			CCDictionary* t_caEqui	 = StateCenter::sharedStateCenter()->f_get_itemlist(4);		//Group 4 -- Equips.
			string t_sMask;

			setscclicks(ConfigManager::sharedConfigManager()->GetConfigS("view_skill"));
			m_cbSChange->setactivator(this,menu_selector(CharaState::skiback));
			m_cbSChange->setVisible(true);

			if(!m_cbSPackD){
				m_cbSPackD = new Container();
				m_cbSPackD->initwithPre("Images/button",124,22);
				m_cbSPackD->setstringnull();
				m_cbSPackD->setTag(-1);
				m_cbSPackD->setAnchorPoint(ccp(0,0));
				m_cbSPackD->setPosition(ccp(481,251));
				m_cbSPackD->setstring(ConfigManager::sharedConfigManager()->GetConfigS("pack_down"));
				m_cbSPackD->setactivator(this,menu_selector(CharaState::changeEqui));

				r_mb->addChild(m_cbSPackD);
			}


			if(mpChara->m_miiEquips[m_icurE] > 0){
				m_cbSPackD->setVisible(true);				
				m_cbSPackD->onNormal();
			}else{
				m_cbSPackD->setVisible(false);
			}
			
			if(t_caEqui){
			
				CCDictElement* pCe = NULL;


				CCDICT_FOREACH(t_caEqui,pCe){
					t_sMask +=  CCString::createWithFormat("%d,",pCe->getIntKey())->getCString();
				}
				if(t_sMask.length() > 0){
					t_sMask.erase(t_sMask.length()-1);
					CCString* t_csSql = CCString::createWithFormat("select * from equip_list where itemid IN (%s) and position = %d", t_sMask.c_str(), m_icurE);

					CC_SAFE_RELEASE_NULL(m_ldbEquList);
					m_ldbEquList = new ListDBView<ItemCell>(300,250, t_csSql->getCString(),t_caEqui, this,menu_selector(CharaState::ItemBack),2);
					if(m_ldbEquList->init()){
						m_ldbEquList->setAnchorPoint(ccp(0,1));
						m_ldbEquList->setPosition(ccp(2,0));
						m_ldbEquList->setContentSize(CCSizeMake(300,250));
						r_inBox->addChild(m_ldbEquList);
						m_ldbEquList->autorelease();


					}else{
						CC_SAFE_RELEASE_NULL(m_ldbEquList);
					}

				}
				setmbclicks(ConfigManager::sharedConfigManager()->GetConfigS("change"));
				m_cbIClick->setactivator(this,menu_selector(CharaState::changeEqui));
				m_cbIClick->setVisible(false);

				Equip* t_eq = ((Equip*) m_cdEquips->objectForKey(mpChara->m_miiEquips[m_icurE]));
				if(t_eq)
					m_tDis->setString(t_eq->discript.c_str());
				else
					m_tDis->setString("");

				break;			
			
			
			}
			

		}


	}


}

void CharaState::flush_value(string name, int value){
	r_mb->removeChildByTag(0x5004 + m_x + m_y);
	CCLabelBMFont* t_clbmf = CCLabelBMFont::create(CCString::createWithFormat("%s:%d",name.c_str(),value)->getCString(),  "fonts/CocoTd.fnt");
	t_clbmf->setTag(0x5004 + m_x + m_y);	//Test Only
	
	t_clbmf->setAnchorPoint(CCPointZero);
	t_clbmf->setPosition(ccp(m_x,m_y));
	r_mb->addChild(t_clbmf);
	CCLOG(">FlushValue:%s,%d AT %f,%f",name.c_str(),value,m_x,m_y);
	if(m_x<250){
		m_x += 100;
		
	}else{
		m_x -= 100;
		m_y -= 30;
	}
}

void CharaState::RefreshValues()
{
	lbhp->setval(mpChara->gethp());
	lbmp->setval(mpChara->getmp());
}

void CharaState::show_content()			//显示背景和状态条
{
	l_mb = new BYLayerDescendant();
	l_mb->setAnchorPoint(ccp(0,0));
	l_mb->setPosition(ccp(1,1));
	l_mb->autorelease();
	addChild(l_mb);

	removeChildByTag(0x98);

	m_iChState = 1;

	mpChraSprite->setVisible(true);
	mpChraSprite->setPosition(ccp(0,0));
	mpChraSprite->setTag(0x98);
	l_mb->addChild(mpChraSprite);

	float t_height = 78;

	labelAtlas = CCLabelBMFont::create(mpChara->m_sName.c_str(), "fonts/CocoTd.fnt");
	labelAtlas->setAnchorPoint(CCPointZero);
	labelAtlas->setPosition(ccp(10,t_height));
	l_mb->addChild(labelAtlas,1);

	t_height -= 25;
	lbhp = new LifeBar("Images/life_bar.png",mpChara->getvalue("hp"));
	lbhp->setAnchorPoint(CCPointZero);
	lbhp->setPosition(ccp(5,t_height));
	l_mb->addChild(lbhp,1);

	t_height -= 20;
	lbmp = new LifeBar("Images/life_bar1.png",mpChara->getvalue("mp"));
	lbmp->setAnchorPoint(CCPointZero);
	lbmp->setPosition(ccp(5,t_height));
	l_mb->addChild(lbmp,1);

	CCScale9Sprite* nback = CCScale9Sprite::create("Images/button.png"); 
	nback->setContentSize(CCSizeMake(140,100));
	nback->setPosition(ccp(0,0));
	nback->setAnchorPoint(CCPointZero);
	nback->setOpacity(140);
	l_mb->addChild(nback);

	RefreshValues();
}

void CharaState::ItemBack( CCObject* pSender )
{	

	int t_iItem = ((ItemCell*) pSender)->getTag();
	CCLOG(">ItemCallsBack:%d",t_iItem);
	switch(m_iGroup){
	case(-1):
		{
			m_iItem = t_iItem;
			m_tDis->setString(m_ldbEquList->getdiscrip(m_iItem).c_str());

			//m_cbIClick->setstring(ConfigManager::sharedConfigManager()->GetConfigS("use"));
			int i = stoi(m_ldbEquList->getval("usecase",m_iItem));
			if(i&1) 
				m_cbIClick->settouchstate(true);
			else 
				m_cbIClick->onDisable();//settouchstate(false);
			break;
		}
	default:
		{
			m_iItem = ((ItemCell*) pSender)->getTag();

			ItemCellData* t_icd = (ItemCellData*) StateCenter::sharedStateCenter()->f_get_itemlist(4)->objectForKey(m_iItem);
			bool t_canE = t_icd->sum > t_icd->lock;
			Equip* t_eq = (Equip*) m_cdEquips->objectForKey(mpChara->m_miiEquips[m_icurE]);
			string e2 = m_ldbEquList->getval("effect",m_iItem);
			if(t_eq){
				mpChara->calDiffer(t_eq->effect,e2);
			}
			else{
				mpChara->calDiffer("",e2);
			}

			m_tDis->setString(m_ldbEquList->getdiscrip(m_iItem).c_str());
			m_cbIClick->setVisible(true);

			if(m_iItem != mpChara->m_miiEquips[m_icurE] && t_canE) 
				m_cbIClick->settouchstate(true);
			else 
				m_cbIClick->onDisable();//settouchstate(false);

			CCLOG(">Calculate the differ.");
			showEquDiffer();
			CCLOG(">m_iItem:%d",m_iItem);

			break;
		}
	}
}

void CharaState::RefreshEBtns()
{
	for(vector<Container*>::iterator it = m_vEBtns.begin(); it != m_vEBtns.end(); ++it){
		if((*it)->getTag() == m_icurE){
			continue;
		}
		(*it)->onNormal();
	}
}

void CharaState::eqBack( CCObject* pSender )			//Calling back of left equi list. Decide the equ position and prepare to look for avalible equs.
{
	m_icurE		 =	 ((Container*) pSender)->getTag();
	m_iGroup	 =	 m_icurE;

	CCLOG(">EQBACK:%d",m_icurE);	
	
	RefreshEBtns();
	show_listndis();

	//ItemBack(pSender);
}

void CharaState::useItemB( CCObject* pSender )
{
	int t_iTag = ((Container*) pSender)->getTag();
	((Container*) pSender)->onNormal();
	if(t_iTag<0){
		settouchstate(true);
		activate();
	}else{
		if(m_iItem>-1)
			EffectControler::sharedEffectControler()->md_use_skill(this,m_iItem,mpChara);
	}
	CCLOG("UseItem");
}

void CharaState::get_target(){
	int t_iTarget = EffectControler::sharedEffectControler()->m_iTarget;		//TODO: chech if it is 0
	r_mb->m_bIsEnabled = false;
	
	m_caTList	=	new CCArray();		//DONE:it's cleared by ec every effect.

	removeChildByTag(0x3333);

	CharaListView* clw = new CharaListView();
	clw->autorelease();
	clw->m_iCLVState = -1;
	clw->init();
	
	clw->setbegin(0);
	clw->setAnchorPoint(CCPointZero);
	clw->setPosition(ccp(51,141));

	clw->setactivator(this,menu_selector(CharaState::targetBack));
	
	clw->setTag(0x3333);
	addChild(clw,3);
}

void CharaState::targetBack( CCObject* pSender)
{
	//IN:[TODO]:if value_target == 0 , then it means affecting all the chara, we just get that the sender is back.

	CharaState* t_cs = (CharaState*) pSender;
	if(t_cs->getTag() < 0)
	{
		EffectControler::sharedEffectControler()->f_effect_over();
		EffectControler::sharedEffectControler()->f_clear();
		m_bKill3333 = true;
		r_mb->m_bIsEnabled = true;
	}else{
		m_caTList->addObject(t_cs->getcontent());
		CCLOG("[ECICS]>Target back, add to catlist&callc.");

		EffectControler::sharedEffectControler()->md_act_skill(mpChara,m_caTList);
	}

}

Chara* CharaState::getcontent()
{
	return mpChara;
}

void CharaState::effect_over()
{
	CC_SAFE_RELEASE_NULL(m_caTList);
	CCLOG("[ECICS]>Effect over&clear states.");
	m_caTList	=	new CCArray();			// only release this list when clv_mini is gone.
}

void CharaState::show_text( string s )
{
	((CharaListView*) getChildByTag(0x3333))->showtargettext(s);
	((CharaListView*) getChildByTag(0x3333))->RefreshListValue();
	RefreshValues();
	EffectControler::sharedEffectControler()->f_sp_gonext();
}

void CharaState::refresh_view()					//not used...
{
	RefreshValues();
}



void CharaState::setscclicks( string title )
{
	if(!m_cbSChange){
		m_cbSChange = new Container();
		m_cbSChange->initwithPre("Images/button",124,22);
		m_cbSChange->setstringnull();
		m_cbSChange->setTag(1);
		m_cbSChange->setAnchorPoint(ccp(0,0));
		m_cbSChange->setPosition(ccp(355,251));
		r_mb->addChild(m_cbSChange);
	}
	m_cbSChange->setstring(title);
	m_cbSChange->onNormal();
}

void CharaState::setmbclicks( string title )
{
	if(!m_cbIClick){
		m_cbIClick = new Container();
		m_cbIClick->initwithPre("Images/button",124,22);
		m_cbIClick->setstringnull();
		m_cbIClick->setTag(1);
		m_cbIClick->setAnchorPoint(ccp(0,0));
		m_cbIClick->setPosition(ccp(151,1));
		r_mb->addChild(m_cbIClick);
	}
	m_cbIClick->setstring(title);
	m_cbIClick->onNormal();
}


void CharaState::showEquDiffer()
{
	string t_mask;
	for(map<string,int>::iterator it = mpChara->mt_stDiffer.begin(); it != mpChara->mt_stDiffer.end(); ++it){
		t_mask += CCString::createWithFormat("%s,%d",it->first.c_str(),it->second)->getCString();
	}
	m_testdeffer->setCString(t_mask.c_str());
	m_testdeffer->setVisible(true);
}

void CharaState::refresh_attrtable()
{
	

	m_x = 160;
	m_y = 480;

	for(map<string,int>::iterator it =  mpChara->m_iiAttrs.begin(); it!= mpChara->m_iiAttrs.end(); it++){
		flush_value(it->first,it->second);
	}

	flush_value("More is coming...This table is for test only.",0);
}

//The Equ Is now Change. Refresh all the data and view.
void CharaState::changeEqui(CCObject* pSender)
{
	int itag = ((Container*) pSender)->getTag();

	m_testdeffer->setVisible(false);
	CCDictionary* t_caEqui	 = StateCenter::sharedStateCenter()->f_get_itemlist(4);		//Group 4 -- Equips.

	if(itag<0){
		ItemCellData* t_icd = (ItemCellData*) t_caEqui->objectForKey(mpChara->m_miiEquips[m_icurE]);
		if(t_icd) t_icd->lock--;

		Equip* t_eq = (Equip*) m_cdEquips->objectForKey(mpChara->m_miiEquips[m_icurE]);
		
		/*if(t_eq){*/
			mpChara->calDiffer(t_eq->effect,"");
		//}else{
		//	exit(0x5005);			//
		//}

		mpChara->m_miiEquips.erase(m_icurE);
		mpChara->aplyDiffer();
	
	}
	else
	{		
		ItemCellData* t_icd = (ItemCellData*) t_caEqui->objectForKey(mpChara->m_miiEquips[m_icurE]);
		if(t_icd) t_icd->lock--;
		t_icd = (ItemCellData*) t_caEqui->objectForKey(m_iItem);
		if(t_icd) t_icd->lock++;
		
		mpChara->m_miiEquips[m_icurE] = m_iItem;
		mpChara->aplyDiffer();
	}


	refresh_attrtable();
	refresh_equlist();


	show_listndis();
	RefreshEBtns();
	//m_vEBtns do not need to be changed
	//Equip* t_e = (Equip*) m_cdEquips->objectForKey(mpChara->m_miiEquips[i]);
}

void CharaState::skiback( CCObject* pSender )
{
	m_iGroup = -1;
	m_iItem = -1;

	refresh_attrtable();
	refresh_equlist();

	show_listndis();
	RefreshEBtns();
}
