#include "BattleMap.h"

#include "SingleTon/BattleFiled.h"
#include "GameManager.h"

#include "hud/TFollow.h"
#include "ccMacros.h"
#include "SingleTon/SkillJudge.h"
#include "SingleTon/EGroup.h"

#include "json/json.h"

#define ASTAR_DEPTH 2500
#define  MAP_SCALE	1.40625

BattleMap::BattleMap()
{
	miMapType = 2;
	mfMapScale = MAP_SCALE;
}

BattleMap::~BattleMap(){
	CC_SAFE_RELEASE_NULL(m_touch);
	CC_SAFE_RELEASE_NULL(m_caTarget);
	CC_SAFE_RELEASE_NULL(m_caTarCharas);
	CC_SAFE_DELETE(m_bSearch);
}

bool BattleMap::init()
{
	CCLog(">[BattleMap] init() | begins.");
	miMapType = 2;

	mbLockCamera = false;
	m_bSearch = new BSearch();
	//////////////////////////////
	// 1. super init first
	if ( !CCLayer::init() )
	{
		return false;
	}

	m_notuseditems = new CCArray();

	m_touch = NULL; 
	m_caTarget = NULL;
	m_caTarCharas = NULL;

	cancontrol = false;
	mfVx= 0;mfVy = 0;
	m_touch = NULL;
	set_bbattle(-1);
	m_mi=-1;m_mj=-1;
	cs_y.clear();cs_r.clear();cs_b.clear();
	cs_cy.clear();
	//2. TileMap初始化
	

	CCSpriteFrameCache *cache = CCSpriteFrameCache::sharedSpriteFrameCache();
	sheet = CCSpriteBatchNode::create("Images/test.png");
	cache->addSpriteFramesWithFile("Images/test.plist");

	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

	m_tilemap = CCTMXTiledMap::create(CCString::createWithFormat("map/%s.tmx",map_path.c_str())->getCString());
	addChild(m_tilemap, MapdDepth, kTagMap);
	CCSize s = m_tilemap->getContentSize();
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	m_tilemap->setAnchorPoint(ccp(0, 0));
	m_tilemap->setScale( mfMapScale);

	CCSize ms = m_tilemap->getMapSize() /**  mfMapScale*/;
	CCSize ts = m_tilemap->getTileSize() *  mfMapScale;

	//float r = (ms.width + ms.height) / 2;
	rw = ms.width * ts.width;
	rh = ms.height * ts.height;

	max_x = m_tilemap->getMapSize().width;
	max_y = m_tilemap->getMapSize().height;
	int mapWidth = m_tilemap->getMapSize().width * m_tilemap->getTileSize().width *  mfMapScale;
	int mapHeight = m_tilemap->getMapSize().height * m_tilemap->getTileSize().height *  mfMapScale;
	x0 = (ms.height - 1) * ts.width / 2;
	y0 = rh - ts.height;
	dtx = ts.width / 2;
	dty = ts.height / 2;

	dx = rw-mapWidth;
	dy = rh-mapHeight;

	m_tilemap->setPosition(ccp(0,0));
	x0 -= dx;
	y0 -= dy;

	//3. b2world初始化
	//////////////////////////////////////////////////////////////////////////

	b2Vec2 gravity = b2Vec2(0.0f, 0.0f);  

	_world = new b2World(gravity); 

	colse = new MyContactListener();
	_world->SetContactListener(colse);

	b2BodyDef groundBodyDef;  
	groundBodyDef.position.Set(0.0f, 0.0f);							// 设置位置   
	b2Body *groundBody = _world->CreateBody(&groundBodyDef);  

	b2ChainShape chain;												//使用chain将地图包围起来 

	b2FixtureDef sd;  
	sd.shape = &chain;  
	sd.density = 0.0f;  
	sd.restitution = 0.0f;  // Edge 的弹性如何~ 


	b2Vec2 vs[4];
	vs[0].Set((ms.height * ts.width/2 -dx)/ PTM_RATIO, (rh - dy) / PTM_RATIO);
	vs[1].Set((rw - dx) / PTM_RATIO, (ms.height * ts.height/2 - dy) / PTM_RATIO);
	vs[2].Set((ms.width * ts.width/2 - dx) / PTM_RATIO, -dy/PTM_RATIO);
	vs[3].Set(-dx/PTM_RATIO, (ms.width * ts.height/2 - dy )/ PTM_RATIO);


	chain.CreateLoop(vs, 4);

	Entiles* kted = new Entiles();
	kted->name = "edge";
	groundBody->SetUserData(kted);
	groundBody->CreateFixture(&sd);
	m_notuseditems->addObject(kted);
	//////////////////////////////////////////////////////////////////////////
#ifdef DEBUGDRAW

	m_debugDraw =  new MyGLESDebugDraw( PTM_RATIO );
	_world->SetDebugDraw(m_debugDraw);

	unsigned int flags = 0;
	flags += b2Draw::e_shapeBit;
	flags += b2Draw::e_jointBit;
	flags += b2Draw::e_aabbBit;
	flags += b2Draw::e_pairBit;
	flags += b2Draw::e_centerOfMassBit;
	m_debugDraw->SetFlags(flags);		

#endif	


	//////////////////////////////////////////////////////////////////////////
	if(!f_load_entile()) return false;

	if(m_itemlist->count() < 1) CC_SAFE_RELEASE_NULL(m_itemlist);
	m_controller = NULL;

	
	//////////////////////////////////////////////////////////////////////////

	f_setcontroller(m_getEntile("chara_1"));	//ITEMMANAGER设置控制器和镜头
	f_setcamara(m_getEntile("chara_1"));

	//////////////////////////////////////////////////////////////////////////

	//setTouchEnabled(true);
	this->scheduleUpdate();

	// <BattleMap的必要性初始化
	CCTMXTiledMap* m_tilemap = (CCTMXTiledMap*) getChildByTag(kTagMap);
	CCTMXLayer* layer = m_tilemap->layerNamed("Battle");
	CCDictionary* colordic;
	colordic = layer->getProperties();
	c_r = 0;c_b = 0; c_y =0;
	c_r = colordic->valueForKey("red")->intValue();
	c_b = colordic->valueForKey("blue")->intValue();
	c_y = colordic->valueForKey("yellow")->intValue();
	set_bbattle(1);

	// <Block，地形设定
	layer = m_tilemap->layerNamed("Block");
	for(int i = 0;i<ms.width;i++)
		for(int j = 0;j<ms.height;j++)
		{
			unsigned int GID = layer->tileGIDAt(ccp(i,j));		
			CCDictionary* dic;
			dic = m_tilemap->propertiesForGID(GID);
			if(dic){
				int ti = dic->valueForKey("block")->intValue();
				mBlockSet.insert(make_pair(i,j));
				mBlocks.insert(make_pair(make_pair(i,j), ti));
			}
		}

	//////////////////////////////////////////////////////////////////////////
	mFolCamara = CCNode::create();
	mFolCamara->setAnchorPoint(CCPointZero);
	addChild(mFolCamara);
	//////////////////////////////////////////////////////////////////////////
	//mv_iiGroup.clear();
	CCLog(">[BattleMap] init() | ends.");
	return true;
}


int stx = -10;
int sty = -10;
void BattleMap::f_decide(int i, int j){		// <通过新的选中tile对map进行重构。 use the set to decide.
	if(i == stx && j == sty) return;
	CCLog(">[BattleMap] f_decide(%d, %d) ||", i, j);
	clear_mcaTarget();

	stx = i;
	sty = j;
	//CC_SAFE_DELETE(m_bSearch);
	m_bSearch->clear();
	//clear_mcaTarget();
	//clear_Arrange();

	CCTMXTiledMap* map = (CCTMXTiledMap*) getChildByTag(kTagMap);

	if(i<0) return;
	if(j<0) return;
	if(i>=map->getMapSize().width) return;
	if(j>=map->getMapSize().height) return;

	CCLog(">[BattleMap] f_decide() | Pass the Filter.");
	imply_set(ts_last,0);
	center_i = i;
	center_j = j;
	ts_last.clear();

	imply_set(cs_hit_block,c_b);

	
	CCTMXLayer* layer = map->layerNamed("Battle");

	unsigned int gid = layer->tileGIDAt(ccp(i,j));

	draw_mouse_range(ccp(i,j));

	m_mou_cur = ccp(i,j);
	
	CalBlockCS();
	imply_set(ts_last,c_r);
	imply_set(cs_dis,c_b);			//blue is higher than red.
	
}

void BattleMap::update(float dt)
{
	//CCLog(">[BattleMap]update() || Update states:%d", b_battle);
	switch(b_battle){
	case(-1):		//Wait For UnLock & Switch Control
		{
			CC_BREAK_IF(!cancontrol);
			m_bAnimateOver = true;
			clean_cs();
			BattleField::sharedBattleField()->AMOver();
			
			break;
		}
	case(1):						// state == 1 : <点击选择单位.
		{
			checkpoint(m_touch);
			BattleField::sharedBattleField()->ShowChess(m_eCurMouse);
			CC_BREAK_IF(!m_touch);

			break;
		}
	case(8):
		{
			checkpoint(m_touch);
			BattleField::sharedBattleField()->ShowChess(m_eCurMouse);
			CC_BREAK_IF(!m_touch);
			break;
		}
	case(3):						// state == 3 : <移动鼠标并选择目标.
	case(7):
		{		
			CC_BREAK_IF(!m_touch);

			m_touchpoint = this->convertTouchToNodeSpace(m_touch);

			m_touchpoint = ccpAdd(m_touchpoint,ccp(-dtx,-dty));
			CCPoint ti = m_getTilec(m_touchpoint);

			int i = round(ti.x);
			if(i >= max_x) i = max_x -1;
			int j = round(ti.y);		
			if(j >= max_y) j = max_y - 1;
			f_decide(i,j);				

			//if(cs_y.count(make_pair(i,j)) || cs_y.size() == 0){
				mbLockCamera = false;
				fAutoCamara();			
			//}

			//arange_target(miRangeType);

			break;
		}
	case(4):		// state == 4 : Map is bussy moving charaentile.
		{
			CCLog("[BattleMap]update() || ++++++++++++++++++.");
			CC_BREAK_IF(!m_touch);
			CCLog("[BattleMap]update() || Call Moving Acts.");
			((EChessComp*) m_controller->getComponent("controller"))->move_by_path(vc_path);
			m_bAnimateOver = false;
			set_bbattle(-1);
			break;
		}
	case(5):
		{
			set_bbattle(-1);
			m_bAnimateOver = false;
			break;
		}
	}

	//CCLog(">[BattleMap] update from logic.");
	update_collide();
	update_b2world(dt);


}

