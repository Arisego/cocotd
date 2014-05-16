#include "sublayer/MapLayer.h"
#include "utils/EffectControler.h"
#include "SingleTon/BattleFiled.h"
#include "Component/AIComponent.h"
#include "GameManager.h"
#include "hud/RightUpgr.h"
#include "SingleTon/EGroup.h"
#include "SoundManager.h"
#include "SingleTon/SkillJudge.h"

USING_NS_CC;

#define WM_Z 1
#define BM_Z 2
#define SL_Z 3

#define LFM_ZORDER 90
#define SPL_ZORDER 89

static const char* sArrowPath[] = {"Images/arrow_broken_left.png","Images/arrow_normal_left.png","Images/arrow_blood_left.png"};

//void MapLayer::registerWithTouchDispatcher()
//{
//	CCDirector* pDirector = CCDirector::sharedDirector();
//	pDirector->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority, true);
//}


MapLayer::MapLayer():
	tm(NULL),
	bm(NULL),
	wm(NULL),
	mbAutoLogic(true),
	mlgStat(NULL),
	mbNeedScroll(false),
	mbSBProtect(false),
	m_iItem(-1),
	mbCheckLock(false)
{
	mbAIControl = false;
}


MapLayer::~MapLayer(){
	CCLog(">MapLayer Destruct.");
	
	EventCenter::sharedEventCenter()->unsetBmCake(this);

	BFsp->re_init();
	delete BFsp;
	removeAllChildren();
	
	if(m_SkillList) {
		//m_SkillList->removeFromParent();
		CC_SAFE_RELEASE_NULL(m_SkillList);							// <��ֹ�Ҽ��㼶�п��ܳ��ֵ�bug
	}

	CC_SAFE_RELEASE_NULL(bm);
	CC_SAFE_RELEASE_NULL(wm);

	CC_SAFE_RELEASE_NULL(m_BattleMenu);
	CC_SAFE_RELEASE_NULL(m_ldb);
	CC_SAFE_RELEASE_NULL(m_lfm);
	//CC_SAFE_RELEASE_NULL(m_lpJudgement);
	//CC_SAFE_RELEASE_NULL(m_etClock);
	//tm = NULL;
	//
	CC_SAFE_RELEASE_NULL(m_Spliter);
	CC_SAFE_RELEASE_NULL(m_Hmenu);
	CC_SAFE_RELEASE_NULL(mlgStat);
	//Spliter::purgeSharedSpliter();
	//HMenu::purgeSharedHMenu();
}

void MapLayer::LoadMap(const char* pszN, const char* pscR, Script* tp){
	//removeAllChildren();
	tm = NULL;
	wm = new WalkMap(pszN,pscR,tp);
	addChild(wm,WM_Z);
	wm->f_init(this,SL_Z);
	tm = wm;
	m_iMLState = 1;
	tm->ELoad();

}

void MapLayer::openTileMap(const char* pszN, const char* pscr){
	if(wm){
		wm->removeFromParent();
		wm->f_clear_circles();
		CC_SAFE_RELEASE_NULL(wm);
	}
	wm = new WalkMap(pszN,pscr);
	//wm->retain();
	addChild(wm,WM_Z);
	wm->f_init(this,SL_Z);
	tm = wm;
	m_iMLState = 1;
}

void MapLayer::openBattleMap(const char* pszN, int i_egr){
	
	if(bm){
		bm->removeFromParent();
		CC_SAFE_RELEASE_NULL(bm);
	}
	bm = new BattleMap(pszN);
	bm->f_generateEnemy(i_egr);
	bm->f_load_chara();
	bm->PlaceEChesses(NULL);
	BattleField::sharedBattleField()->InitBattleFiled();	 
	bm->init();
	addChild(bm,BM_Z);
	tm = bm;
	f_init_battle();
	bm->EnActive();
}

void MapLayer::closeBattleMap(){
	if(bm){
		bm->removeFromParent();
		CC_SAFE_RELEASE_NULL(bm);
	}
	EventCenter::sharedEventCenter()->setBmCake(NULL);
	tm = wm;
	m_iMLState = 1;
}

void MapLayer::closeTileMap(){
	if(wm){
		wm->removeFromParent();
		wm->f_clear_circles();
		CC_SAFE_RELEASE_NULL(wm);
	}
	tm = NULL;
	m_iMLState = -1;
}

void MapLayer::f_init(){
	bm    =  NULL;
	wm    =  NULL;
	tm    =  NULL;
	m_IP  =  NULL;
	m_ltb =	 NULL;
	m_BattleMenu  =	 NULL;
	m_ldb =	 NULL;
	m_lfm =	 NULL;

	m_lsb = NULL;
	m_rsb = NULL;

	m_SkillList = NULL;

	//m_lpJudgement	=	NULL;
	//m_etClock		=	NULL;
	//m_sCon			=	NULL;

	BFsp = new Scriptor();

	m_iMLState = -1;
	//scheduleUpdate();
	MapLayerComp* mlp = new MapLayerComp();
	addComponent(mlp);
	mlp->autorelease();

	mbAIControl = false;
	//////////////////////////////////////////////////////////////////////////
	// <���HUD��
	m_Spliter = new Spliter();
	m_Spliter->setAnchorPoint(CCPointZero);
	m_Spliter->setPosition(CCPointZero);
	addChild(m_Spliter, SPL_ZORDER);

	m_Hmenu = new HMenu();
	m_Hmenu->setAnchorPoint(CCPointZero);
	m_Hmenu->setPosition(CCPointZero);
	addChild(m_Hmenu, LFM_ZORDER);

	mlgStat = new LgStat();
	addChild(mlgStat,SPL_ZORDER);
}

void MapLayer::f_pauseall(){
	if(!tm) return;
	tm->pauseSchedulerAndActions();
}

void MapLayer::f_stopcontrol(){
	if(!tm) return;
	tm->clearcontrol();
	CCLog("CONTORL STOPED");
}

void MapLayer::f_resumeall(){
	if(!tm) return;
	tm->resumeSchedulerAndActions();
}

void MapLayer::f_resumecon(){
	if(!tm) return;
	if(m_Hmenu)
		if(m_Hmenu->miState == 1){
			CCLog(">[MapLayer] f_sesumecon() | m_Hmenu state == 1 | Can not resume control.");
			return;
		}
	tm->cancontrol = true;
	
	if(m_lsb) m_lsb->RefreshAll();
	CCLog("CONTORL RESUME");
}

void MapLayer::Close(){		//�ر���ʾ�͵�ͼ�л��ɱ�ĺ�������
	f_pauseall();
	close_hud();
}

void MapLayer::Pause(){
	setTouchEnabled(false);
	f_stopcontrol();
	if(!tm) return;
	tm->setTouchEnabled(false);
	close_hud();
}

void MapLayer::Resume(){
	setTouchEnabled(true);
	setVisible(true);
	f_resumecon();
	f_resumeall();
	if(!tm) return;
	show_hud();
	tm->setTouchEnabled(true);
}

