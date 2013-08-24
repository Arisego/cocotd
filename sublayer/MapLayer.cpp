#include "sublayer/MapLayer.h"
#include "utils/EffectControler.h"
#include "SingleTon/BattleFiled.h"

USING_NS_CC;

#define WM_Z 1
#define BM_Z 2
#define SL_Z 3

//void MapLayer::registerWithTouchDispatcher()
//{
//	CCDirector* pDirector = CCDirector::sharedDirector();
//	pDirector->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority, true);
//}

MapLayer::~MapLayer(){
	CCLOG(">MapLayer Destruct.");
	

	BFsp->re_init();
	delete BFsp;
	removeAllChildren();
	

	CC_SAFE_RELEASE_NULL(bm);
	CC_SAFE_RELEASE_NULL(wm);

	CC_SAFE_RELEASE_NULL(t_bm);
	CC_SAFE_RELEASE_NULL(m_ldb);
	//CC_SAFE_RELEASE_NULL(m_lpJudgement);
	//CC_SAFE_RELEASE_NULL(m_etClock);

	//
}

void MapLayer::LoadMap(const char* pszN, const char* pscR, Script* tp){
	removeAllChildren();
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
	BattleField::sharedBattleField()->InitChessRefreh();
	bm->init();
	addChild(bm,BM_Z);
	tm = bm;
	f_init_battle();
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
	t_bm  =	 NULL;
	m_ldb =	 NULL;

	m_lsb = NULL;
	m_rsb = NULL;

	//m_lpJudgement	=	NULL;
	//m_etClock		=	NULL;
	//m_sCon			=	NULL;

	BFsp = new Scriptor();

	m_iMLState = -1;
	//scheduleUpdate();
	MapLayerComp* mlp = new MapLayerComp();
	addComponent(mlp);
	mlp->autorelease();
}

void MapLayer::f_pauseall(){
	if(!tm) return;
	tm->pauseSchedulerAndActions();
}

void MapLayer::f_stopcontrol(){
	if(!tm) return;
	tm->clearcontrol();
}

void MapLayer::f_resumeall(){
	if(!tm) return;
	tm->resumeSchedulerAndActions();
}

void MapLayer::f_resumecon(){
	if(!tm) return;
	tm->cancontrol = true;
}