void BattleMap::thread_update_b2wrold()
{

	TileMap::thread_update_b2wrold();

}

void BattleMap::ccTouchMoved(CCTouch *touch, CCEvent * pEvent){
	TileMap::ccTouchMoved(touch,pEvent);
	if(!cancontrol) {
		CC_SAFE_RELEASE_NULL(m_touch);
		return;
	}
	if(!m_touch) m_touch = new CCTouch();
	m_touch->setTouchInfo(touch->getID(),touch->getLocationInView().x,touch->getLocationInView().y);
}

/* <生成敌人单位 [EChess-Chara] | 战场导入信息的核心接口[140724] */
void BattleMap::f_generateEnemy( int i )
{
	Scriptor* tsp = new Scriptor();

	CC_SAFE_RELEASE_NULL(m_itemlist);
	m_itemlist = new CCDictionary();
	vector<map<string,string>> vdata;
	DBUtil::initDB("save.db");
	string m_sSql = CCString::createWithFormat("select * from enemy_group where id = %d",i)->getCString();
	vdata = DBUtil::getDataInfo(m_sSql,NULL);

	map<string,string> t_ssm = (map<string,string>) vdata.at(0);
	string t_mask	= t_ssm.at("mask");
	string t_is		= t_ssm.at("member");
	string t_scps	= t_ssm.at("script");
	string t_json	= t_ssm.at("additinfo");
	vdata.clear();
	m_sSql = CCString::createWithFormat("select * from enemy_list where id IN (%s)", t_mask.c_str())->getCString();
	vdata = DBUtil::getDataInfo(m_sSql,NULL);
	DBUtil::closeDB();

	//////////////////////////////////////////////////////////////////////////
	
	Json::Value tjv;
	Json::Reader tjr;
	if(tjr.parse(t_json, tjv)){
		GameManager::sharedLogicCenter()->ml->m_iMaxGroup = tjv["groupnums"].asInt();
		miPlayerMask = tjv["playermask"].asInt();
	}else{
		GameManager::sharedLogicCenter()->ml->m_iMaxGroup = 2;
		miPlayerMask = 0x02;
	}


	//////////////////////////////////////////////////////////////////////////
	BattleField::sharedBattleField()->InitBfSp(t_scps.c_str());

	map<int,int> t_miEnemis;
	t_miEnemis.clear();
	// <生成可以使用的敌方单位属性索引表
	for(int i = 0;i<vdata.size();++i){
		map<string,string> t_ssm = (map<string,string>) vdata.at(i);
		int t_fi_id	=	stoi(t_ssm.at("id"));
		t_miEnemis[t_fi_id] = i;
	}

	// <生成敌方单位
	Scriptor* t_scp = new Scriptor();
	stringstream teststream;
	teststream<<t_is;
	int t_id,t_x,t_y,t_gid,t_gmsk;
	string t_name;
	do{
		t_id = 0;
		teststream>>t_id;		// <类型ID
		if(t_id < 1) break;

		CCLog(">[BattleMap] f_generateEnemy() | %d - Begin", t_id);

		teststream>>t_x;		// <横坐标
		teststream>>t_y;		// <纵坐标
		teststream>>t_name;		// <敌人的唯一标示
		teststream>>t_gid;		// <敌人的标识ID
		teststream>>t_gmsk;		// <敌人的标识掩码
		/* [id x y s_name] */

		map<string,string> t_ssm = (map<string,string>) vdata.at(t_miEnemis[t_id]);

		t_scp->re_init();
		t_scp->parse_string(t_ssm.at("attr"));		// <读取属性

		//////////////////////////////////////////////////////////////////////////
		EChesses* t_fij_ecd = new EChesses();

		t_fij_ecd->load_chara_dbsp((Script*) t_scp->m_caScript->objectAtIndex(0));


		t_fij_ecd->psz	=	t_ssm.at("spx");			//Test Only. SPX 入口
		t_fij_ecd->pos	=	ccp(t_x,t_y);				

		// <敌我识别用ID和Mask在此设置
		t_fij_ecd->group_id = t_gid;
		t_fij_ecd->group_mask = t_gmsk;

		t_fij_ecd->name	=	t_name;
		m_itemlist->setObject(t_fij_ecd,t_fij_ecd->name);
		//t_fij_ecd->autorelease();
		t_fij_ecd->m_pChara->initWithSsm(t_ssm);

		//////////////////////////////////////////////////////////////////////////
		tsp->re_init();
		tsp->parse_string(t_ssm.at("sound"));
		t_fij_ecd->m_pChara->m_ssLib		 =	 tsp->mapscps;
		tsp->mapscps->retain();

		CCLog(">[BattleMap] f_generateEnemy() | %d - Over", t_id);

		//BattleField::sharedBattleField()->SetChess(t_fij_ecd,t_fij_ecd->pos.x,t_fij_ecd->pos.y);

	} while(1);
	CCLOG(">Enemy Generationg Over.");

	CC_SAFE_DELETE(t_scp);
	CC_SAFE_DELETE(tsp);

}

/* <载入角色 [Chara-EChess] */
void BattleMap::f_load_chara()
{
	CCArray* tca = BattleField::sharedBattleField()->mSpLinker->initcs;
	Script* stmp = (Script*) tca->objectAtIndex(0);				
	CCArray* mca = stmp->scriptnodes;

	for(int i = 0;i<mca->count();i++){
		Script* tmp = (Script*) mca->objectAtIndex(i);					//script pack

		Chara* t_cca = CharaS::sharedCharaS()->getIDChara(tmp->getint("name"));					
		if(!t_cca) return;

		EChesses* t_ec = new EChesses();
		t_cca->retain();

		t_ec->pos = ccp(tmp->getfloat("x"),tmp->getfloat("y"));

		// <角色的敌我识别ID和Mask在此设置|注意载入次序
		t_ec->group_id = 0x01;
		t_ec->group_mask = miPlayerMask;
		t_ec->m_pChara = t_cca;
		t_ec->name = CCString::createWithFormat("chara_%d",tmp->getint("name"))->getCString();
		t_ec->psz  = t_cca->m_sSpx;//tmp->getstring("file");//t_cca->m_sPsz;			//Spx									//Whether use the same psz is due to further design.

		m_itemlist->setObject(t_ec,t_ec->name);									//Test: get one and only one.

		//BattleField::sharedBattleField()->SetChess(t_ec,t_ec->pos.x,t_ec->pos.y);

	}

}

bool BattleMap::f_load_entile()
{

do 
{
	b2CircleShape circle; 

	b2BodyDef ballBodyDef;					
	ballBodyDef.type = b2_dynamicBody;  	 
	circle.m_radius = 16.0/PTM_RATIO;  

	b2FixtureDef ballShapeDef;				
	ballShapeDef.shape = &circle;  
	ballShapeDef.density = 1.0f;  
	ballShapeDef.friction = 1.0f;  
	ballShapeDef.restitution = 0.0f;  
	ballShapeDef.isSensor = true;				// <将碰撞形状改为sensor不会提高多少效率却会破坏map原本事件机制。
	
	//// b2body准备工作
	//////////////////////////////////////////////////////////////////////////
	//CCSize ts = m_tilemap->getTileSize();
	//b2PolygonShape boxShape;
	//float hbw = (ts.width-1)/( PTM_RATIO * 4);
	//float hbh = (ts.height-1)/( PTM_RATIO);
	//hbh *= 1.2;
	//boxShape.SetAsBox(hbw,hbh,b2Vec2(0,hbh),0);


	CCDictElement* cde = NULL;
	CCDICT_FOREACH(m_itemlist,cde){
		EChesses* t_ec = (EChesses*) cde->getObject();
		CCLog(">[BattleMap] f_load_entile() | Name:%s - Head:%s", t_ec->m_pChara->m_sName.c_str(), t_ec->m_pChara->msBHead.c_str());

		t_ec->initFiles(t_ec->psz.c_str());
		CCPoint d = m_getViewc(t_ec->pos);

		t_ec->setAnchorPoint(ccp(0,0));
		t_ec->b_Re = true;
		addChild(t_ec);

		ballBodyDef.position.Set((dtx+d.x)/PTM_RATIO, (d.y+dty)/PTM_RATIO);  
		_body = _world->CreateBody(&ballBodyDef);  
		_body->SetFixedRotation(true);
		_body->CreateFixture(&ballShapeDef); 

		t_ec->m_body = _body;
		_body->SetUserData(t_ec);			

		//t_ec->FixPosition();

		//b2FixtureDef boxFixtureDef;				
		//boxFixtureDef.shape = &boxShape;
		//boxFixtureDef.density = 1;
		//boxFixtureDef.filter.categoryBits = 0x0002;
		//boxFixtureDef.filter.maskBits = 0x0001;
		//boxFixtureDef.isSensor = true;

		//_body->CreateFixture(&boxFixtureDef);
	}
	// EChesses in BattleMap has its own logic, so a logic based on sensor is no longer needed.

	return true;
} while (0);


return false;
}