void MapLayer::show_hud(){
	switch(m_iMLState){

	case(1):		//WalkMap with HUD
		{
			

			if(!m_IP){
				m_IP = new ItemPicker();
				m_IP->autorelease();
		
				//if(!
				m_IP->init();
				//	){
				//	CC_SAFE_RELEASE_NULL(m_IP);
				//}else{
					m_IP->setAnchorPoint(CCPointZero);
					m_IP->setPosition(ccp(0,0));
					m_IP->setactivator(this,menu_selector(MapLayer::item_use));
					addChild(m_IP,99);
			//	}
				
			}
			 m_IP->onShow();

			if(!m_ltb){
				m_ltb = new LTLifeBlock();
				m_ltb->autorelease();

				m_ltb->init();
				m_ltb->setAnchorPoint(ccp(0,1));
				m_ltb->setPosition(ccp(1,500));
				m_ltb->setChara();

				addChild(m_ltb,98);
			}
			m_ltb->onShow();

			break;
		}
	case 2:
	case 3:
		{
			if(m_Spliter) m_Spliter->f_open();
			else {
				m_Spliter = new Spliter();
				addChild(m_Spliter,SPL_ZORDER);
				m_Spliter->f_open();
			}
			if(m_Hmenu) m_Hmenu->f_open();
			else {
				m_Hmenu = new HMenu();
				addChild(m_Hmenu,SPL_ZORDER);
				m_Hmenu->f_open();
			}

			if(m_ltb) m_ltb->setVisible(false);
			if(m_IP) m_IP->setVisible(false);

			if(mlgStat) mlgStat->setVisible(true);

			CCLOG("[PE]>unknown maplayer state:%d",m_iMLState);
			break;
		}
	default:
		{
			break;			
		}


	}

}

void MapLayer::close_hud(){
	if(m_IP){
		m_IP->onDismiss();
	}
	if(m_ltb){					//It seems duplicate. Remove it if it's really safe.
		m_ltb->onDismiss();
	}
	if(m_lsb){
		;
	}

	if(m_Spliter) m_Spliter->f_pause();
	if(m_Hmenu) m_Hmenu->f_pause();
	if(mlgStat) mlgStat->setVisible(false);
}

void MapLayer::item_use( CCObject* pSender )
{
	m_sIcd = (ItemCellData*) pSender;
	EffectControler::sharedEffectControler()->md_use_item(this,m_sIcd->type_id);
}

void MapLayer::refresh_view()
{
	show_hud();
}

void MapLayer::show_text( string s )			//Value Changes?
{
	CCLOG("Text Show:%s",s.c_str());
	
	switch(m_iMLState){
	case(1):
		{
			wm->show_text(s);
			break;
		}
	case(2):
		{
			CCLOG("Show_Test FOR ALL THE TARGET.");			//����WM��showtext��ע�����е���Ч�����������ã������޷�Ӧ�ô浵���ܡ�
			bm->show_text(s);
			break;
		}
	default:
		{
			CCLOG(">[PE:Unknow mlstate | show_text:%d",m_iMLState);
			break;
		}
	}
	EffectControler::sharedEffectControler()->f_sp_gonext();
	
}

void MapLayer::effect_over()
{
	refresh_view();
}

// <������Ч��
void MapLayer::get_target()
{
	switch(m_iMLState){
	case(1):
		{
			m_sIcd->sum--;							//DerLock is now not used for item use, it always pass, cause we do not have reason to lock item use. so we simply minus one to the num here.
			int t_iTarget = EffectControler::sharedEffectControler()->m_iTarget;		//TODO: check if it is 0
			if(t_iTarget == -1){					//Target == -1 . there is no need for target. use it here is just for notifaction to view.
				//m_sIcd->sum -= 1;				//[ATT]:The design has changed.
				EffectControler::sharedEffectControler()->md_act_item(NULL);			//????????
			}else{
				if(m_caTList) CC_SAFE_RELEASE_NULL(m_caTList);
				m_caTList	=	new CCArray();
				if(m_ltb->mpChara) m_caTList->addObject(m_ltb->mpChara);			//Test Only.This check may not be needed.
				EffectControler::sharedEffectControler()->md_act_item(m_caTList);
			}
			break;
		}
	case(2):
		{
			//EffectControler::sharedEffectControler()->md_act_skill(this,bm->m_caTarCharas);
			BFsp->re_init();
			BFsp->parse_string(EffectControler::sharedEffectControler()->getESp());
			bm->HandleScriptor(BFsp);
			bm->clean_allcs();
			break;
		}
	}



}

void MapLayer::handlesp( Script* sp )
{
	if(sp->type == sMapAction){					//MapLayer now only handle mapaction. Others will be refused.
		switch(m_iMLState){
		case(1):
			{
				switch(sp->getint("type")){
				case(0):
					{
						wm->addbullet(sp);
						break;
					}
				default:
					{
						CCLOG(">[PE:Unknow mlstate | show_text:%d",m_iMLState);
						break;
					}
				}// switch sp->getint("type");
				break;
			}

		}// switch m_iMLState
	}		
	EffectControler::sharedEffectControler()->f_sp_gonext();
	CCLOG(">Prepare to handle mapaction.");
}

void MapLayer::show_menu(int aiFlag)
{
	CCLog(">[MapLayer] show_menu() | Ready for menu check...");
	if(((EChesses*) bm->m_controller)->m_pChara->checkXuDong()){
		CCLog(">[ML]You can not move it any more.");
		return;
	}

	if(!bm->m_touch) return;
	if(!m_BattleMenu){
		m_BattleMenu = new BattleMenu();			//TODO:Decide what to show by bit lock.
		CCLog(">LKDJFLKJ----- New Menu.....");
		addChild(m_BattleMenu,99);
	}

	CCLog(">[MapLayer] show_menu() | Ready for menu show...");
	mbSBProtect = true;
	
	m_BattleMenu->setAnchorPoint(CCPointZero);
	
	CCPoint worldPoint = this->bm->convertToWorldSpace(bm->m_controller->getPosition());
	CCLog(">[MapLayer] show_menu() | worldPoint(%f, %f)", worldPoint.x, worldPoint.y);
	worldPoint = CCDirector::sharedDirector()->convertToUI(worldPoint);

	CCLog(">[MapLayer] show_menu() | worldPoint(%f, %f)", worldPoint.x, worldPoint.y);
	float nx = worldPoint.x+44;
	float ny = CCDirector::sharedDirector()->getWinSize().height - worldPoint.y;
	ny -= 100;

	ny = max(30,ny);
	ny = min(200,ny);

	//if(nx>800) nx -= 580;
	m_BattleMenu->setPosition(nx,ny);
	m_BattleMenu->bitLock = aiFlag;
	m_BattleMenu->setactivator(this,menu_selector(MapLayer::menu_back));
	if(m_lsb) m_lsb->setVisible(true);
	m_BattleMenu->show();
	m_BattleMenu->setVisible(true);
	bm->set_bbattle(2);

}

void MapLayer::ccTouchEnded( CCTouch *pTouch, CCEvent *pEvent )
{
	CCLog(">[MapLayer] ccTouchEnded() | Head.%d", m_iMLState);
	if(m_bLfMenu) {
		BYLayerAncestor::ccTouchEnded(pTouch,pEvent);
		return;
	}
	if(m_Hmenu->mbShown){
		BYLayerAncestor::ccTouchEnded(pTouch,pEvent);
		return;
	}

	//bm->clear_mcaTarget();
	switch(m_iMLState){
	case(2):
		{
			CCLog(">[MapLayer] bm->get_bbattle() | %d", bm->get_bbattle());
			switch(bm->get_bbattle()){
			case(1):
				{
					bm->checkpoint(pTouch);
					if(!bm->m_eCurMouse) {

						showLeftFm();
						break;
					}else if(bm->m_eCurMouse->group_id == m_iCurGroup){
						// <[TD] ����|������������
						if(BattleField::sharedBattleField()->meConS != NULL && BattleField::sharedBattleField()->meConS != bm->m_eCurMouse) break;

						//bm->set_bbattle(2);
						bm->mbLockCamera = true;
						bm->f_setcontroller(bm->m_eCurMouse);
						bm->LockCamera(bm->m_eCurMouse);

						show_menu();	// <[DOUBLE_SM][D1] �ֶ����
						BattleField::sharedBattleField()->SetSrc((EChesses*) bm->m_controller);		

					}else if(bm->m_eCurMouse){
						CCLOG(">Show none controller info.");
					}
					break;
				}
			case(2):
				{
					CCLOG(">Click should be block.");
					//t_bm->setVisible(false);
					break;
				}
			case(3):							//Menu func is now waiting for bm click.
				{
					click_act();
					break;
				}
			case(7):
				{
					CtnSkill();
					break;
				}
			}
			BYLayerAncestor::ccTouchEnded(pTouch,pEvent);				
			break;
		}
	case(3):
		{
			CCLOG(">[TODO|5]Click&Show the EChesses's information.");
			break;
		}
	}

	
			
	return;
}