void MapLayer::Close(){		//关闭显示和地图切换由别的函数负责
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
	default:
		{
			if(m_ltb) m_ltb->setVisible(false);
			if(m_IP) m_IP->setVisible(false);
			CCLOG("[PE]>unknown maplayer state:%d",m_iMLState);
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
			CCLOG("Show_Test FOR ALL THE TARGET.");			//参照WM的showtext，注意所有的特效都不保留引用，所以无法应用存档功能。
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

// <技能生效点
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

void MapLayer::show_menu()
{
	if(!t_bm){
		t_bm = new BattleMenu();			//TODO:Decide what to show by bit lock.
		addChild(t_bm);
	}

	BattleField::sharedBattleField()->SetSrc((EChesses*) bm->m_controller);				// <尝试移动到控制交接的函数里

	t_bm->setAnchorPoint(CCPointZero);
	t_bm->setPosition(200,200);
	t_bm->bitLock = 0xff;
	t_bm->setactivator(this,menu_selector(MapLayer::menu_back));
	if(m_lsb) m_lsb->setVisible(true);
	t_bm->show();
	t_bm->setVisible(true);
	bm->b_battle = 2;
}

void MapLayer::ccTouchEnded( CCTouch *pTouch, CCEvent *pEvent )
{
	switch(m_iMLState){
	case(2):
		{
			switch(bm->b_battle){
			case(1):
				{
					bm->checkpoint(pTouch);
					if(!bm->m_eCurMouse) break;
					if(bm->m_eCurMouse->group_id == m_iCurGroup){
						show_menu();

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

// <虽然是叫ItemBack却不知为什么现在是被用来做技能调用的
void MapLayer::ItemBack( CCObject* pSender )
{
	CCLOG(">ItemBack....We use the skill directly.");
	
	if(!EffectControler::sharedEffectControler()->DerLock()) return;			// <如果技能使用条件不符则不起作用，注意播放声音的接口放在这里

	// <获得对象所携带的ID
	m_iItem = ((ItemCell*) pSender)->getTag();
	// <隐藏技能选择框
	m_ldb->setVisible(false);
	// <置换标志位
	bm->b_battle = 3;
	// <获得usecase参数
	m_iSUseCase = stoi(m_ldb->getval("usecase",m_iItem));		//No use; Test Only. Read out range and effect_dis from database "additional". They should be stored in pair of int value.
	bm->miRangeType = m_iSUseCase;

	vector<int> ars;
	ars.push_back(7);
	bm->draw_skill_range(1,ars);							//TODO:TEST ONLY: Type & ars are from db. simply use stringstream to read pair
	ars.clear();
	ars.push_back(3);
	bm->set_mouse_range(1,ars);								//TODO:TEST ONLY: Type & ars are from db. simply use stringstream to read pair

	//EffectControler::sharedEffectControler()->md_use_skill(this,m_iItem,mpChara);		

}

void MapLayer::menu_back( CCObject* pSender )
{
	Container* t_c = (Container*) pSender;
	m_iFuncType = t_c->getTag();
	EChesses* t_ec = (EChesses*) bm->m_controller;				// <控制器角色在地图上始终只有一个
	
	bm->miRangeType = 0;
	BattleField::sharedBattleField()->setBattle(false);

	switch(m_iFuncType){
	case(1):
		{
			t_bm->setVisible(false);
			t_bm->miFlag = -1;
			t_bm->Refresh_Button();
			bm->b_battle = 1;
			
			break;
		}
	case(2):
		{
			t_bm->setVisible(false);
			t_bm->miFlag = -1;
			t_bm->Refresh_Button();
			switch_control();
			break;
		}
	case(4):
		{
			//m_ldb should be cleaned at somewhere else or if you want to keep it, clean it here.
			t_bm->setVisible(false);
			t_bm->miFlag = -1;
			t_bm->Refresh_Button();
			//////////////////////////////////////////////////////////////////////////

			Chara* mpChara = t_ec->m_pChara;
			if(mpChara->m_viSkills.size() > 0){			//TODO: Not Showing skill-list/m_ldbEquList if there is no skill, modify it while needed.
				CCDictionary* m_cid = new CCDictionary();
				string t_sMask;
				for(map<int,int>::iterator it = mpChara->m_viSkills.begin(); it != mpChara->m_viSkills.end(); ++it)
				{
					int t_id = it->second;
					t_sMask +=  CCString::createWithFormat("%d,",t_id)->getCString();
					ItemCellData* t_icd	=	new ItemCellData(t_id,0,0);
					m_cid->setObject(t_icd,t_id);
					t_icd->autorelease();
				}
				t_sMask.erase(t_sMask.length()-1);
				CCString* t_csSql = CCString::createWithFormat("select * from skill_list where itemid IN (%s)",t_sMask.c_str());

				m_ldb = new ListDBView<ItemCell>(300,250, t_csSql->getCString(),m_cid, this,menu_selector(MapLayer::ItemBack),1);
				if(m_ldb->init()){
					m_ldb->setPosition(ccp(2,0));
					m_ldb->setContentSize(CCSizeMake(300,250));
					addChild(m_ldb);
				}else{
					CC_SAFE_RELEASE_NULL(m_ldb);
				}
			}
			BattleField::sharedBattleField()->setBattle(true);
			break;
		}
	case(16):		// <攻击
		{
			Chara* mpChara = t_ec->m_pChara;
			int t_iType = mpChara->getvalue("attack");		// <[TODO]从单位中获得攻击属性,具体的来源需要根据设计进行变更，注意默认取得的是0

			bm->b_battle = 3;

			t_bm->setVisible(false);
			t_bm->Refresh_Button();

			t_iType = 1;								// <[TestOnly] 使用上面获取的值

			vdata.clear();
			DBUtil::initDB("save.db");
			CCString* t_csSql = CCString::createWithFormat("select * from attr_attact where id = %d",t_iType);
			vdata = DBUtil::getDataInfo(t_csSql->getCString(),NULL);
			int m_number = vdata.size();
			DBUtil::closeDB(); 

			map<string,string> t_ssm = (map<string,string>) vdata.at(0);	

			vector<int> ars;
			ars.push_back(7);
			bm->draw_skill_range(stoi(t_ssm.at("type_id")),ars);							//	<攻击的范围绘制			arrs -- 根据攻击配置的不同这个值需要从存档中读取
			ars.clear();
			ars.push_back(3);
			bm->set_mouse_range(stoi(t_ssm.at("range_type_id")),ars);								//	<定义鼠标选择图形

			BattleField::sharedBattleField()->setBattle(true);

			break;
		}
	case(32):		// <移动
		{
			bm->draw_moving_tile();
			bm->b_battle = 3;
			t_bm->setVisible(false);
			t_bm->Refresh_Button();
			bm->m_mouse_type = 0;
			break;
		}
	}

}

void MapLayer::click_act()
{
	//if(bm->b_battle != 3) return;
	switch(m_iFuncType){
	case(4):			//type == 4 | using skill
		{
			//[IN]ts_last;
			//[IN]m_iSUseCase;
			if(bm->arange_targetwithtest(m_iSUseCase)){
				CCLOG(">Prepare for EC-SkillUsing.");
				bm->clean_cs();
				EffectControler::sharedEffectControler()->md_use_skill(this,m_iItem,((EChesses*) bm->m_controller)->m_pChara);			// <[TODO]技能修改入口点，尝试让EC吐出所有的sp？
				bm->b_battle = 5;
				bm->m_bAnimateOver = false;
			}else{
				return;
			}

			break;
		}
	case 16:
		{
			if(bm->arange_targetwithtest(0)){
				
				map<string,string> t_ssm = (map<string,string>) vdata.at(0);

				BFsp->re_init();
				BFsp->parse_string(t_ssm.at("action_sp"));

				bm->HandleScriptor(BFsp);
				bm->clean_cs();
				bm->b_battle = 5;
				bm->m_bAnimateOver = false;

				//[0803]CCLog(">Reach here if every thing is right.....");
			}
			
			break;
		}
	case(32):				// type == 32 | move controller.
		{
			if(bm->move_control()){
				bm->b_battle = 4;
				bm->m_bAnimateOver = false;
				t_bm->dismiss();
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
	// <组别控制 - 扩展从这里读入配置
	m_iMaxGroup = 2;
	m_iCurGroup = 0;		// <没零

	CCDictElement* t_cde = NULL;
	CCDictionary* t_cd = bm->m_itemlist;
	EChesses* t_ec;
	CCDICT_FOREACH(t_cd,t_cde){
		t_ec = (EChesses*) t_cde->getObject();
		t_ec->m_pChara->initValues();
	}

	m_iTurn = 0;

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
	switch(m_iMLState){
	case(2):
		{
			do 
			{
				CC_BREAK_IF(bm->b_battle != -1);
				CC_BREAK_IF(!bm->m_bAnimateOver);

				bm->control_switch();
				script_over();
			} while (0);
			break;
		}
	case(3):	// <时钟-TO不可用  --->>> 变更 [交换控制权]
		{
			//m_lpJudgement->tick();
			//m_etClock->tick();

			//////////////////////////////////////////////////////////////////////////
			m_iCurGroup = m_iCurGroup%m_iMaxGroup;

			++m_iCurGroup;
			m_iMLState = 2;
			switch_control();
			if(m_iTurn>0){
				CCDictElement* tc;
				CCDictionary* te = bm->m_itemlist;
				CCDICT_FOREACH(te,tc){
					EChesses* ce = (EChesses*) tc->getObject();
					if(ce->group_id == m_iCurGroup){
						ce->m_pChara->roundUp();
					}
				}
			}
			++m_iTurn;

			break;
		}
	}
}

void MapLayer::switch_control()
{
	////if(m_sCon) m_sCon->replace_pin();
	//m_lsb->setVisible(false);
	//m_rsb->setVisible(false);
	//if(m_iCCur < m_iCSum){
	//	//m_sCon = (LinePin*) m_lpJudgement->m_caPinSteped->objectAtIndex(m_iCCur);

	//	//bm->f_setcontroller(bm->m_getEntile(m_sCon->name.c_str()));
	//	//bm->f_setcamara(bm->m_getEntile(m_sCon->name.c_str()));

	//	m_iCCur++;
	//	give_control();
	//}else{
	//	EventCenter::sharedEventCenter()->setBmCake(NULL);
	//	//m_sCon = NULL;
	//	m_iMLState = 3;
	//}
	//////////////////////////////////////////////////////////////////////////
	// <回合转换

	
	CCDictElement* tc;
	CCDictionary* te = bm->m_itemlist;
	CCDICT_FOREACH(te,tc){
		EChesses* ce = (EChesses*) tc->getObject();
		if(ce->group_id == m_iCurGroup){
			int xd = ce->m_pChara->getvalue("b_xudong");
			if(xd>0){
				bm->f_setcontroller(ce);
				bm->f_setcamara(ce);
				bm->b_battle = 1;
				//ce->m_pChara->setvalue("b_xudong",xd-1);
				
				return;
			}else{
				continue;
			}


		}
	}


	m_iMLState = 3;
}

void MapLayer::give_control()
{
	EChesses* t_ec = (EChesses*) bm->m_controller;

	switch(t_ec->group_id){
	case(0x01):					// <玩家控制获得
		{
			show_menu();
			break;
		}
	case(0x02):					//TODO: Enemy now directly give up control, add AI here.
		{
			switch_control();		
			break;
		}
	}
}

void MapLayer::right_click()
{
	if(!tm->cancontrol) return;
	switch(m_iMLState){
	case(2):{			// state 2 popup menu they may need to be canceled;

			switch(bm->b_battle){
			case(1):
				{
					CCLOG(">No need for right click.");
					break;
				}
			case(2):
				{
					if(!m_ldb){					//second layer also stay in state 2,may be control it by func_type is more clever?
						t_bm->setVisible(false);
						t_bm->miFlag = -1;
						t_bm->Refresh_Button();
						bm->b_battle = 1;

						if(m_lsb) m_lsb->setVisible(false);
					}else{
						t_bm->setVisible(true);
						m_ldb->setVisible(false);
						CC_SAFE_RELEASE_NULL(m_ldb);			// <牺牲效率换取简便性
					}

					break;
				}
			case(3):							//Menu func is now waiting for bm click.
				{
					if(m_ldb){
						m_ldb->setVisible(true);
					}else{
						t_bm->setVisible(true);
					}
					if(m_rsb) m_rsb->setVisible(false);
					bm->clean_cs();
					bm->b_battle = 2;
					break;
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
	//[TODO] <注意这个函数的调用来源，它可能会被移除
	m_lsb->setVisible(false);
	m_rsb->setVisible(false);

	if(BattleField::sharedBattleField()->IsOver()) {
		switch_to_walk();	// Change it~
		return;
	}
	switch_control();
}

// Test: Battle通常由GameScene借由msp或计算得出并调用。其开始和结束都必须保证通知到GameScene。
void MapLayer::switch_to_battle( string s )
{
	m_iMLState = 3;
	if(wm){
		wm->setVisible(false);
		wm->f_state_circle(false);
		wm->cancontrol = false;
		show_hud();

		f_pauseall();
		f_stopcontrol();
	}
	openBattleMap(s.c_str(),0);
	
	bm->cancontrol = true;
	init_Stat();

	//bm->b_battle = 3;
}

void MapLayer::switch_to_walk()
{
	if(wm){
		EventCenter::sharedEventCenter()->setBmCake(NULL);

		//m_lpJudgement->removeFromParent();
		//m_etClock->removeFromParent();
		bm->removeFromParent();
		CC_SAFE_RELEASE_NULL(bm);

		//CC_SAFE_RELEASE_NULL(m_lpJudgement);
		//CC_SAFE_RELEASE_NULL(m_etClock);

		tm = wm;
		f_resumeall();
		f_resumecon();

		EventCenter::sharedEventCenter()->setController(wm);

		wm->setVisible(true);
		wm->f_state_circle(true);
		wm->cancontrol = true;
		m_iMLState = 1;
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

void MapLayer::RePack()
{
	CCLog(">[BF_ML]Trying to act the third attack...");
	map<string,string> t_ssm = (map<string,string>) vdata.at(0);
	BFsp->re_init();
	BFsp->parse_string(t_ssm.at("action_sp"));

	bm->HandleScriptor(BFsp);
}

//////////////////////////////////////////////////////////////////////////
// <角色状态显示
void MapLayer::init_Stat()
{
	if(!m_lsb){
		m_lsb = new LStatBar();
		m_lsb->autorelease();

		m_lsb->init();
		m_lsb->setPosition(ccp(0,399));

		addChild(m_lsb,98);
	}
	m_lsb->setVisible(false);

	if(!m_rsb){
		m_rsb = new RStatBar();
		m_rsb->autorelease();

		m_rsb->init();
		m_rsb->setPosition(ccp(475,399));

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