Script* BattleMap::f_scrfind( string gn, int t )
{
	return NULL;
}

void BattleMap::checkpoint(CCTouch* a_touch)
{
	if(!a_touch){
		m_eCurMouse = NULL;
		return;
	}
	fAutoCamara();
	CCPoint tm_touchpoint = this->convertTouchToNodeSpace(a_touch);

	tm_touchpoint = ccp(tm_touchpoint.x/PTM_RATIO,tm_touchpoint.y/PTM_RATIO);
	
	b2AABB* aabb = new b2AABB();

	aabb->lowerBound = b2Vec2(tm_touchpoint.x,tm_touchpoint.y); 
	aabb->upperBound = b2Vec2(tm_touchpoint.x,tm_touchpoint.y); 

	//CCLOG(">Ready For checkpoint:%f,%f",tm_touchpoint.x,tm_touchpoint.y);
	MyQueryCallback queryCallback;
	_world->QueryAABB( &queryCallback, *aabb );
	vector<b2Fixture*>::iterator iter;  
	delete aabb;

	m_eCurMouse = NULL;
	float t_miny = rh;

	for (iter= queryCallback.foundBodies.begin();iter!= queryCallback.foundBodies.end();iter++)  
	{  
		b2Fixture* f = (*iter);
		if(f->GetBody()->GetType() == b2_dynamicBody && f->IsSensor())		
		{	
			if(f->GetBody()->GetPosition().y < t_miny){
				//CCLOG(">mouse contact:%s",getfname(f).c_str());	
				m_eCurMouse = (EChesses*) f->GetBody()->GetUserData();				
				t_miny = f->GetBody()->GetPosition().y;
			}


		} //end of if
		
	} //end of for_iter
	
}

void BattleMap::draw_skill_range(int a_type, vector<int> a_ran)
{
	cs_y.clear();
	cs_cy.clear();
	m_con_cur = ((EChesses*) m_controller)->pos;

	EChesses* t_ce = (EChesses*) m_controller;
	switch(a_type)
	{
	case(-2):	// <所有同组单位
		{
			EChesses* tcce;
			CCDictElement* tcde;
			CCDICT_FOREACH(m_itemlist,tcde){
				tcce = (EChesses*) tcde->getObject();
				if(EGroup::sharedEGroup()->IsEnemy(t_ce,tcce)) continue;	// <非敌即友
				cs_y.insert(make_pair(tcce->pos.x, tcce->pos.y));
			}
			break;
		}
	case(1):				// 1 is default a circle.
		{
			int radiu = a_ran[0];
			dps_rect(t_ce->pos, cs_y, radiu);
			break;
		}
	case(2):
		{
			dps_szd(t_ce->pos, cs_y,a_ran[0],a_ran[1]);
			break;
		}
	case(3):
		{
			dps_ring(t_ce->pos, cs_y,a_ran[0],a_ran[1]);
			break;
		}
	}

	imply_set(cs_y,c_y);			//Imply.
	imply_set(cs_cy,c_y);
}

void BattleMap::draw_moving_tile()
{
	EChesses* t_control = (EChesses*) m_controller;

	m_con_cur = t_control->pos;

	Chara* t_chara = t_control->m_pChara;

	int t_iMove = t_chara->getMoveDis();			

	int i = m_con_cur.x;
	int j = m_con_cur.y;

	CCLog(">[BattleMap] Prepare Drawing Layer 1:%d,%d.",i,j);

	cs_hit_block.clear();
	cs_y.clear();				//cs_y存储第一层
	cs_cy.clear();
	draw_moving_block();
	dps_range(m_con_cur, cs_y, t_iMove);

	CCLog(">[BattleMap] draw_moving_tile() | dps_range() overed.");
	cs_y.erase(make_pair(m_con_cur.x,m_con_cur.y));
	imply_set(cs_y,c_y);			//Imply.
	
	for(set<pair<int,int>>::iterator it = cs_y.begin(); it != cs_y.end(); ++it){
		CCLog(">[BattleMap] draw_moving_tile() | cs_y:%d,%d", it->first, it->second);
	} 

	for(set<pair<int,int>>::iterator it = cs_hit_block.begin(); it != cs_hit_block.end(); ++it){
		CCLog(">[BattleMap] draw_moving_tile() | cs_hit_block:%d,%d", it->first, it->second);
	} 

	CalBlockCS();
	imply_set(cs_dis,c_b);
	//imply_set(cs_block,c_b);
	imply_set(cs_hit_block,c_b);
	//[0803]CCLog("[BM]cs_dis:%d|cs_block:%d",cs_dis.size(),cs_block.size());

}

void BattleMap::CalBlockCS(){
	for(set<pair<int,int>>::iterator it = cs_y.begin(); it != cs_y.end(); ++it){
		if(cs_block.count(make_pair(it->first,it->second)) || mBlockSet.count(make_pair(it->first,it->second))){
			CCLog(">[BattleMap] CalBlockCS() | %d,%d", it->first, it->second);
			cs_dis.insert(make_pair(it->first,it->second));
			CCLog(" +++ [%d,%d] +++", it->first, it->second);
		}
	} 
}

void BattleMap::draw_moving_block()
{
	clean_cs();

	CCLOG(">Drawing block by g&m:%d,%d.",m_controller->group_id,m_controller->group_mask);
	CCDictElement* pce = NULL;
	CCDICT_FOREACH(m_itemlist,pce){
		EChesses* t_ec = (EChesses*) pce->getObject();
		int t_x = t_ec->pos.x;				
		int t_y = t_ec->pos.y;
		
		//cs_dis.insert(make_pair(t_x,t_y));
		

		CCLOG(">m_itemlist:%s",t_ec->name.c_str());

		// <如果目标单位敌视m_controller，则生成ZOC
		if(EGroup::sharedEGroup()->IsEnemy(t_ec, m_controller)){	// For Moving | cs_block is for calculating && cs_dis is for moving deciding while mouse move.
			int t_range = 2;							//TODO: Test. ZOC Range = 1; type = ring
			dps_szd(t_ec->pos, cs_block, 0, t_range);
		}else{
			cs_block.insert(make_pair(t_x,t_y));		// <友军所站的位置不能停泊。
			//cs_dis.insert(make_pair(t_x,t_y));
		}
		
		if(mBlockSet.count(make_pair(t_x, t_y))){
			cs_block.insert(make_pair(t_x, t_y));
		}
	}

	cs_block.erase(make_pair(m_con_cur.x,m_con_cur.y));
}

void BattleMap::dps_rectangle(CCPoint a_cp, set<pair<int,int>> &a_dt, CCPoint a1, CCPoint a2){
	int low_x = min(a1.x,a2.x);
	int low_y = min(a1.y,a2.y);

	int h = abs(a1.y-a2.y);
	int w = abs(a1.x-a2.x);

	h = max(1,h);
	w = max(1,w);

	//low_x;
	//low_y;

	for(int i = 0; i<h;++i){
		for(int j= 0;j<w;++j){
			a_dt.insert(make_pair(low_x+j,low_y+i));
		}
	}
	a_dt.erase(make_pair(a_cp.x,a_cp.y));
	a_dt.erase(make_pair(a1.x,a1.y));
	a_dt.erase(make_pair(a2.x,a2.y));
}

/* <绘制环形 */
void BattleMap::dps_szd(CCPoint a_cp, set<pair<int,int>> &a_dt, int a_min , int a_max){
	int t_x = a_cp.x;
	int t_y = a_cp.y;
	//a_dt.insert(make_pair(t_x,t_y));

	//no one can block ring, we just add them into set.

	for(int i = a_min; i<a_max; ++i){
		a_dt.insert(make_pair(t_x+i,t_y));
		a_dt.insert(make_pair(t_x-i,t_y));
		a_dt.insert(make_pair(t_x,t_y+i));
		a_dt.insert(make_pair(t_x,t_y-i));
	}
	//a_dt.erase(make_pair(a_cp.x,a_cp.y));
}

void BattleMap::dps_ring( CCPoint a_cp , set<pair<int,int>> &a_dt, int a_min, int a_max)
{	
	int t_x = a_cp.x;
	int t_y = a_cp.y;
	a_dt.insert(make_pair(t_x,t_y));

	int ny;
	for(int i = 0; i <= a_max; ++i){
		ny = t_y + i;
		for(int j = t_x - a_max + i; j <= min(t_x, t_x - a_min + i); ++j)
		{
			a_dt.insert(make_pair(j,ny));
		}

		for(int j = max(t_x, t_x + a_min - i); j <= t_x + a_max - i; ++j)
		{
			a_dt.insert(make_pair(j,ny));
		}

	}

	for(int i = 1; i <= a_max; ++i){
		ny = t_y - i;
		for(int j = t_x - a_max + i; j <= min(t_x, t_x - a_min + i); ++j)
		{
			a_dt.insert(make_pair(j,ny));
		}

		for(int j = max(t_x, t_x + a_min - i); j <= t_x + a_max - i; ++j)
		{
			a_dt.insert(make_pair(j,ny));
		}
	}
}

void BattleMap::dps_rect(CCPoint a_cp, set<pair<int,int>> &a_dt, int a_max)
{
	int t_x = a_cp.x;
	int t_y = a_cp.y;
	a_dt.insert(make_pair(t_x,t_y));

	int ny;
	for(int i = 0; i <= a_max; ++i){
		ny = t_y + i;
		for(int j = t_x - a_max + i; j <= t_x + a_max - i; ++j)
		{
			a_dt.insert(make_pair(j,ny));
		}
	}

	for(int i = 1; i <= a_max; ++i){
		ny = t_y - i;
		for(int j = t_x - a_max + i; j <= t_x + a_max - i; ++j)
		{
			a_dt.insert(make_pair(j,ny));
		}
	}
}