// <��Ȼ�ǽ�ItemBackȴ��֪Ϊʲô�����Ǳ����������ܵ��õ�
void MapLayer::ItemBack( CCObject* pSender )
{
	CCLog(">[MapLayer] ItemBack() | Use the Skill");
	
	EffectControler::sharedEffectControler()->SetCLock(((EChesses*) bm->m_controller)->m_pChara,((ItemCell*) pSender)->getTag());
	//EffectControler::sharedEffectControler()->m_src = BattleField::sharedBattleField()->meSrc->m_pChara;

	// <��鼼���Ƿ����ʹ��
	if(!EffectControler::sharedEffectControler()->DerLock()){

		if(!((EChessComp*) bm->m_controller->getComponent("controller"))->HaveAI())
			SoundManager::sharedSoundManager()->PlayDisablSFX();

		return;			
	}else{
		if(!((EChessComp*) bm->m_controller->getComponent("controller"))->HaveAI())
			SoundManager::sharedSoundManager()->PlayDecideSFX();
		//BattleField::sharedBattleField()->mbIsMagic = true;
	}

	// <��ö�����Я����ID
	m_iItem = ((ItemCell*) pSender)->getTag();
	// <���ؼ���ѡ���
	m_SkillList->setVisible(false);
	// <�û���־λ
	if(bm->get_bbattle() >= 6)
		bm->set_bbattle(7);
	else
		bm->set_bbattle(3);
	// <���usecase����
	m_iSUseCase = m_SkillList->getval(m_iItem)->m_iUseCase;//stoi(m_SkillList->getval("usecase",m_iItem));		
	bm->miRangeType = m_iSUseCase;
	// <���������������� 09-27
	bm->setLink(m_SkillList->getval(m_iItem)->m_iLink/*stoi(m_SkillList->getval("link",m_iItem))*/);
	//////////////////////////////////////////////////////////////////////////
	// <��ȡ��Χ����

	CCLog("[ML]Read Skill Range");
	
	string tsAddition;
	tsAddition = m_SkillList->getval(/*"additional",*/m_iItem)->msAddition;

	BMSkDrawMove(tsAddition);

	if(BattleField::sharedBattleField()->mCachedSPp.size()>0){
		if(BattleField::sharedBattleField()->mCachedSPp.count(m_iItem)){
			bm->m_mou_cur = BattleField::sharedBattleField()->mCachedSPp.at(m_iItem);
			bm->f_decide(BattleField::sharedBattleField()->mCachedSPp.at(m_iItem).x, BattleField::sharedBattleField()->mCachedSPp.at(m_iItem).y);
			CtnSkill();
		}else{	// <û��Ŀ��ļ���
			CtnSkill();
		}
		BattleField::sharedBattleField()->mCachedSPp.clear();
	}
	
	//EffectControler::sharedEffectControler()->md_use_skill(this,m_iItem,mpChara);		

}

void MapLayer::menu_back( CCObject* pSender )
{
	Container* t_c = (Container*) pSender;
	m_iFuncType = t_c->getTag();
	EChesses* t_ec = (EChesses*) bm->m_controller;				// <��������ɫ�ڵ�ͼ��ʼ��ֻ��һ��
	
	bm->miRangeType = 0;
	BattleField::sharedBattleField()->setBattle(false);
	if(m_SkillList) {
		m_SkillList->removeFromParent();
		CC_SAFE_RELEASE_NULL(m_SkillList);							// <��ֹ�Ҽ��㼶�п��ܳ��ֵ�bug
	}

	switch(m_iFuncType){
	case(1):
		{
			m_BattleMenu->setVisible(false);
			m_BattleMenu->miFlag = -1;
			m_BattleMenu->Refresh_Button();
			bm->ScrollBacKChess();
			bm->set_bbattle(1);
			
			break;
		}
	case(2):
		{
			m_BattleMenu->setVisible(false);
			m_BattleMenu->miFlag = -1;
			m_BattleMenu->Refresh_Button();
			BattleField::sharedBattleField()->ActionFac();

			((EChesses*) bm->m_controller)->m_pChara->miSHitBClass = 0;

			switch_control();
			break;
		}
	case(8):
		{
			//m_ldb should be cleaned at somewhere else or if you want to keep it, clean it here.
			m_BattleMenu->setVisible(false);
			m_BattleMenu->miFlag = -1;
			m_BattleMenu->Refresh_Button();
			//////////////////////////////////////////////////////////////////////////
			if(!show_skill_list()){// <�����б�����
				m_BattleMenu->setVisible(true);
			
			}else{
				//BattleField::sharedBattleField()->setBattle(true);
			}
			//Chara* mpChara = t_ec->m_pChara;
			//if(mpChara->m_viSkills.size() > 0){			//TODO: Not Showing skill-list/m_ldbEquList if there is no skill, modify it while needed.
			//	CCDictionary* m_cid = new CCDictionary();
			//	string t_sMask;
			//	for(map<int,int>::iterator it = mpChara->m_viSkills.begin(); it != mpChara->m_viSkills.end(); ++it)
			//	{
			//		int t_id = it->second;
			//		t_sMask +=  CCString::createWithFormat("%d,",t_id)->getCString();
			//		ItemCellData* t_icd	=	new ItemCellData(t_id,0,0);
			//		m_cid->setObject(t_icd,t_id);
			//		t_icd->autorelease();
			//	}
			//	t_sMask.erase(t_sMask.length()-1);
			//	CCString* t_csSql = CCString::createWithFormat("select * from skill_list where itemid IN (%s)",t_sMask.c_str());

			//	m_ldb = new ListDBView<ItemCell>(300,250, t_csSql->getCString(),m_cid, this,menu_selector(MapLayer::ItemBack),1);
			//	if(m_ldb->init()){
			//		m_ldb->setPosition(ccp(2,0));
			//		m_ldb->setContentSize(CCSizeMake(300,250));
			//		addChild(m_ldb);
			//	}else{
			//		CC_SAFE_RELEASE_NULL(m_ldb);
			//	}
			//}
			
			break;
		}
	case(16):		// <����
		{
			Chara* mpChara = t_ec->m_pChara;
			int t_iType = mpChara->getvalue("attack");		// <[TODO]�ӵ�λ�л�ù�������,�������Դ��Ҫ������ƽ��б����ע��Ĭ��ȡ�õ���0

			bm->set_bbattle(3);
			//BattleField::sharedBattleField()->setBattle(true);

			m_BattleMenu->setVisible(false);
			m_BattleMenu->Refresh_Button();

			vdata.clear();
			DBUtil::initDB("save.db");
			CCString* t_csSql = CCString::createWithFormat("select * from attr_attact where id = %d",t_iType);
			CCLog("%s",t_csSql->getCString());
			vdata = DBUtil::getDataInfo(t_csSql->getCString(),NULL);
			int m_number = vdata.size();
			DBUtil::closeDB(); 

			map<string,string> t_ssm = (map<string,string>) vdata.at(0);
			string tsAddition = t_ssm.at("additional");

			BMAtDrawMove(tsAddition);

			

			break;
		}
	case(32):		// <�ƶ�
		{
			bm->draw_moving_tile();
			bm->set_bbattle(3);
			m_BattleMenu->setVisible(false);
			m_BattleMenu->Refresh_Button();
			bm->m_mouse_type = 0;
			break;
		}
	}

}