void BattleMap::dps_range( CCPoint a_cp , set<pair<int,int>> &a_dt, int a_max )
{
	int t_x = a_cp.x;
	int t_y = a_cp.y;

	CCLog(">Dps_range_begin:%d,%d",t_x,t_y);
	
	if(cs_block.count(make_pair(t_x,t_y))>0){
		cs_hit_block.insert(make_pair(t_x,t_y));
		return;
	}

	if(mBlockSet.count(make_pair(t_x, t_y))){
		//cs_block.insert(make_pair(t_x,t_y));
		return;
	}

	CCLog(">Dps_range_pass:%d,%d",t_x,t_y);

	if(a_dt.count(make_pair(t_x,t_y))>0) return;
	a_dt.insert(make_pair(t_x,t_y));
	if((abs(t_x - m_con_cur.x) + abs(t_y - m_con_cur.y)) == a_max) return;
	dps_range(ccpAdd(a_cp,ccp(0,1)), a_dt, a_max);
	dps_range(ccpAdd(a_cp,ccp(0,-1)), a_dt, a_max);
	dps_range(ccpAdd(a_cp,ccp(1,0)), a_dt, a_max);
	dps_range(ccpAdd(a_cp,ccp(-1,0)), a_dt, a_max);
	CCLog(">Dps_range_back:%d,%d",t_x,t_y);
}

void BattleMap::imply_set( set<pair<int,int>> a_dt, unsigned int d_c, bool ab_clean )
{
	CCTMXTiledMap* map = (CCTMXTiledMap*) getChildByTag(kTagMap);
	CCTMXLayer* layer = map->layerNamed("Battle");
	if(d_c == 0 && !ab_clean){
		for(set<pair<int,int>>::iterator it = a_dt.begin(); it != a_dt.end(); ++it){
			if(it->first >= 0 && it->second >= 0 && it->first < max_x && it->second < max_y)
			{
				unsigned int gid = layer->tileGIDAt(ccp(it->first,it->second));
				if(cs_y.count(*it) > 0)
					layer->setTileGID(c_y,ccp(it->first,it->second));
				else
					layer->setTileGID(0,ccp(it->first,it->second));
			}
		}
	}else{
		for(set<pair<int,int>>::iterator it = a_dt.begin(); it != a_dt.end(); ++it){
			if(it->first >= 0 && it->second >= 0 && it->first < max_x && it->second < max_y)
			{
				unsigned int gid = layer->tileGIDAt(ccp(it->first,it->second));
				//CCLOG("gid:%d.",gid);

				layer->setTileGID(d_c,ccp(it->first,it->second));
			}
		}
	}

}

bool BattleMap::move_control()
{
	bool ret = false;
	do 
	{
		int tx = m_mou_cur.x;
		int ty = m_mou_cur.y;
		pair<int,int> t_pii = make_pair(tx,ty);

		CC_BREAK_IF(cs_y.count(t_pii) == 0 && cs_hit_block.count(t_pii) == 0);
		CC_BREAK_IF(cs_dis.count(t_pii) > 0);
		CC_BREAK_IF(BattleField::sharedBattleField()->HasChess(tx, ty));

		ret = move_control(m_mou_cur.x,m_mou_cur.y);
		m_mou_cur = ccp(-1,-1);
	} while (0);
	return ret;
}


bool BattleMap::move_control(int ax, int ay)
{
	do 
	{
		//[0803]CCLog("Try to move all the element. S&C");
		m_con_cur = ((EChesses*) m_controller)->pos;
		micECx = m_con_cur.x;
		micECy = m_con_cur.y;

		m_mov_tar = ccp(ax,ay);
		a_star();
		set_bbattle(4);
		vc_path.pop_back();		// <最后一个点是起点，所以将其弹出。

		((EChesses*) m_controller)->pos = CCPointMake(ax,ay);
		CCLog(">[BattleMap]move_control() || Move is all ready");
		if(!m_touch) m_touch = new CCTouch();		// <防止AI操作时没有Touch导致Update不执行
		//clean_cs();
		//m_mou_cur = ccp(-1,-1);
		return true;
	} while (0);
	return false;
}

void BattleMap::a_star()		// <结果被存储在vc_path中。
{
	CCLog(">[BattleMap] a_star() | Begin");
	vc_path.clear();
	list<StepNode> lsn = getSearchPath(m_con_cur,m_mov_tar);
	BattleField::sharedBattleField()->ChessMoved((EChesses*)m_controller, m_con_cur, m_mov_tar);
	vc_path.push_back(m_mov_tar);
	lsn.pop_back();

	CCLog(">[BattleMap] a_star() | Mid");
	int t_idirect = 0;
	StepNode t_sn = lsn.back();
	int dx = m_mov_tar.x - t_sn.x;
	int dy = m_mov_tar.y - t_sn.y;
	//[0803]CCLog("BEGIN POINT:%f,%f.",m_mov_tar.x,m_mov_tar.y);
	//[0803]CCLog("End POINT:%f,%f.",t_sn.x,t_sn.y);

	CCLog(">[BattleMap]a_star() || Ready for finding path.");

	if(dx > 0) t_idirect = 3;
	else if(dx < 0) t_idirect = 2;
	else if(dy > 0) t_idirect = 4;
	else t_idirect = 1;

	//t_idirect = t_sn.status;

	dx = t_sn.x;
	dy = t_sn.y;
	bool turn_lock = true;		//The flag identity that last move is a turn.

	set<pair<int,int>> t_ps;
	t_ps.insert(make_pair(m_mov_tar.x,m_mov_tar.y));
	for(list<StepNode>::reverse_iterator it = lsn.rbegin(); it != lsn.rend(); ++it){
		if(it->x != dx || it->y != dy) continue;
		if(t_ps.count(make_pair(dx,dy)) > 0) continue;
		t_ps.insert(make_pair(dx,dy));
		t_sn = *it;
		//[0803]CCLog("NextParentGot:%d,%d G:%d H:%d F:%d",dx,dy,t_sn.G,t_sn.H,t_sn.getF());


		if(t_sn.status != t_idirect){
			vc_path.push_back(ccp(t_sn.x,t_sn.y));
			
			t_idirect = t_sn.status;
			turn_lock = true;
			//[0803]CCLog("Turned:%d,%d.",dx,dy);
		}else{
			//[0803]CCLog("UnTurned:%d,%d.",dx,dy);
			turn_lock = false;
		}	
		


		switch(t_idirect){
		case 1:
			dy += 1;
			break;
		case 2:
			dx += 1;
			break;
		case 3:
			dx -= 1;
			break;
		case 4:
			dy -= 1;
			break;
		}

	};

	if(!turn_lock){
		vc_path.push_back(m_con_cur);
	}

	CCLog(">[BattleMap]a_star() || Find the path over.");
	//[0803]CCLog("A_STAR is over. Try to check the path.");
	//for(vector<CCPoint>::iterator it = vc_path.begin(); it != vc_path.end(); ++it){
		//[0803]CCLog("Point:%f,%f.",it->x,it->y);
	//}
}

void BattleMap::clean_cs()
{
	imply_set(cs_y,0,true);
	imply_set(cs_cy,0,true);
	imply_set(cs_dis,0,true);
	imply_set(ts_last,0,true);
	imply_set(cs_block,0,true);

	cs_b.clear();
	cs_dis.clear();
	cs_block.clear();
	cs_hit_block.clear();
}

void BattleMap::clean_allcs()
{
	clean_cs();
	cs_y.clear();
	cs_cy.clear();
	ts_last.clear();
}

///A*寻路算法
float BattleMap::getNodeH(CCPoint to, StepNode& node)
{
	return std::sqrt( std::pow((to.x - node.x),2) + std::pow(( to.y - node.y),2) );
}

StepNode BattleMap::getNodeChild(StepNode sn, int i)			
{
	StepNode point;

	int a = 0;
	int b = 0;
	switch (i)
	{
	case 0:
		a = 0;
		b = -1;
		break;
	case 1:
		a = 0;
		b = -1;
		break;
	case 2:
		a = -1;
		b = 0;
		break;
	case 3:
		a = 1;
		b = 0;
		break;
	case 4:
		a = 0;
		b = 1;
		break;
	case 5:
		a = 1;
		b = 1;
		break;
	case 6:
		a = 1;
		b = 0;
		break;
	case 7:
		a = 1;
		b = -1;
		break;
	default:
		break;
	}

	sn.x += a;
	sn.y += b;

	point.x = sn.x;
	point.y = sn.y;
	point.G = sn.G;
	point.H = sn.H;
	point.status = 0;

	return point;
}

bool compare_index(const StepNode t1,const StepNode t2){  
	//printf("CompareIndex:%d,%d/n",t1->index,t2->index); 
	int i_t = (t1.G + t1.H) - (t2.G + t2.H);

	if( i_t < 0 ) return false;
	else if(i_t == 0){
/*		CCLOG(">t_xy:%d,%d",t_ix,t_iy);
		if((t1.x == t_ix) && (t1.y == t_iy)) return true;
		else if((t2.x == t_ix) && (t2.y == t_iy)) return false;
		else*/ 
			return (t1.status > t2.status);
	}else return true;

}  

list<StepNode> BattleMap::getSearchPath(CCPoint startPos, CCPoint to)
{
	vector<StepNode> openNodeVec;				//space to time; it works.
	map<int,map<int,int>> mStatuDmaps;

	list<StepNode> pathList;
	// <查找steps格范围内的目标
	int steps = ASTAR_DEPTH;			// <20x20 = 400; IN:小心
	int last_f = 500;
	// <初始点
	StepNode startNode;
	startNode.x = startPos.x;
	startNode.y = startPos.y;
	startNode.G = 0;
	startNode.H = getNodeH(to, startNode);;
	startNode.status = 1;
	//[0803]CCLog("[BM]StartNode Began>Start End---[%d,%d]||>>>>||[%f,%f]",startNode.x,startNode.y,to.x,to.y);

	// <记录点
	openNodeVec.push_back(startNode);

	bool isFind = false;
	
	StepNode nNod;
	nNod = startNode;

	while (!isFind && steps > 0){
		// <以F值从大到小 降序排列
		sort(openNodeVec.begin(), openNodeVec.end(), compare_index);
		
		if (openNodeVec.size() > 0){					//TODO: Is this check necessary?
			StepNode curNode = openNodeVec[openNodeVec.size()-1];
			
			if(last_f <= curNode.getF() && as_checkpoint(nNod.x, nNod.y)){
				curNode = nNod;
				
			}else{
				openNodeVec.pop_back();
			}	
		
			mStatuDmaps[curNode.x][curNode.y] = 1;
			pathList.push_back(curNode);
			//[0803]CCLog("[BM]Layer count:%d||The Next Node is Added:%d,%d G:%d H:%f F:%f",ASTAR_DEPTH-steps,curNode.x,curNode.y,curNode.G,curNode.H,curNode.getF());
			//////////////////////////////////////////////////////////////////////////
			nNod = getNodeChild(curNode,curNode.status);
			nNod.status = curNode.status;
			nNod.H = getNodeH(to, nNod);
			nNod.G = curNode.G + 1;
			last_f = nNod.getF();


			for (int i = 1; i < 5 ; i++){
				StepNode nextNode = getNodeChild(curNode, i);
				if (nextNode.x == to.x && nextNode.y == to.y){
					pathList.push_back(nextNode);
					nextNode.status = i;
					isFind = true;
					if(isFind) CCLog(">[BM] We Find the point here.");
					break;
				}
				/* <CHECK POINT 非阻挡点 是连续移动点*/
				else if ( as_checkpoint(nextNode.x, nextNode.y) //&& as_checkNpcMove(curNode.x, curNode.y , nextNode.x , nextNode.y) 
					){
					int g = //((nextNode.x == curNode.x || nextNode.y == curNode.y) ? 1 : 2) 
						1 + curNode.G;
					int h = getNodeH(to, nextNode);
					if (mStatuDmaps[nextNode.x][nextNode.y]== 0){
						nextNode.G = g;
						nextNode.H = h;
						nextNode.status = i;
						openNodeVec.push_back(nextNode);
					}

				}
			}
		}
		steps--;
		//[0803]CCLog("[BM] Step is down:%d",steps);
	}
	//[0803]CCLog("[BM]A star is over, depth:%d",(ASTAR_DEPTH-steps));
	return pathList;
}

bool BattleMap::as_checkpoint( int x, int y )
{
	bool bRet;
	bRet = cs_y.count(make_pair(x,y)) > 0;
	if(!bRet) CCLog(">Get:%d,%d",x,y);
	return bRet;			//the only blue-yellow point is blocked as closed list at start.
}

bool BattleMap::as_checkNpcMove( int x, int y, int m, int n )
{
	return (cs_y.count(make_pair(x,n))+cs_y.count(make_pair(m,y))) > 0;
}

void BattleMap::set_mouse_range( int a_type, vector<int> a_ran )
{
	m_mouse_type	=	a_type;
	m_mouse_arrs.clear();
	m_mouse_arrs	=	a_ran;
}

bool BattleMap::InMap(int ax, int ay)
{
	do 
	{
		if(ax <0) break;
		if(ay < 0) break;
		if(ax >= max_x) break;
		if(ay >= max_y) break;
		if(mBlockSet.count(make_pair(ax,ay)) > 0) break;
		CCLog(">[BattleMap] InMap(%d, %d) success", ax, ay);
		return true;
	} while (false);
	return false;
}

/*
	<a_cp -- 当前鼠标所在的点 || [技能圈定]
	< 本函数通过传入的鼠标位置和预先设置的类型进行判定
	< 将绘制目标传入ts_last和cs_cy
*/
void BattleMap::draw_mouse_range(CCPoint a_cp/* <当前鼠标所在的点*/)
{
	set<pair<int,int>> ts_temp;		// <临时容器
	CCObject* toe;

	m_mou_cur = a_cp;

	switch(m_mouse_type)
	{
	case -1:
		{

			cp_last = a_cp;
			int radiu = m_mouse_arrs[0];
			dps_rect(a_cp, ts_last, radiu);
			dps_rect(a_cp, cs_y, radiu);
			break;
		}
	case(0):
		{
			ts_last.insert(make_pair(a_cp.x,a_cp.y));
			cp_last = a_cp;
			break;
		}
	case(1):				// 1 is default a circle.
		{
			if(cs_y.count(make_pair(a_cp.x,a_cp.y)) > 0)
			{
				cp_last = a_cp;
				int radiu = m_mouse_arrs[0];
				dps_rect(a_cp, ts_last, radiu);
			}
			break;
		}
	case(2):		// <2 | 弹跳传递 - 螺旋枪杀
		{
			if(cs_y.count(make_pair(a_cp.x,a_cp.y)) > 0)
			{
				CCLog(">Skill Type 2:Jump..");

				cp_last = a_cp;
				int radiu = m_mouse_arrs[0];
				dps_rect(a_cp, ts_last, radiu);
				find_target_arrage(miRangeType);

				int dpress = m_mouse_arrs[1];


				int x = a_cp.x;
				int y = a_cp.y;

				CC_BREAK_IF(!m_caTarget);

				a_cp = ((EChesses*) m_controller)->pos;

				CCObject* tce = NULL;
				CCARRAY_FOREACH(m_caTarget,tce){
					CCPoint tcp = ((EChesses*) tce)->pos;
					int dx = tcp.x - a_cp.x;
					int dy = tcp.y - a_cp.y;
					
					if(dx != 0) dx = dx/abs(dx);
					if(dy != 0) dy = dy/abs(dy);


					for(int i = 0;i<dpress;++i){
						x += dx;
						y += dy;
						ts_last.insert(make_pair(x,y));
						cs_cy.insert(make_pair(x,y));
					}
				}

				find_target_arrage(miRangeType);
				CCLog(">[BattleMap] case|2 | total:%d", m_caTarget->count());
				//////////////////////////////////////////////////////////////////////////
			}
			break;
		}
	case(3):	// < 3 | 两点之间的矩形区域 + 十字搜索1格 (狂嗜·突袭·追击)
		{
			if(cs_y.count(make_pair(a_cp.x,a_cp.y)) > 0){
				//////////////////////////////////////////////////////////////////////////
				// <十字搜索1格

				ts_temp.clear();
				dps_rect(a_cp, ts_temp, 1);		// <如果有复用的情况就传入动态的参数
				ts_temp.erase(make_pair(a_cp.x,a_cp.y));
				find_target_arrage(0, ts_temp);			// <优先搜索敌方单位
				if(m_caTarCharas->count() == 0){ // <搜索友方单位
					find_target_arrage(1, ts_temp);
				}
				ts_temp.clear();
				if(m_caTarCharas->count()>0){
					EChesses* te = (EChesses*) m_caTarget->lastObject();
					m_caTarget->removeAllObjects();
					m_caTarCharas->removeAllObjects();

					m_caTarCharas->addObject(te->m_pChara);
					m_caTarget->addObject(te);

					ts_temp.insert(make_pair(te->pos.x,te->pos.y));
					cs_cy.insert(make_pair(te->pos.x,te->pos.y));
					te->miHitGroup |= 2;			// <攻击编组，不同的编组所播放的技能特效会不同。
					CCLog(">[BM]Find the third place entile:%s",te->name.c_str());
				}
				CCLog(">[BattleMap] case|3 | total:%d", m_caTarget->count());

				ts_last.insert(ts_temp.begin(),ts_temp.end());
				//////////////////////////////////////////////////////////////////////////
				// <矩形搜索

				ts_temp.clear();
				CCPoint tcp = ((EChesses*) m_controller)->pos;
				dps_rectangle(a_cp,ts_temp,a_cp,tcp);
				dps_rectangle(a_cp,cs_cy,a_cp,tcp);
				ts_last.insert(ts_temp.begin(),ts_temp.end());
				find_target_arrage(-1);

				CCARRAY_FOREACH(m_caTarget,toe){
					if(toe == m_controller) continue;					// <不以自己为对象
					if(ts_temp.count(make_pair(((EChesses*) toe)->pos.x, ((EChesses*) toe)->pos.y)) == 0) continue;
					((EChesses*) toe)->miHitGroup |= 1;
				}

				
				//////////////////////////////////////////////////////////////////////////
				//ts_last.insert(make_pair(a_cp.x,a_cp.y));
				//cs_cy.insert(make_pair(a_cp.x,a_cp.y));
			}
			break;
		}
	case 4:		// <狂嗜•君临
		{
			if(cs_y.count(make_pair(a_cp.x,a_cp.y)) > 0){
				//////////////////////////////////////////////////////////////////////////
				// <矩形搜索
				CCLog(">[BattleMap] ---- SkPointCache ----");
				CCLog(">[BattleMap] Point:%f,%f", a_cp.x, a_cp.y);

				CCLog(">[BattleMap] ca total:%d", m_caTarget->count());
				CCARRAY_FOREACH(m_caTarget,toe){
					CCLog(" HigGroup:%d || Position:%f, %f", ((EChesses*) toe)->miHitGroup, ((EChesses*) toe)->pos.x, ((EChesses*) toe)->pos.y);
				}

				ts_temp.clear();
				CCPoint tcp = ((EChesses*) m_controller)->pos;
				dps_rectangle(a_cp,ts_temp,a_cp,tcp);
				dps_rectangle(a_cp,cs_cy,a_cp,tcp);
				ts_last.insert(ts_temp.begin(),ts_temp.end());

				CCLog(">[BattleMap] Search for first dash.");
				find_target_arrage(-1);

				CCARRAY_FOREACH(m_caTarget,toe){
					if(toe == m_controller) continue;					// <不以自己为对象
					CCLog(">[BattleMap] Set For First Dash:%d", ((EChesses*) toe)->miHitGroup);
					((EChesses*) toe)->miHitGroup |= 1;
				}
				
				m_caTarget->removeObject(m_controller);
				CCLog(">[BattleMap] ca total:%d", m_caTarget->count());
				CCARRAY_FOREACH(m_caTarget,toe){
					CCLog(" HigGroup:%d || Position:%f, %f", ((EChesses*) toe)->miHitGroup, ((EChesses*) toe)->pos.x, ((EChesses*) toe)->pos.y);
				}
				

				int tiNum = m_mouse_arrs[0] + rand() % m_mouse_arrs[1];
				int tiDec = m_mouse_arrs[2];
				--tiNum;
				//////////////////////////////////////////////////////////////////////////
				// <
				CCPoint tbp = a_cp;
				CCPoint tep;

				for(int i = 0; i< tiNum; ++i){
					CCLog(">[BattleMap] Skill Rage.M. | %d/%d", i, tiNum);
					ts_temp.clear();
					
					do 
					{
						float trate = CCRANDOM_0_1();

						if(trate<0.25){
							tep = ccpAdd(tbp,ccp(0,tiDec));
							if(InMap(tep.x, tep.y)) break;
						}else if(trate < 0.5){
							tep = ccpAdd(tbp,ccp(tiDec,0));
							if(InMap(tep.x, tep.y)) break;
						}else if(trate < 0.75){
							tep = ccpAdd(tbp,ccp(0,-tiDec));
							if(InMap(tep.x, tep.y)) break;				
						}else{
							tep = ccpAdd(tbp,ccp(-tiDec,0));
							if(InMap(tep.x, tep.y)) break;
						}
						CCLog(">[BattleMap] Map is smaller than tiDec:%d", tiDec);
					} while (true);

					CCLog(">[BattleMap] Point:%f,%f", tep.x, tep.y);
					m_bSearch->inval_plist(tep.x, tep.y);
					dps_rectangle(tbp,ts_temp,tbp,tep);
					//dps_rectangle(tbp,cs_cy,tbp,tep);

					//ts_last.insert(ts_temp.begin(),ts_temp.end());
					CCLog(">[BattleMap] draw_mouse_range() | Search for tmp: %d", ts_temp.size());

					find_target_arrage(-1, ts_temp);

					CCARRAY_FOREACH(m_caTarget,toe){
						if(toe == m_controller) continue;					// <不以自己为对象
						CCLog(">[BattleMap] search find. bf.");
						if(ts_temp.count(make_pair(((EChesses*) toe)->pos.x, ((EChesses*) toe)->pos.y)) == 0) continue;
						int af = 2 << i;
						CCLog(">[BattleMap] search find. af:%d", af);
						((EChesses*) toe)->miHitGroup |= (2<<i);
					}
					tbp = tep;
					CCLog(">[BattleMap] ca total:%d", m_caTarget->count());
					CCARRAY_FOREACH(m_caTarget,toe){
						CCLog(" HigGroup:%d || Position:%f, %f", ((EChesses*) toe)->miHitGroup, ((EChesses*) toe)->pos.x, ((EChesses*) toe)->pos.y);
					}
				}

				CCLog(">[BattleMap] ---- SkPointCache Over ----");
				CCLog(">[BattleMap] m_caTarget:%d", m_caTarget->count());

				CCLog(">[BattleMap] ca total:%d", m_caTarget->count());
				CCARRAY_FOREACH(m_caTarget,toe){
					CCLog(" HigGroup:%d || Position:%f, %f", ((EChesses*) toe)->miHitGroup, ((EChesses*) toe)->pos.x, ((EChesses*) toe)->pos.y);
				}
			}
			break;
		}
	case 5:	// <晓绝 - 一闪
		{
			if(cs_y.count(make_pair(a_cp.x,a_cp.y)) > 0){
				bool check = false;

				ts_temp.clear();
				CCPoint tcp = ((EChesses*) m_controller)->pos;
				dps_rectangle(a_cp,ts_temp,a_cp,tcp);
				dps_rectangle(a_cp,cs_cy,a_cp,tcp);
				ts_last.insert(ts_temp.begin(),ts_temp.end());
				find_target_arrage(0);

				CCARRAY_FOREACH(m_caTarget,toe){
					if(toe == m_controller) continue;					// <不以自己为对象
					((EChesses*) toe)->miHitGroup |= 1;
					if(ccpDistance(m_mou_cur, ((EChesses*) toe)->pos) == 1) {
						check = true;
						((EChesses*) toe)->miHitGroup |= 2;
					}
				}			

				ts_last.insert(make_pair(a_cp.x,a_cp.y));

				if(!check) {
					mbLink = false;
					CCLog(">[BattleMap] Skill_XJ_YS | No player caught.");
					return;
				}

				break;
			}
		}
	}

	calLink(a_cp);
}