void MapLayer::click_act()
{
	//if(bm->b_battle != 3) return;
	CCLog(">[MapLayer] click_act() | %d",m_iFuncType);
	switch(m_iFuncType){
	case(8):			//type == 4 | using skill
		{
			//[IN]ts_last;
			//[IN]m_iSUseCase;
			
			if(bm->testLink()){
				if(!BattleField::sharedBattleField()->mbIsInBattle){
					BattleField::sharedBattleField()->mbIsMagic = true;
					BattleField::sharedBattleField()->mbCurMagic = true;
				}
				
				ActSkill();
				
			}else{
				return;
			}

			break;
		}
	case 16:			// | Attack
		{
			if(bm->arange_targetwithtest(0)){
				
				map<string,string> t_ssm = (map<string,string>) vdata.at(0);

				BFsp->re_init();
				BFsp->parse_string(t_ssm.at("action_sp"));

				BattleField::sharedBattleField()->setBattle(true);
				BattleField::sharedBattleField()->ActionFac();
				//if(!BattleField::sharedBattleField()->mbIsInBattle){
				//	CCLog(">[MapLayer] click_act() | mbIsInBattle - False");
					BattleField::sharedBattleField()->mbIsMagic = false;
					BattleField::sharedBattleField()->mbCurMagic = false;
				//}

				
				bm->HandleScriptor(BFsp);
				bm->clean_cs();
				bm->set_bbattle(5);
				bm->m_bAnimateOver = false;
				
				//[0803]CCLog(">Reach here if every thing is right.....");
			}
			
			break;
		}
	case(32):				// type == 32 | move controller.
		{
			if(bm->move_control()){
				bm->set_bbattle(4);
				bm->m_bAnimateOver = false;
				//BattleField::sharedBattleField()->ActionFac();
				BattleField::sharedBattleField()->miState = 5;
				m_BattleMenu->dismiss();
				//bm->b_battle = 1;
				bm->clean_cs();
				CCLOG(">Prepare for moving.");
			}else{
				CCLOG(">Can not move.Stay in the state 3.");
				return;
			}			
			break;
		}
	}
	///switch_control();			// Move it, you may need a CCAction to control this, or put a flag in bm and watch it in upadte.
}

void MapLayer::f_init_battle()
{
	
	bm->m_controller = NULL;

	//CC_SAFE_RELEASE_NULL(m_lpJudgement);
	//CC_SAFE_RELEASE_NULL(m_etClock);

	//m_lpJudgement	=	new LinePrior();				//while bm is release, the two component should also be released null.
	//m_etClock		=	new ElementTicker();

	//m_lpJudgement->setAnchorPoint(ccp(0,0));
	//m_lpJudgement->setPosition(ccp(570,549));
	//addChild(m_lpJudgement,11);

	//m_etClock->setAnchorPoint(ccp(0,0));
	//m_etClock->setPosition(ccp(745,55));
	//addChild(m_etClock,11);
	
	//m_lpJudgement->f_init();
	//m_etClock->f_init();
	//m_lpJudgement->setactivator(this,menu_selector(MapLayer::TickBack));

	//CCDictElement* t_cde = NULL;
	//CCDictionary* t_cd = bm->m_itemlist;
	//CCDICT_FOREACH(t_cd,t_cde){
	//	EChesses* t_ec = (EChesses*) t_cde->getObject();
	//	m_lpJudgement->addPin(t_ec->m_pChara, t_ec->name);
	//}
	//m_lpJudgement->refresh_pin();

	//////////////////////////////////////////////////////////////////////////
	// <������ - ��չ�������������
	m_iMaxGroup = 2;
	m_iCurGroup = 0;		// <û��

	CCDictElement* t_cde = NULL;
	CCDictionary* t_cd = bm->m_itemlist;
	EChesses* t_ec;
	CCDICT_FOREACH(t_cd,t_cde){
		t_ec = (EChesses*) t_cde->getObject();
		t_ec->m_pChara->initValues();
	}

	m_iTurn = -1;
	m_bLfMenu = false;


	m_upArrow = NULL;
	m_bottomArrow = NULL;

	EventCenter::sharedEventCenter()->setBmCake(this);
	//m_iMLState = 3;					//state change to 3, wait for token.
}

//void MapLayer::TickBack( CCObject* pSender )
//{
//	CCLOG("Tick is back.");
//	m_iMLState = 2;
//
//	//m_iCSum = m_lpJudgement->m_caPinSteped->count();
//	m_iCCur = 0;
//
//	switch_control();
//}

void MapLayer::update( float fDelta )
{
	//if(mbAIControl) return;
	//CCLog(">[MapLayer] update() | %d", m_iMLState);
	switch(m_iMLState){
	case(2):	// User Token
		{
			do 
			{
				CC_BREAK_IF(bm->get_bbattle() != -1);	// < == -1 ��ǰ��
				CC_BREAK_IF(!bm->m_bAnimateOver);

				bm->control_switch();
				script_over();
			} while (0);
			break;
		}
	case(3):	// <ʱ��-TO������  --->>> ��� [��������Ȩ]
		{
			//m_lpJudgement->tick();
			//m_etClock->tick();
			if(!mbAutoLogic) return;		// <�Զ��߼����ز����Ƶڶ��㣬������ƻ�ѭ�������߼� || 

			if(!m_Spliter->mbShowed){
				m_Spliter->ShowRound();
				CCLog(">[MapLayer] ShowRound Over.");
				return;
			}
			BattleField::sharedBattleField()->meConS = nullptr;		// <�غ��л�ʱȡ������
			BattleField::sharedBattleField()->miState = 0;

			//////////////////////////////////////////////////////////////////////////
			// 
			
			m_iCurGroup = m_iCurGroup%m_iMaxGroup;		// <��ǰ��
			++m_iCurGroup;								// ....
			
			m_iMLState = 2;
			
			// <m_iTurn - �����ļ����ܻ��� ��miMaxGroup������Եõ��غ���
			if(m_iTurn>0){
				CCDictElement* tc;
				CCDictionary* te = bm->m_itemlist;
				CCDICT_FOREACH(te,tc){
					EChesses* ce = (EChesses*) tc->getObject();
					ce->setGreys(0);
					if(ce->group_id == m_iCurGroup){
						ce->m_pChara->roundUp();
					}
				}
			}
			++m_iTurn;
			CCLog(">[MapLayer]update()-m_iMLState:3 || Next Round:%d",m_iTurn);

			m_Spliter->mbShowed = false;
			switch_control();

			break;
		}
	}
}