bool BattleMap::arange_targetwithtest( int a_type )
{
	do 
	{
		CCLog(">[BM] arange_targetwithtest(int:%d)[%d,%d]",a_type,stx,sty);

		pair<int,int> t_pii = make_pair(m_mou_cur.x,m_mou_cur.y);
		CC_BREAK_IF(cs_y.count(t_pii) == 0 && cs_block.count(t_pii) == 0);
		CC_BREAK_IF(cs_dis.count(t_pii) > 0);

		return arange_target(a_type);
	} while (0);
	return false;
}


bool BattleMap::f_Arange( int a_type , CCObject* atar)
{
	do 
	{
		EChesses* t_cfie = (EChesses*) atar;

		pair<int,int> t_pii = make_pair(m_mou_cur.x,m_mou_cur.y);
		CC_BREAK_IF(cs_cy.count(t_pii) == 0&&cs_y.count(t_pii) == 0 && cs_block.count(t_pii) == 0);
		CC_BREAK_IF(cs_dis.count(t_pii) > 0);

		return ts_last.count(make_pair(t_cfie->pos.x,t_cfie->pos.y)) > 0;
	} while (0);
	return false;
}

void BattleMap::clear_mcaTarget(){
	CCObject* toe;
	CCARRAY_FOREACH(m_caTarget,toe){
		((EChesses*) toe)->miHitGroup = 0;
	}
	CC_SAFE_RELEASE_NULL(m_caTarget);
	CC_SAFE_RELEASE_NULL(m_caTarCharas);

	m_caTarCharas = new CCArray();
	m_caTarget = new CCArray();
}

void BattleMap::find_target_arrage(int a_type, set<pair<int,int>> &a_dt){
	CCDictElement* t_cde = NULL;

	//CC_SAFE_RELEASE_NULL(m_caTarCharas);
	//CC_SAFE_RELEASE_NULL(m_caTarget);

	//m_caTarCharas = new CCArray();
	//m_caTarget = new CCArray();
	//int g_id = m_controller->group_id;

	switch (a_type)
	{
	case -1:			// <全员伤害型
		{
			CCDICT_FOREACH(m_itemlist,t_cde){
				EChesses* t_cfie = (EChesses*) t_cde->getObject();
				if(a_dt.count(make_pair(t_cfie->pos.x,t_cfie->pos.y)) > 0){
					if(!m_caTarget->containsObject(t_cfie)) m_caTarget->addObject(t_cfie);
					if(!m_caTarCharas->containsObject(t_cfie->m_pChara)) m_caTarCharas->addObject(t_cfie->m_pChara);
				}

			}
			break;
		}
	case 0:				// <标准索敌模式
		{
			CCDICT_FOREACH(m_itemlist,t_cde){
				EChesses* t_cfie = (EChesses*) t_cde->getObject();
				if(a_dt.count(make_pair(t_cfie->pos.x,t_cfie->pos.y)) > 0){
					if(EGroup::sharedEGroup()->IsEnemy(m_controller, t_cfie)){
						if(!m_caTarget->containsObject(t_cfie)) m_caTarget->addObject(t_cfie);
						if(!m_caTarCharas->containsObject(t_cfie->m_pChara)) m_caTarCharas->addObject(t_cfie->m_pChara);
					}
				}

			}
			break;
		}
	case 1:				// <只寻找友军单位
		{
			CCDICT_FOREACH(m_itemlist,t_cde){
				EChesses* t_cfie = (EChesses*) t_cde->getObject();
				if(a_dt.count(make_pair(t_cfie->pos.x,t_cfie->pos.y)) > 0){

					if(! (EGroup::sharedEGroup()->IsEnemy(m_controller, t_cfie))){
						if(!m_caTarget->containsObject(t_cfie)) m_caTarget->addObject(t_cfie);
						if(!m_caTarCharas->containsObject(t_cfie->m_pChara)) m_caTarCharas->addObject(t_cfie->m_pChara);
					}
				}

			}
			break;
		}


	default:
		break;
	}

}

void BattleMap::find_target_arrage(int a_type) 
{
	find_target_arrage(a_type, ts_last);

}

// <检查范围内是否有指定的单位类型
bool BattleMap::arange_target( int a_type )
{
	CCLog(">[BM] Arrange_target(int:%d) || Find target Begins here.",a_type);
	find_target_arrage(a_type);

	CCLog(">[BM] Arrange_target(int:%d)||>>>>>>>>>>>>>>>>>>>pass the lck&passing to BattleField:%d <<<<<<<<<<<<<<<<<<<<<<<<", a_type, m_caTarget->count());
	// [1103] <临时性的，小心技能不执行 :D
	//if(b_battle>= 6) return m_caTarget->count()>0;
	if(SkillJudge::sharedSkillJudge()->mbJudging){
		stx = -10;
		sty = -10;
		return m_caTarget->count()>0;
	}

	CCLog(">[BM] Arrange_target(int:%d)||pass the lck&passing to BattleField:%d", a_type, m_caTarget->count());
	if(m_caTarget->count()>0){
		BattleField::sharedBattleField()->SetTars(m_caTarget);
		return true;
	}else{
		BattleField::sharedBattleField()->SetTars(NULL);
		return false;
	}

}

void BattleMap::show_text(EChesses* a_ec,string s)
{

	CCLabelBMFont* c_ttlbmf = CCLabelBMFont::create(s.c_str(),FNT_CHN);
	c_ttlbmf->setAnchorPoint(CCPointZero);
	c_ttlbmf->setVertexZ(a_ec->getVertexZ());
	c_ttlbmf->setPosition(a_ec->getPosition());
	c_ttlbmf->setZOrder(a_ec->getZOrder());
	//	c_ttlbmf->setTag(0x299);
	addChild(c_ttlbmf,11);
	//	mt_EffectList->addObject(c_ttlbmf);

	CCActionInterval* t_cai = CCSpawn::createWithTwoActions(CCMoveTo::create(0.3,ccpAdd(c_ttlbmf->getPosition(),ccp(0,100))),CCFadeOut::create(0.3));
	c_ttlbmf->runAction(CCSequence::createWithTwoActions(t_cai,CCCallFuncO::create(this,callfuncO_selector(WalkMap::effectback),c_ttlbmf)));
}

void BattleMap::show_text( string s )
{
	for(int i = 0; i< m_caTarget->count(); ++i){
		show_text((EChesses*) m_caTarget->objectAtIndex(i),s);
	}

}

void BattleMap::control_switch()
{
	m_bAnimateOver = true;
	clean_cs();


}

void BattleMap::clearcontrol()
{
	CC_SAFE_RELEASE_NULL(m_touch);
	TileMap::clearcontrol();
}

void BattleMap::clear_Arrange()
{
	stx = -10;
	sty = -10;
}