void MapLayer::switch_control()
{
	if(m_iMLState < 2) return;
	if(tm != bm) return;
	//////////////////////////////////////////////////////////////////////////
	// <�غ�ת��
	if(mbAIControl) return;
	//if(!tm->cancontrol) return;
	CCLog(">[MapLayer]switchcontrol() || m_iCurGroup:%d",m_iCurGroup);
	BattleField::sharedBattleField()->miState = 0;

	mbSBProtect = true;

	CCDictElement* tc;
	CCDictionary* te = bm->m_itemlist;
	CCDICT_FOREACH(te,tc){
		EChesses* ce = (EChesses*) tc->getObject();
		if(ce->group_id == m_iCurGroup){
			if(m_iCurGroup == 1){
				// <[TD]��������
				if(BattleField::sharedBattleField()->meConS){
					
					bm->set_bbattle(1);
					FastShift();
					return;
				}

				int xd = ce->m_pChara->getvalue("b_xudong");
				if(!ce->m_pChara->checkXuDong()){
					bm->f_setcontroller(ce);
					bm->f_setcamara(ce);
					bm->set_bbattle(1);
					
					bm->mbLockCamera = false;
					CCLog(">[MapLayer] switch_control() | Set PC to switch.");
					//ce->m_pChara->setvalue("b_xudong",xd-1);

					return;
				}else{
					continue;
				}
			}else{
				//////////////////////////////////////////////////////////////////////////
				// <����ҵ�λ��ÿ���Ȩ
				CCLog(">[MapLayer]switchcontrol() || AIPlayer:%s . Check...", ce->name.c_str());

				int xd = ce->m_pChara->getvalue("b_xudong");
				if(!ce->m_pChara->checkXuDong()){
					CCLog(">[MapLayer]switchcontrol() || XuDong:%d",xd-1);
					bm->f_setcontroller(ce);
					bm->f_setcamara(ce);
					bm->set_bbattle(-1);					
					//ce->m_pChara->setvalue("b_xudong",xd-1);

					give_control();
					return;		// <ֱ���˳�����
				}else{
					CCLog(">[MapLayer]switchcontrol() || XuDong is zero Or No right to move.");
					continue;
				}
			}



		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	// <�غϲ����������޿ɿ��Ƶ�λ
	CCLog(">[MapLayer]switchcontrol()|| The group has no avalibale chesses.");
	if(m_iCurGroup != 1){
		m_iMLState = 3;
	}else{
		CCLog(">[MapLayer]switchcontrol()|| Turn is now over.Wait Player to press next.");
		m_iMLState = 2;
		bm->set_bbattle(1);
	}
	
}

void MapLayer::give_control()
{
	EChesses* t_ec = (EChesses*) bm->m_controller;
	bm->LockCamera(t_ec);


	switch(t_ec->group_id){
	case(0x01):					// <��ҿ��ƻ��
		{
			show_menu();	// <[DOUBLE_SM][D2] �Զ����
			BattleField::sharedBattleField()->SetSrc((EChesses*) bm->m_controller);	
			break;
		}
	case(0x02):					//TODO: Enemy now directly give up control, add AI here.
		{
			//switch_control();		
			//bm->set_bbattle(0);
			((AIComponent*) t_ec->getComponent("ai"))->giveControl();
			break;
		}
	}
}

void MapLayer::right_click()
{
	CCLog(">[MapLayer] right_click()");
	// <���HMENU�������ȵ� || �����Ҽ��߼�
	if(m_Hmenu->mbShown){
		if(m_bLfMenu){
			dissmissLeftFm();
		}
		return;
	}

	bm->mbLockCamera = false;
	switch(m_iMLState){
	case(2):{			// state 2 popup menu they may need to be canceled;

			switch(bm->get_bbattle()){
			case(1):
				{
					CCLog(">No need for right click.");
					break;
				}
			case(2):
				{
					if(m_ldb){
						m_BattleMenu->setVisible(true);
						m_ldb->setVisible(false);
						CC_SAFE_RELEASE_NULL(m_ldb);			// <����Ч�ʻ�ȡ�����
					}else if(m_SkillList){
						m_BattleMenu->setVisible(true);
						
						m_SkillList->setVisible(false);
						CC_SAFE_RELEASE_NULL(m_SkillList);			// <����Ч�ʻ�ȡ�����
					}else{
						m_BattleMenu->setVisible(false);
						m_BattleMenu->miFlag = -1;
						m_BattleMenu->Refresh_Button();
						bm->set_bbattle(1);
						CC_SAFE_RELEASE_NULL(m_BattleMenu);
						bm->ScrollBacKChess();
						if(m_lsb) m_lsb->setVisible(false);
					}
					break;
				}
			case(3):							//Menu func is now waiting for bm click.
				{
					bm->LockCamera(bm->m_controller);
					// [PE]: <ע������Ʒ���Ǽ���״̬��������ܻ���ַ��ش���
					if(m_ldb){
						m_ldb->setVisible(true);
					}else if(m_SkillList){
						m_SkillList->setVisible(true);
					}else{
						m_BattleMenu->setVisible(true);
					}
					if(m_rsb) 
						m_rsb->setVisible(false);
					bm->clean_cs();
					bm->clear_mcaTarget();
					BattleField::sharedBattleField()->miState = 0;

					bm->set_bbattle(2);
					Dissmiss_Arrows();
					break;
				}
			case(6):						// <���������б���н���
				{
					CCLog(">[ML] Over and continue.");
					if(!m_SkillList->isVisible()) break;
					m_SkillList->setVisible(false);
					ReleaseCLock();
					break;
				}
			case(7):
				{
					m_SkillList->setVisible(true);
					bm->clean_cs();
					bm->set_bbattle(6);
					Dissmiss_Arrows();
				}
			}
			
			break;

		}


	}
}

void MapLayer::script_over()
{
	//m_lpJudgement->removePin(t_ec->name);
	//bm->m_itemlist->removeObjectForKey(t_ec->name);
	//[TODO] <ע����������ĵ�����Դ�������ܻᱻ�Ƴ�
	m_lsb->setVisible(false);
	m_rsb->setVisible(false);

	m_lsb->SetContent(NULL);
	m_rsb->SetContent(NULL);

	if(BattleField::sharedBattleField()->IsOver()) {
		switch_to_walk();	// Change it~
		return;
	}
	switch_control();
}

// Test: Battleͨ����GameScene����msp�����ó������á��俪ʼ�ͽ��������뱣֤֪ͨ��GameScene��
void MapLayer::switch_to_battle( string s )
{
	m_iMLState = 2;				// <TODO |��EMERGENT | ʹ�ýŲ����г�ʼ�� [START] ����ת��ʼ�����ӿڿ��ƣ�ͬʱ����ͷ��������ű��ӿ�(f_setcamera || ��ʵ��)
	if(wm){
		wm->setVisible(false);
		wm->f_state_circle(false);
		wm->DeActive();

		show_hud();

		f_pauseall();
		f_stopcontrol();
	}
	openBattleMap(s.c_str(),0);
	
	m_Spliter->f_open();
	m_Spliter->ShowClock();

	bm->cancontrol = true;
	init_Stat();

	BattleField::sharedBattleField()->InitBF();

	//bm->b_battle = 3;
}

void MapLayer::switch_to_walk()
{
	close_hud();
	clearBattle();
	if(wm){
		EventCenter::sharedEventCenter()->setBmCake(NULL);

		BattleField::sharedBattleField()->Clean();
		BattleField::sharedBattleField()->ClearBF();
		BattleField::purgeSharedBattleField();
		
		//m_lpJudgement->removeFromParent();
		//m_etClock->removeFromParent();
		bm->DeActive();
		bm->removeFromParent();
		CC_SAFE_RELEASE_NULL(bm);

		//CC_SAFE_RELEASE_NULL(m_lpJudgement);
		//CC_SAFE_RELEASE_NULL(m_etClock);
		m_iItem = -1;

		tm = wm;
		f_resumeall();
		f_resumecon();

		EventCenter::sharedEventCenter()->setController(wm);

		wm->setVisible(true);
		wm->f_state_circle(true);
		wm->cancontrol = true;
		m_iMLState = 1;
		wm->EnActive();
		show_hud();
	}
}

void MapLayer::beforesnap()
{
	retain();
	removeFromParentAndCleanup(false);
	if(!wm) return;
	m_bSnap = wm->m_bAvalible;
	wm->f_state_circle(false);
}

void MapLayer::aftersnap()
{
	release();
	if(!wm) return;
	wm->f_state_circle(m_bSnap);
}

void MapLayer::snap()
{
	if(!wm) return;
	wm->visit();
	if(m_IP) m_IP->visit();
	if(m_ltb) m_ltb->visit();
}

//void MapLayer::RePack()
//{
//	CCLog(">[BF_ML]Trying to act the third attack...");
//	map<string,string> t_ssm = (map<string,string>) vdata.at(0);
//	BFsp->re_init();
//	BFsp->parse_string(t_ssm.at("action_sp"));
//
//	bm->HandleScriptor(BFsp);
//}

//////////////////////////////////////////////////////////////////////////
// <��ɫ״̬��ʾ
void MapLayer::init_Stat()
{
	if(!m_lsb){
		m_lsb = new RStatBar();
		m_lsb->autorelease();

		m_lsb->init();
		m_lsb->setPosition(ccp(475,399));

		addChild(m_lsb,98);

	}
	m_lsb->setVisible(false);

	if(!m_rsb){
		m_rsb = new LStatBar();
		m_rsb->autorelease();

		m_rsb->init();
		m_rsb->setPosition(ccp(0,399));

		addChild(m_rsb,98);
	}
	m_rsb->setVisible(false);
}

void MapLayer::DismissBHUD()
{
	if(m_lsb){
		CC_SAFE_RELEASE_NULL(m_lsb);
	}
	if(m_rsb){
		CC_SAFE_RELEASE_NULL(m_rsb);
	}
}

bool MapLayer::show_skill_list()
{
	if(!m_SkillList){
		m_SkillList = new SkillList();
		m_SkillList->f_init();
		m_SkillList->setAnchorPoint(ccp(0,1));
		m_SkillList->setPosition(0,400);
		m_SkillList->setactivator(this,menu_selector(MapLayer::ItemBack));
		
		addChild(m_SkillList,2);
	}


	if(m_SkillList->setChara(((EChesses*) bm->m_controller)->m_pChara)){
		m_SkillList->setVisible(true);
		return true;
	}else
	{
		// < [11-01][Notice] | ����ط�ò�Ʋ��ᱻ�õ�
		m_SkillList->setVisible(false);
		CC_SAFE_RELEASE_NULL(m_SkillList);
		return false;
	}
		
}

void MapLayer::showLeftFm()
{
	
	m_Hmenu->showLeftFM();
	if(!m_lfm){
		m_lfm  = new LeftMenu();			//TODO:Decide what to show by bit lock.
		m_Hmenu->addChild(m_lfm,LFM_ZORDER);
	}

	m_lfm->bitLock = 0xfff;
	m_lfm->setactivator(this,menu_selector(MapLayer::lfmenu_back));
	m_lfm->show();

	m_lfm->setAnchorPoint(CCPointZero);
	m_lfm->setPosition(46,105);
	m_lfm->setVisible(true);

	m_bLfMenu = true;
}

void MapLayer::dissmissLeftFm()
{
	m_Hmenu->dissmissLeftFM();
	m_lfm->setVisible(false);		// <��Ҫ�������ͷ�m_lfm�������¼����ᱻ�ƻ���
	m_bLfMenu = false;
}

void MapLayer::lfmenu_back( CCObject* pSender )
{
	Container* t_c = (Container*) pSender;
	int tiType = t_c->getTag();
	CCLog("[][][][%d",tiType);

	switch (tiType)
	{
	case 1:
		{
			m_iMLState = 3;
			dissmissLeftFm();
			break;
		}
	case 2:
		{
			m_Hmenu->showSB();
			break;
		}
	default:
		break;
	}
}

void MapLayer::Show_Arrows(int aiu, int aib)
{

	CCLog("[%d,%d]",aiu,aib);
	if(m_upArrow) m_upArrow->removeFromParent();
	if(m_bottomArrow) m_bottomArrow->removeFromParent();

	//m_upArrow = CCSprite::create("Images/arrow_blood_left.png");
	//m_bottomArrow = CCSprite::create("Images/arrow_blood_left.png");

	//////////////////////////////////////////////////////////////////////////
	// <��ʾ

	m_upArrow = CCSprite::create(sArrowPath[aiu]);
	m_bottomArrow =  CCSprite::create(sArrowPath[aib]);

	m_upArrow->setFlipX(true);
	m_upArrow->setPosition(ccp(445,565));
	m_bottomArrow->setPosition(ccp(445,490));
	addChild(m_upArrow,3);
	addChild(m_bottomArrow,3);
}

void MapLayer::Dissmiss_Arrows()
{
	if(m_upArrow) m_upArrow->removeFromParent();
	if(m_bottomArrow) m_bottomArrow->removeFromParent();
	m_bottomArrow = NULL;
	m_upArrow = NULL;
}

//////////////////////////////////////////////////////////////////////////
// <����

bool MapLayer::SC_Popup(const char* asName, int aiFlag)
{
	bm->PauseAllActs();
	bm->cancontrol = true;
	bm->set_bbattle(6);

	//m_pActionManager->pauseTarget(this);
	//f_pauseall();		// < �����ã���Ҫ����ÿ��entile����ͣ = = �����ܵ��˽ڵ��ϵ��Ӱ�죬����һ�º�����
	
	bool ret = false;
	do 
	{

		ret = popup_ctn(asName,aiFlag);
	} while (false);
	if(!ret) {		
		//f_resumeall();		// <�ָ����еĻ��
		//m_pScheduler->resumeTarget(this);
		//m_pActionManager->resumeTarget(this);
		CCLog(">[MapLayer] Tring to pop mask but faild.");
		ReleaseCLock();
	}
		
	return ret;
}

// <�����������ʱ��Ҫִ�еĲ���
void MapLayer::ReleaseCLock()
{
	CCLog("[MapLayer] Release C Lock.");
	BattleField::sharedBattleField()->ResumeJudge();

	if(BattleField::sharedBattleField()->mbNeLock){
		BattleField::sharedBattleField()->mbNeLock = false;
		((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRelease();
	}

	bm->cancontrol = false;
	bm->ResumeAllActs();
	bm->set_bbattle(-1);		// [AT] <����
	mlgStat->GiveUpCont();
}

void MapLayer::CtnSkill()
{
	CCLog(">[MapLayer] CtnSkill() | Begins.");
	// < ���ڷ����飬��������������ڵ�һ�ε�λ���з�����ʱ����еġ�
	
	// < Case 1: ��ϴ | Ĭ����Ϊ 
	// < BattleMap::HandleScriptor ֮�󱻹�����Ŀ������ᱻ�޸�
	if(bm->testLink()){
		if(BattleField::sharedBattleField()->mbNeLock){
			BattleField::sharedBattleField()->mbNeLock = false;
			ReleaseCLock();
			BattleField::sharedBattleField()->mbNeLock = true;
		}else{
			ReleaseCLock();
		}
		
		CCLog(">[MapLayer] CtnSkill() | Prepare for EC-SkillUsing.");
		if(((EChessComp*) ((EChesses*) bm->m_controller)->getComponent("controller"))->mbThinked) ((EChessComp*) ((EChesses*) bm->m_controller)->getComponent("controller"))->miReState = 2;
		
		bm->clean_cs();
		EffectControler::sharedEffectControler()->md_use_skill(this,m_iItem,((EChesses*) bm->m_controller)->m_pChara);		

		bm->set_bbattle(5);
		bm->m_bAnimateOver = false;
	}else{
		CCLog(">[MapLayer] CtnSkill() | Skill failed test no link.");
		//ReleaseCLock();
		return;
	}
}

// <�����˵�
bool MapLayer::popup_ctn(const char* sMask,int aiFlag)
{
	int tai = ((EChessComp*) bm->m_controller->getComponent("controller"))->HaveAI();
	CCLog(">[MapLayer] popup_ctn() | AI Control: %d", tai);
	//if(tai){
	//	// <AI����
	//	
	//	CCLog(">[MapLayer] popup_ctn() | AI Control Give Out");
	//	return false;
	//}

	// <��״̬����ʹ�ý����༼��
	if(EventCenter::sharedEventCenter()->mbDefendLock && !tai) return false;
	
	CCLog(">[MapLayer] popup_ctn() | Player Control Give In");

	if(!m_SkillList){
		m_SkillList = new SkillList();
		m_SkillList->f_init();
		m_SkillList->setAnchorPoint(ccp(0,1));
		m_SkillList->setPosition(0,400);
		m_SkillList->setactivator(this,menu_selector(MapLayer::ItemBack));

		addChild(m_SkillList,2);
		
	}
	m_SkillList->miFlag = aiFlag;
	if(m_SkillList->setctns(((EChesses*) bm->m_controller)->m_pChara,sMask)){

		if(tai){
			m_SkillList->setVisible(false);
			m_SkillList->randomact();
			return true;
		}else{
			m_SkillList->setVisible(true);
			mlgStat->CountDown(3.0, this, menu_selector(MapLayer::CountBack));
			return true;
		}
		
	}else
	{
		// < [11-01][Notice] | ����ط�ò�Ʋ��ᱻ�õ�
		m_SkillList->setVisible(false);
		CC_SAFE_RELEASE_NULL(m_SkillList);
		return false;
	}

}

//////////////////////////////////////////////////////////////////////////
void MapLayer::releaseControl()
{
	CCLog(">[MapLayer] ReleaseControl()");
	if(m_iMLState < 2) return;
	AIUnLock();
	runAction(CCSequence::create(CCDelayTime::create(0), CCCallFunc::create(this, callfunc_selector(MapLayer::switch_control)), NULL));
}

void MapLayer::AILock()
{
	bm->set_bbattle(8);
	mbAIControl = true;
}

void MapLayer::AIUnLock()
{
	mbAIControl = false;
}

//////////////////////////////////////////////////////////////////////////

void MapLayer::BMAtDrawMove( string &tsAddition )
{
	// < ����ֱ�ӵ��� sk
	BMSkDrawMove(tsAddition);
	return;

	// < ...
	//stringstream ss;
	//ss<<tsAddition;
	//int k;//,j;
	//vector<int> ars;
	//CCLog("Ready for dest.");

	/////////////
	//ss>>k;
	//switch (k)
	//{
	//case 1:
	//	{
	//		ss>>k;
	//		ars.push_back(k);
	//		bm->draw_skill_range(1,ars);
	//		break;
	//	}
	//default:
	//	break;
	//}
	//ars.clear();

	/////////////
	//ss>>k;
	//switch (k)
	//{
	//case 1:
	//	{
	//		ss>>k;
	//		ars.push_back(k);
	//		bm->set_mouse_range(1,ars);
	//		break;
	//	}
	//default:
	//	break;
	//}
}

void MapLayer::BMSkDrawMove( string tsAddition )
{
	mbIsDirect = false;
	stringstream ss;
	ss<<tsAddition;

	int k;//,j;
	vector<int> ars;

	///////////
	ss>>k;
	switch (k)
	{
	case 0:
		{
			CCLog(">[MapLayer] BMSkDrawMove() | Skill type directly run.");
			mbIsDirect = true;
			if(((EChessComp*) ((EChesses*) bm->m_controller)->getComponent("controller"))->mbThinked && !SkillJudge::sharedSkillJudge()->mbJudging) ((EChessComp*) ((EChesses*) bm->m_controller)->getComponent("controller"))->miReState = 2;
			bm->find_target_arrage(-99);

			if(mbCheckLock) return;
			if(!m_SkillList) return;
			if(m_iItem>=0){
				CCLog(">[MapLayer] BMSkDrawMove() | %d | addit check", m_iItem);
				int ti = m_SkillList->getval(m_iItem)->m_iUseCase;

				if(ti>10){	// <ֱ�����Buffer�ļ��� | û����Ч
					/*((EChesses*) bm->m_controller)->m_pChara->LoadBuffer(ti);*/
					BufferSkill(m_iItem, bm->m_controller);

					((EChesses*)  bm->m_controller)->m_pChara->miSeType = 4;
					BattleField::sharedBattleField()->PlaySe(((EChesses*)  bm->m_controller)->m_pChara);

					if(((EChessComp*) ((EChesses*) bm->m_controller)->getComponent("controller"))->mbThinked && !SkillJudge::sharedSkillJudge()->mbJudging) ((EChessComp*) ((EChesses*) bm->m_controller)->getComponent("controller"))->miReState = 2;
					ReleaseCLock();
					return;
				}
			}


			
			if(EGroup::sharedEGroup()->CheckGrp(bm->m_controller)) {		
				if(!BattleField::sharedBattleField()->mbIsInBattle) BattleField::sharedBattleField()->mbIsMagic = true;
				BattleField::sharedBattleField()->mbCurMagic = true;

				ActSkill();	
				ReleaseCLock();
			}else{
				mbIsDirect = true;
			}
			return;
			break;
		}
	case 1:		// <Բ��
		{
			ss>>k;
			ars.push_back(k);
			bm->draw_skill_range(1,ars);
			break;
		}
	case 2:		// <ʮ�ּ�
		{
			ss>>k;
			ars.push_back(k);
			ss>>k;
			ars.push_back(k);
			bm->draw_skill_range(2,ars);
			break;
		}
	case 3:		// <����
		{
			ss>>k;
			ars.push_back(k);
			ss>>k;
			ars.push_back(k);
			bm->draw_skill_range(3,ars);
			break;
		}
	default:
		bm->draw_skill_range(k,ars);
		break;
	}
	ars.clear();

	///////////
	ss>>k;
	switch (k)
	{
	case -1:
		{
			ss>>k;
			ars.push_back(k);
			bm->set_mouse_range(-1,ars);
			break;
		}
	case 1:
		{
			ss>>k;
			ars.push_back(k);
			bm->set_mouse_range(1,ars);
			break;
		}
	case 2:
		{
			ss>>k;
			ars.push_back(k);
			ss>>k;
			ars.push_back(k);
			bm->set_mouse_range(2,ars);
			break;
		}
	case 3:	// <��������ľ���
		{
			bm->set_mouse_range(3,ars);
			break;
		}
	case 4:
		{
			ss >> k;
			ars.push_back(k);
			ss >> k;
			ars.push_back(k);
			ss >> k;
			ars.push_back(k);
			bm->set_mouse_range(4,ars);
			break;
		}
	default:
		bm->set_mouse_range(k,ars);
		break;
	}

}

void MapLayer::showUpgrd(EChesses* aeTar)
{
	CCLog("+++ showUpgrd");
	m_Hmenu->showRightUpg(aeTar);

	m_lsb->setVisible(false);
	m_rsb->setVisible(false);
	Dissmiss_Arrows();

	//((MapLayerComp*) getComponent("controller"))->ActRetain();
}

void MapLayer::clearBattle()
{
	((MapLayerComp*) getComponent("controller"))->MlUnLock();
	if(m_Spliter){
		m_Spliter->removeFromParentAndCleanup(false);
		//Spliter::purgeSharedSpliter();
		CC_SAFE_RELEASE_NULL(m_Spliter);
		m_Spliter = NULL;
	}
	
	if(m_Hmenu){
		m_Hmenu->dissmissLeftFM();
		m_Hmenu->removeFromParentAndCleanup(false);
		CC_SAFE_RELEASE_NULL(m_Hmenu);
		//HMenu::purgeSharedHMenu();
		m_Hmenu = NULL;
	}

	m_bLfMenu = false;

}

void MapLayer::ActSkill()
{
	CCLOG(">Prepare for EC-SkillUsing.");

	BattleField::sharedBattleField()->setBattle(true);
	BattleField::sharedBattleField()->ActionFac();
	bm->clean_cs();

	EffectControler::sharedEffectControler()->md_use_skill(this,m_iItem,((EChesses*) bm->m_controller)->m_pChara);			// <[TODO]�����޸���ڵ㣬������EC�³����е�sp��
	bm->set_bbattle(5);
	bm->m_bAnimateOver = false;

	// <�趨���ܱ�־λ || �ɽ����ļ��ܽ���洢��script��
	((EChesses*) bm->m_controller)->m_pChara->cur_sid = m_iItem;
	((EChesses*) bm->m_controller)->m_pChara->cur_shit = false;

	
}
//
//void MapLayer::PunpOut()
//{
//	BattleField::sharedBattleField()->PunpOut();
//}

void MapLayer::CountBack( CCObject* pSender ){
	m_SkillList->setVisible(false);
	bm->clean_cs();
	bm->set_bbattle(6);
	Dissmiss_Arrows();
	ReleaseCLock();
}

void MapLayer::FastShift()
{
	CCLog(">[MapLayer] FastShift() ");
	if(mbAIControl) return;
	if(m_bLfMenu) return;
	switch(m_iMLState){
	case(2):
		{
			CCLog(">[MapLayer] bm->get_bbattle() | %d", bm->get_bbattle());
			switch(bm->get_bbattle()){
			case(2):
				{
					if(m_ldb){
						return;
					}else if(m_SkillList){
						return;
					}else{
						m_BattleMenu->setVisible(false);
						m_BattleMenu->miFlag = -1;
						m_BattleMenu->Refresh_Button();
						bm->set_bbattle(1);
						CC_SAFE_RELEASE_NULL(m_BattleMenu);
						bm->ScrollBacKChess();
						if(m_lsb) m_lsb->setVisible(false);
					}
			
				}
			case(1):
				{
					if(!BattleField::sharedBattleField()->meConS)
					{
						bm->m_controller = (Entiles*) BattleField::sharedBattleField()->meConS;

						CCDictElement* tc;
						CCDictionary* te = bm->m_itemlist;
						bool tMatch = false;
						EChesses* ttaebefor = nullptr;	// <��һ��
						EChesses* ttaeafter = nullptr;

						CCLog(">[+++] %d", te->count());
						CCDICT_FOREACH(te,tc){
							EChesses* ce = (EChesses*) tc->getObject();
							if(ce->group_id != m_iCurGroup) continue;

							if(ce != bm->m_controller){
								if(!ce->m_pChara->checkXuDong()){
									if(!ttaebefor) ttaebefor = ce; // <��¼��һ��ƥ��ķ�c��λ
									if(tMatch){
										ttaeafter = ce;				// <��¼c֮��ĵ�һ����λ
										break;
									}
								}
							}else{
								tMatch = true;
							}
						}

						if(!ttaeafter && !tMatch) return;
						if(!ttaeafter) ttaeafter = ttaebefor;

						if(ttaeafter){
							bm->f_setcontroller(ttaeafter);
							bm->f_setcamara(ttaeafter);
							bm->set_bbattle(1);
						}

						if(bm->m_controller->group_id != m_iCurGroup) return;
					}else{
						if(((EChesses*) BattleField::sharedBattleField()->meConS)->m_pChara->checkXuDong()){
							((EChesses*) BattleField::sharedBattleField()->meConS)->setGrey(1);
							BattleField::sharedBattleField()->meConS = NULL;
							CCLog(">[MapLayer] FastShift() | Last control can not move.");
							return;
						}
						bm->f_setcontroller((EChesses*) BattleField::sharedBattleField()->meConS);
						//bm->f_setcamara(BattleField::sharedBattleField()->meConS);
						bm->set_bbattle(1);
					}

				

					bm->LockCamera(bm->m_controller);
					bm->m_touch = new CCTouch();
					bm->m_touch->setTouchInfo(0, ((EChesses*) bm->m_controller)->getPositionX(), ((EChesses*) bm->m_controller)->getPositionY() + 100);


					show_menu();	// <[DOUBLE_SM][D3] �������
					BattleField::sharedBattleField()->SetSrc((EChesses*) bm->m_controller);	
					break;

				}
			}
			break;
		}
	}
}

void MapLayer::BufferSkill(int aiSid, CCObject* aSrc)
{
	EChesses* teSrc = (EChesses*) aSrc;
	Chara* tow = teSrc->m_pChara;
	Chara* t_tar = BattleField::sharedBattleField()->meOrig->m_pChara;
	if(t_tar == tow){
		t_tar = BattleField::sharedBattleField()->meSrc->m_pChara;
	}
	//if(t_tar == tow) exit(99);

	BSkill* tbs = m_SkillList->getval(m_iItem);
	int ti = tbs->m_iUseCase;

	BFsp->re_init();
	
	BFsp->parse_string(tbs->msScript);
	CCArray* t_caLocks = BFsp->m_caScript;

	BattleField::sharedBattleField()->RefreshStats();

	for(int i = 0; i<t_caLocks->count(); ++i){
		Script* t_scp = (Script*) t_caLocks->objectAtIndex(i);
		switch(t_scp->getint("type")){
		case 1:
			{
					
				string tname =  t_scp->getstring("name");
				int tiv = -t_scp->getint("value");		// <Lock�ڲ���¼��������ֵ | ���Դ��븺ֵ

				tow->addBatValue(tname, tiv);
				break;
			}
		case 2:		// <��ʱ�����Ч��| Ĭ����ԴΪmeSrc
			{
				//t_tar = ;
				break;
			}
		}
	}

	tow->LoadBuffer(ti,0,t_tar);
	BattleField::sharedBattleField()->RefreshStats();
	BFsp->re_init();
}

void MapLayer::DelaySound(Script* ascp, CCObject* ae)
{
	float tfDelay = ascp->getfloat("pause");

	mDSEChess = ae;
	EChesses* te = (EChesses*) ae;

	float t_sDelay = ascp->getfloat("time");
	string tsLastVoice = ascp->getstring("value");
	CCLog(">Ready for sound playing:%s,%f", tsLastVoice.c_str(), t_sDelay);
	SoundManager::sharedSoundManager()->PlayEffect(tsLastVoice.c_str());

	bm->PauseForVod();
	runAction(CCSequence::create(CCDelayTime::create(tfDelay), CCCallFunc::create(this, callfunc_selector(MapLayer::ReleaseSound)), NULL));

}

void MapLayer::ReleaseSound()
{
	bm->ResuemForVod();
}