void BattleMap::HandleScriptor( Scriptor* asp )
{
	// m_caTarget	<被选中的目标单位
	// ts_last		<被选中的所有点的集合
	GameManager::sharedLogicCenter()->ml->m_iItem = -1;
	BattleField::sharedBattleField()->meSrc = (EChesses*) m_controller;
	
	clear_Arrange();
	BattleField::sharedBattleField()->mCachedSPp.clear();
	BattleField::sharedBattleField()->mCachedSPpScs.clear();
	//if(!m_caTarget) return;
	
	CCArray* t_caS = asp->m_caScript;
	
	CCLog(">[BM] HandleScriptor() || Tying to pass sp to owner unit....");
	Script* tsc;
	if(t_caS->count()<3){
		tsc = NULL;
	}else{
		tsc = (Script*) t_caS->objectAtIndex(2);
	}
	ChangeAllFace();

	BattleField::sharedBattleField()->SetUp((EChesses*) m_controller,m_caTarget,tsc);
	BattleField::sharedBattleField()->CheckBackCh();

	((EChessComp*) m_controller->getComponent("controller"))->mbDebugS = false;
	((EChessComp*) m_controller->getComponent("controller"))->RunScript((Script*) t_caS->objectAtIndex(0));
	

	CCLog(">[BM] HandleScriptor() || Passing owner is over...");

	if(m_caTarget){
		
		
		for(int i = 0; i< m_caTarget->count(); ++i){
			EChesses* ttar = ((EChesses*) m_caTarget->objectAtIndex(i));
			if(ttar->miHitGroup == 0 || (2 & ttar->miHitGroup)){	// <如果需要更加复杂的机制请从脚本中读取配置替换 2
				((EChessComp*) m_controller->getComponent("controller"))->setSrc(ttar);
				break;
			}
			
		}

		CCLog(">[BM] HandleScriptor() || Getter:%d",m_caTarget->count());
		for(int i = 0; i< m_caTarget->count(); ++i){
			
			CCLog(">[BM] HandleScriptor() || Tying to pass sp to getter unit-%d....",i);
			EChesses* ttar = ((EChesses*) m_caTarget->objectAtIndex(i));
			if(!ttar) CCLog(">[BM] HandleScriptor() || Critical Bug. No EChess matched");
			ttar->setState(3);
			//ttar->ChangeFace(m_con_cur);
			EChessComp* ttecc = ((EChessComp*) (ttar->getComponent("controller")));

			//ttecc->mbDebugS = false;
			if(!ttecc) CCLog(">[BM] HandleScriptor() || Critical Bug. No ChessComp matched");
			ttecc->RunScript((Script*) t_caS->objectAtIndex(1));	
			ttecc->setSrc(m_controller);
		}

		if(BattleField::sharedBattleField()->miActCount == 0){
			if(m_caTarget->count()>0)
				GameManager::sharedLogicCenter()->ml->m_rsb->SetContent((EChesses*) m_caTarget->objectAtIndex(0));
		}
			
	}

	asp->re_init();
	if(BattleField::sharedBattleField()->mbNeLock){
		BattleField::sharedBattleField()->mbNeLock = false;
		((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRelease();
	}

	if(BattleField::sharedBattleField()->mbCurMagic){
		((EChesses*) m_controller)->m_pChara->miSeType = 4;
		BattleField::sharedBattleField()->PlaySe(((EChesses*) m_controller)->m_pChara);
	}
	
	//CC_SAFE_RELEASE_NULL(m_caTarget);
}

bool BattleMap::ArrangePoint( CCPoint a )
{
	return ts_last.count(make_pair(a.x,a.y)) > 0;
}

void BattleMap::fAutoCamara()
{
	//CCLog(">[BattleMap] fAutoCamara()| Filt.Bef. ");
	if(b_battle == 2 || b_battle == -1) return;
	//CCLog(">[BattleMap] fAutoCamara() 1");
	if(mbLockCamera) return;
	//CCLog(">[BattleMap] fAutoCamara() 2");
	if(!m_touch) return;
	//CCLog(">[BattleMap] fAutoCamara() ");
	

	CCPoint t_mouse;
	float sw = CCDirector::sharedDirector()->getVisibleSize().width;
	float sh = CCDirector::sharedDirector()->getVisibleSize().height;

	float nx,ny;
	nx = m_touch->getLocation().x;
	ny = m_touch->getLocation().y;

	int a = 0;

	ny = sh-ny;
	//CCLog("TMOUSE:%f,%f||sw,sh:%f,%f",);
	//if(abs(m_touch->getLocation().y - sh/2) < sh*0.4 && abs(m_touch->getLocation().x - sw/2) < sw*0.4) return;
	if(ny < sh/10){
		ny = ny + 0.4 * sh;
	}else if(ny<0.9*sh){
		ny = 0.5 * sh;
		++a;
	}else{
		ny = ny - 0.4 * sh;		
	}
	

	if(nx < sw/10){
		nx = nx + 0.4 * sw;
	}else if(nx<0.9*sw){
		nx = 0.5 * sw;
		++a;
	}else{
		nx = nx - 0.4 * sw;
	}

	
	//nx = 450;
	//ny = 300;

	if(a==2) return;
	static CCTouch* touch = new CCTouch();

	touch->setTouchInfo(m_touch->getID(),nx,ny);
	t_mouse = this->convertTouchToNodeSpace(touch);
	//t_mouse = ccpAdd(t_mouse,ccp(-dtx,-dty));
	mFolCamara->setPosition(t_mouse);

	this->stopActionByTag(0x997);
	TFollow* ttf = TFollow::create(mFolCamara, CCRectMake(-dx, -dy, rw, rh));
	ttf->miID = 997;
	ttf->setTag(0x997);

	this->runAction(ttf);
	ttf->FactMove();
	

}

//////////////////////////////////////////////////////////////////////////
// <独立的范围测定函数
bool BattleMap::f_RangeTest(int a_type, vector<int> a_ran, CCPoint a_cp,CCPoint a)
{	
	set<pair<int,int>> ts_range;

	switch(a_type)
	{
	case(0):
		{
			ts_range.insert(make_pair(a_cp.x,a_cp.y));
			break;
		}
	case(1):				// 1 is default a circle.
		{
			if(cs_y.count(make_pair(a_cp.x,a_cp.y)) + cs_cy.count(make_pair(a_cp.x,a_cp.y)) > 0)
			{
				CCLog(">[BM_Id Rag Che] I forget why these is being check. So i disable it.");
			}
			int radiu = a_ran[0];
			dps_rect(a_cp, ts_range, radiu);
			break;
		}
	}

	return ts_range.count(make_pair(a.x,a.y)) > 0;
}


//////////////////////////////////////////////////////////////////////////
// <技能 09-27
void BattleMap::setLink(int aiLink)
{
	miLink = aiLink;
	mbLink = true;		// <默认为真，不满足条件时会被置为假。
}

bool BattleMap::testLink()
{
	return mbLink;
}

/* <单位判定也被转移到了这个函数里面 */
void BattleMap::calLink(CCPoint a_cp/* <当前鼠标所在的点*/)
{
	switch (GameManager::sharedLogicCenter()->ml->m_iFuncType)
	{
	case 32:
		CCLog(">[BattleMap] calLink() | Mouse.Move. No. Need for calLink...");
		return;
	default:
		break;
	}

	CCLog(">[BattleMap] m_caTarget:%d", m_caTarget->count());
	if(m_caTarget->count() == 0){
		CCLog(">[BattleMap] CalLink() | Range it.");
		mbLink = arange_targetwithtest(miRangeType);
	}else{
		CCLog(">[BattleMap] CalLink() | No Need to cal.");
		
		mbLink = true;
		BattleField::sharedBattleField()->SetTars(m_caTarget);
	}
	
	CCLog(">[CalLink] b:%d | r:%d", mbLink, miRangeType);
	switch (miLink)
	{
	case 1:		// < miLink == 1 || 该点上不可以存在其他人物。无视是否有覆盖到单位。 [矿石突袭]
		{
			mbLink = cs_y.count(make_pair(a_cp.x,a_cp.y));
			mbLink &= !BattleField::sharedBattleField()->HasChess(a_cp.x,a_cp.y);
			mbLink &= (mBlockSet.count(make_pair(a_cp.x,a_cp.y)) == 0);

			CCLog(">[CalLink] b:%d ", mbLink);
			break;
		}
	default:
		{
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// <技能跳转执行类 || 由技能发起方主动跳转或者由被攻击单位拉取跳转。

/* <移动指定的单位的位置 || 位移是相对的 */
void BattleMap::moveChess(EChesses* at, CCPoint ta, bool isRelate)
{
	CCPoint tOrigCP = at->pos;
	CCPoint tEndCP;

	if(isRelate){
		tEndCP = tOrigCP + ta;
	}else{
		tEndCP = ta;		
	}

	ta = m_getViewc(tEndCP);

	b2Vec2 d = at->m_body->GetPosition();
	//at->m_body->SetTransform(b2Vec2((ta.x+dtx)/PTM_RATIO,(ta.y+dty)/PTM_RATIO),0);

	EventCenter::sharedEventCenter()->mQueuedTrans.push(make_pair(at->m_body, make_pair((ta.x+dtx)/PTM_RATIO, (ta.y+dty)/PTM_RATIO)));
	BattleField::sharedBattleField()->ChessMoved(at, tOrigCP, tEndCP);
	at->pos = tEndCP;
	f_setcamara(at);
}

/* <以鼠标所点击的地方为落点 */
void BattleMap::moveChessConcru( EChesses* at )
{
	moveChess(at,m_mou_cur,false);
}

void BattleMap::ChangeFace( EChesses* asrc, EChesses* atar )
{
	asrc->ChangeFace(atar->pos);
}

void BattleMap::ChangeFaceConcur( EChesses* atar )
{
	ChangeFace((EChesses*) m_controller,atar);
}

void BattleMap::ChangeAllFace(CCObject* aeSrc, CCObject* aeTar)
{
	CCLog(">[BattleMap] ChangeAllFace()");
	if(!m_caTarget) return;

	EChesses* teSrc = (EChesses*) aeSrc;
	if(!teSrc)	teSrc = BattleField::sharedBattleField()->meSrc;

	EChesses* teTar = (EChesses*) aeTar;
	if(!teTar){
		if(m_caTarget->count()>0)
			teTar = ((EChesses*) m_caTarget->objectAtIndex(0));
		else
			teTar = (EChesses*) m_controller;
	}

	if(m_caTarget->count()>0) teSrc->ChangeFace(teTar->pos);
	else teSrc->ChangeFace(m_mou_cur);

	teSrc->playAnimate("stand",-1);

	CCLog(">[BattleMap] ChangeAllFace() | Change (%d) Tars.", m_caTarget->count());
	for(int i = 0; i< m_caTarget->count(); ++i){
		((EChesses*) m_caTarget->objectAtIndex(i))->ChangeFace(((EChesses*) m_controller)->pos);
		((EChesses*) m_caTarget->objectAtIndex(i))->playAnimate("stand",-1);
	}
}

//////////////////////////////////////////////////////////////////////////

void BattleMap::PauseAllActs()
{
	CCDictElement* cde;
	CCDICT_FOREACH(m_itemlist,cde){
		EChesses* t_ec = (EChesses*) cde->getObject();
		t_ec->pauseSchedulerAndActions();
		t_ec->PauseMe();
	}
}

void BattleMap::ResumeAllActs()
{
	CCDictElement* cde;
	CCDICT_FOREACH(m_itemlist,cde){
		EChesses* t_ec = (EChesses*) cde->getObject();
		t_ec->resumeSchedulerAndActions();
		t_ec->ResuMe();
	}
}

//////////////////////////////////////////////////////////////////////////

void BattleMap::PauseForVod(){
	CCDictElement* cde;
	CCDICT_FOREACH(m_itemlist,cde){
		EChesses* t_ec = (EChesses*) cde->getObject();
		t_ec->pauseSchedulerAndActions();
		//t_ec->PauseMe();
	}
}

void BattleMap::ResuemForVod(){
	CCDictElement* cde;
	CCDICT_FOREACH(m_itemlist,cde){
		EChesses* t_ec = (EChesses*) cde->getObject();
		t_ec->resumeSchedulerAndActions();
		//t_ec->ResuMe();
	}
}

//////////////////////////////////////////////////////////////////////////

void BattleMap::set_bbattle( int i )
{
	b_battle = i;
}

int BattleMap::get_bbattle()
{
	return b_battle;
}

/* <所有的EChess单位全部在载入地图时载入完成 | 载入存档时只需要改变位置和可能的Chara的属性。 */
void BattleMap::PlaceEChesses(Script* scp)
{
	CCDictionary* tItemList = new CCDictionary();
	if(scp){
		CCArray *tca = scp->scriptnodes;
		CCObject *to;
		string tname;
		EChesses* tec;

		CCARRAY_FOREACH(tca, to){
			Script* ts = (Script*) to;
			int tid = ts->getint("id");
			tname = ts->getstring("name");
			tec = (EChesses*) m_itemlist->objectForKey(tname);

			CCLog(">[BattleMap] PlaceEChesses() | Name:%s - Head:%s", tec->m_pChara->m_sName.c_str(), tec->m_pChara->msBHead.c_str());
			if(0 == tid){	// <对于敌方单位由于不在CharaState中，需要重新载入属性
				tec->load_chara_dbsp((Script*) ts->scriptnodes->objectAtIndex(0));
			}
			tec->pos = ccp(ts->getint("x"), ts->getint("y"));
			tItemList->setObject(tec,tname);
			m_itemlist->removeObjectForKey(tname);
			
		}

		// <由于全部都还没有初始化，所以需要做的清理工作较少。
		CCDictElement* tccde;
		CCDICT_FOREACH(m_itemlist,tccde){
			EChesses* ttec = (EChesses*) tccde->getObject();
			ttec->removeFromParent();
		}
		CC_SAFE_RELEASE_NULL(m_itemlist);
		m_itemlist = tItemList;
	}

	CCDictElement *tcde;
	CCDICT_FOREACH(m_itemlist, tcde){
		EChesses* t_ec = (EChesses*) tcde->getObject();
		CCLog(">[BattleMap] PlaceEChesses() | Final | Name:%s - Head:%s", t_ec->m_pChara->m_sName.c_str(), t_ec->m_pChara->msBHead.c_str());
		BattleField::sharedBattleField()->SetChess(t_ec,t_ec->pos.x,t_ec->pos.y);
	}
	
}

void BattleMap::LockCamera(CCObject* e)
{
	if(!e) return;
	mbLockCamera = true;
	EChesses* tec = (EChesses*) e;
	m_sNaCam = tec->GetFullName();

	this->stopActionByTag(0x997);
	TFollow* ttf = TFollow::create(tec, CCRectMake(-dx, -dy, rw, rh), true);
	ttf->setTag(0x997);
	this->runAction(ttf);
	ttf->FactMove();
	//this->update(0.1);
	this->stopAction(ttf);
}

void BattleMap::f_setcamara(CCObject* e)
{
	if(!e) return;

	CCLog(">[BattleMap] f_setcamara()");

	EChesses* tec = (EChesses*) e;
	m_sNaCam = tec->GetFullName();

	this->stopActionByTag(0x997);
	TFollow* ttf = TFollow::create(tec, CCRectMake(-dx, -dy, rw, rh));
	ttf->miID = tec->m_pChara->m_iCharaID;

	ttf->setTag(0x997);
	this->runAction(ttf);
}

void BattleMap::ScrollBacKChess()
{
	if(GameManager::sharedLogicCenter()->ml->mbNeedScroll) {
		GameManager::sharedLogicCenter()->ml->mbNeedScroll = false;
		if(micECx<0 || micECy<0){
			CCLog(">[BattleMap] ScrollBackChess() | No cache. No scroll.");
			return;
		}
		moveChess((EChesses*) m_controller, ccp(micECx,micECy), false);
	}

	GameManager::sharedLogicCenter()->ml->mbSBProtect = false;
	BattleField::sharedBattleField()->miState = 0;
	f_setcamara(m_controller);
	CC_SAFE_DELETE(m_touch);
	RemoveSCBKState();
}

void BattleMap::RemoveSCBKState()
{
	GameManager::sharedLogicCenter()->ml->mbNeedScroll = false;
	micECx = -1;
	micECy = -1;
}

void BattleMap::RemoveWhileDead(CCObject* achess)
{
	EChesses* tEc = (EChesses*) achess;

	m_itemlist->removeObjectForKey(tEc->name) ;	// <通知BattleMap进行处理
	m_caTarCharas->removeObject(tEc->m_pChara);
	_world->DestroyBody(tEc->m_body);				// < 保存列表并交付给逻辑循环处理[添加对应的函数接口] || world.destroyBody(b);

	m_caTarCharas->removeObject(tEc->m_pChara);
	m_caTarget->removeObject(tEc->m_pChara);

	if(m_controller == tEc) m_controller = nullptr;
}


