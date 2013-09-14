#include "BattleMap.h"

#include "SingleTon/BattleFiled.h"
#include "GameManager.h"

#include "hud/TFollow.h"

#define ASTAR_DEPTH 2500

BattleMap::~BattleMap(){
	CC_SAFE_RELEASE_NULL(m_touch);
	CC_SAFE_RELEASE_NULL(m_caTarget);
	CC_SAFE_RELEASE_NULL(m_caTarCharas);
}

bool BattleMap::init()
{
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
	vx= 0;vy = 0;
	m_touch = NULL;
	b_battle = -1;
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

	CCSize ms = m_tilemap->getMapSize();
	CCSize ts = m_tilemap->getTileSize();

	float r = (ms.width + ms.height) / 2;
	rw = r * ts.width;
	rh = r * ts.height;

	max_x = m_tilemap->getMapSize().width;
	max_y = m_tilemap->getMapSize().height;
	int mapWidth = m_tilemap->getMapSize().width * m_tilemap->getTileSize().width;
	int mapHeight = m_tilemap->getMapSize().height * m_tilemap->getTileSize().height;
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

	m_debugDraw =  new GLESDebugDraw( PTM_RATIO );
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
	b_battle = 1;

	//////////////////////////////////////////////////////////////////////////
	mFolCamara = CCNode::create();
	mFolCamara->setAnchorPoint(CCPointZero);
	addChild(mFolCamara);
	//////////////////////////////////////////////////////////////////////////
	//mv_iiGroup.clear();

	return true;
}


void BattleMap::f_decide(int i, int j){		// <通过新的选中tile对map进行重构。 use the set to decide.
	imply_set(ts_last,0);
	center_i = i;
	center_j = j;
	ts_last.clear();

	imply_set(cs_hit_block,c_y);

	CCTMXTiledMap* map = (CCTMXTiledMap*) getChildByTag(kTagMap);
	CCTMXLayer* layer = map->layerNamed("Battle");

	unsigned int gid = layer->tileGIDAt(ccp(i,j));

	draw_mouse_range(ccp(i,j));

	m_mou_cur = ccp(i,j);
	
	imply_set(ts_last,c_r);
	imply_set(cs_dis,c_b);			//blue is higher than red.

}

void BattleMap::update(float dt)
{

	switch(b_battle){
	case(-1):		//Wait For UnLock & Switch Control
		{
			CC_BREAK_IF(!cancontrol);
			m_bAnimateOver = true;
			BattleField::sharedBattleField()->AMOver();
			
			break;
		}
	case(1):						// state == 1 : <点击选择单位.
		{
			CC_BREAK_IF(!m_touch);
			fAutoCamara();
			checkpoint(m_touch);
			BattleField::sharedBattleField()->ShowChess(m_eCurMouse);
			break;
		}
	case(3):						// state == 3 : <移动鼠标并选择目标.
		{		
			CC_BREAK_IF(!m_touch);
			fAutoCamara();
			m_touchpoint = this->convertTouchToNodeSpace(m_touch);

			m_touchpoint = ccpAdd(m_touchpoint,ccp(-dtx,-dty));
			CCPoint ti = m_getTilec(m_touchpoint);

			int i = round(ti.x);
			if(i >= max_x) i = max_x -1;
			int j = round(ti.y);		
			if(j >= max_y) j = max_y - 1;
			f_decide(i,j);				

			arange_target(miRangeType);

			break;
		}
	case(4):		// state == 4 : Map is bussy moving charaentile.
		{
			CC_BREAK_IF(!m_touch);
			((EChessComp*) m_controller->getComponent("controller"))->move_by_path(vc_path);
			m_bAnimateOver = false;
			b_battle = -1;
			break;
		}
	case(5):
		{
			b_battle = -1;
			m_bAnimateOver = false;
			break;
		}
	}

		
	

	update_collide();
	update_b2world(dt);
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

/* <生成敌人单位 [EChess-Chara] */
void BattleMap::f_generateEnemy( int i )
{
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
	vdata.clear();
	m_sSql = CCString::createWithFormat("select * from enemy_list where id IN (%s)", t_mask.c_str())->getCString();
	vdata = DBUtil::getDataInfo(m_sSql,NULL);
	DBUtil::closeDB();

	BattleField::sharedBattleField()->InitBfSp(t_scps.c_str());

	m_miEnemis.clear();
	// <生成可以使用的敌方单位属性索引表
	for(int i = 0;i<vdata.size();++i){
		map<string,string> t_ssm = (map<string,string>) vdata.at(i);
		int t_fi_id	=	stoi(t_ssm.at("id"));
		m_miEnemis[t_fi_id] = i;
	}

	// <生成敌方单位
	Scriptor* t_scp = new Scriptor();
	stringstream teststream;
	teststream<<t_is;
	int t_id,t_x,t_y;
	string t_name;
	do{
		t_id = 0;
		teststream>>t_id;		// <类型ID
		if(t_id < 1) break;
		teststream>>t_x;		// <横坐标
		teststream>>t_y;		// <纵坐标
		teststream>>t_name;		// <敌人的唯一标示
		/* [id x y s_name] */

		map<string,string> t_ssm = (map<string,string>) vdata.at(m_miEnemis[t_id]);

		t_scp->re_init();
		t_scp->parse_string(t_ssm.at("attr"));		// <读取属性

		//////////////////////////////////////////////////////////////////////////
		EChesses* t_fij_ecd = new EChesses();

		t_fij_ecd->load_chara_dbsp((Script*) t_scp->m_caScript->objectAtIndex(0));


		t_fij_ecd->psz	=	t_ssm.at("spx");			//Test Only. SPX 入口
		t_fij_ecd->pos	=	ccp(t_x,t_y);
		t_fij_ecd->group_id = 0x02;
		t_fij_ecd->group_mask = 0x01;

		t_fij_ecd->name	=	t_name;
		m_itemlist->setObject(t_fij_ecd,t_fij_ecd->name);
		//t_fij_ecd->autorelease();

		t_fij_ecd->m_pChara->m_sName		 =	 t_ssm.at("name");			//TODO:IN:PB: all the generated enemy may use the same chara(), if so put down this to next stage.
		t_fij_ecd->m_pChara->m_sPsz		 =	 t_ssm.at("psz");
		t_fij_ecd->m_pChara->m_iElement	 =	 stoi(t_ssm.at("element"));

		BattleField::sharedBattleField()->SetChess(t_fij_ecd,t_fij_ecd->pos.x,t_fij_ecd->pos.y);

	} while(1);
	CCLOG(">Enemy Generationg Over.");

	CC_SAFE_DELETE(t_scp);


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
		t_ec->group_id = 0x01;
		t_ec->group_mask = 0x02;
		t_ec->m_pChara = t_cca;
		t_ec->name = CCString::createWithFormat("chara_%d",tmp->getint("name"))->getCString();
		t_ec->psz  = t_cca->m_sSpx;//tmp->getstring("file");//t_cca->m_sPsz;			//Spx									//Whether use the same psz is due to further design.

		m_itemlist->setObject(t_ec,t_ec->name);									//Test: get one and only one.

		BattleField::sharedBattleField()->SetChess(t_ec,t_ec->pos.x,t_ec->pos.y);

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
	ballShapeDef.isSensor = true;				// 将碰撞形状改为sensor不会提高多少效率却会破坏map原本事件机制。
	
	//// b2body准备工作
	//////////////////////////////////////////////////////////////////////////
	CCSize ts = m_tilemap->getTileSize();
	b2PolygonShape boxShape;
	float hbw = (ts.width-1)/( PTM_RATIO * 4);
	float hbh = (ts.height-1)/( PTM_RATIO);
	hbh *= 1.2;
	boxShape.SetAsBox(hbw,hbh,b2Vec2(0,hbh),0);


	CCDictElement* cde = NULL;
	CCDICT_FOREACH(m_itemlist,cde){
		EChesses* t_ec = (EChesses*) cde->getObject();
		
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

		b2FixtureDef boxFixtureDef;				
		boxFixtureDef.shape = &boxShape;
		boxFixtureDef.density = 1;
		boxFixtureDef.filter.categoryBits = 0x0002;
		boxFixtureDef.filter.maskBits = 0x0001;
		boxFixtureDef.isSensor = true;

		_body->CreateFixture(&boxFixtureDef);
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
	case(1):				// 1 is default a circle.
		{
			int radiu = a_ran[0];
			dps_rect(t_ce->pos, cs_y, radiu);
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

	int t_iMove = 16;			//TODO: Test only. Get it from chara please.
	int i = m_con_cur.x;
	int j = m_con_cur.y;

	CCLOG(">Prepare Drawing Layer 1:%d,%d.",i,j);

	cs_hit_block.clear();
	cs_y.clear();				//cs_y存储第一层
	cs_cy.clear();
	draw_moving_block();
	dps_range(m_con_cur, cs_y, t_iMove);
	cs_y.erase(make_pair(m_con_cur.x,m_con_cur.y));
	imply_set(cs_y,c_y);			//Imply.
	imply_set(cs_dis,c_b);
	imply_set(cs_hit_block,c_y);
	//[0803]CCLog(">[BM]cs_dis:%d|cs_block:%d",cs_dis.size(),cs_block.size());

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
		if(t_ec->group_id & m_controller->group_mask){	// For Moving | cs_block is for calculating && cs_dis is for moving deciding while mouse move.
			int t_range = 2;							//TODO: Test. ZOC Range = 1; type = ring
			dps_ring(t_ec->pos, cs_block, t_range);
		}else{
			cs_block.insert(make_pair(t_x,t_y));		// <友军所站的位置不能停泊。
			//cs_dis.insert(make_pair(t_x,t_y));
		}
	}

	cs_block.erase(make_pair(m_con_cur.x,m_con_cur.y));
}

void BattleMap::dps_ring( CCPoint a_cp , set<pair<int,int>> &a_dt, int a_max)
{	
	int t_x = a_cp.x;
	int t_y = a_cp.y;
	a_dt.insert(make_pair(t_x,t_y));

	//no one can block ring, we just add them into set.

	for(int i = 0; i<a_max; ++i){
		a_dt.insert(make_pair(t_x+i,t_y));
		a_dt.insert(make_pair(t_x-i,t_y));
		a_dt.insert(make_pair(t_x,t_y+i));
		a_dt.insert(make_pair(t_x,t_y-i));
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

	//CCLOG(">Dps_range_begin:%d,%d",t_x,t_y);
	
	if(cs_block.count(make_pair(t_x,t_y))>0){
		cs_hit_block.insert(make_pair(t_x,t_y));
		return;
	}
	if(a_dt.count(make_pair(t_x,t_y))>0) return;
	a_dt.insert(make_pair(t_x,t_y));
	if((abs(t_x - m_con_cur.x) + abs(t_y - m_con_cur.y)) == a_max) return;
	dps_range(ccpAdd(a_cp,ccp(0,1)), a_dt, a_max);
	dps_range(ccpAdd(a_cp,ccp(0,-1)), a_dt, a_max);
	dps_range(ccpAdd(a_cp,ccp(1,0)), a_dt, a_max);
	dps_range(ccpAdd(a_cp,ccp(-1,0)), a_dt, a_max);
	//CCLOG(">Dps_range[%d]_back:%d,%d",a_indent,t_x,t_y);
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
	do 
	{
		pair<int,int> t_pii = make_pair(m_mou_cur.x,m_mou_cur.y);
		CC_BREAK_IF(cs_y.count(t_pii) == 0 && cs_hit_block.count(t_pii) == 0);
		CC_BREAK_IF(cs_dis.count(t_pii) > 0);

		//[0803]CCLog(">Try to move all the element.");
		a_star();
		b_battle = 4;
		vc_path.pop_back();		// <最后一个点是起点，所以将其弹出。

		((EChesses*) m_controller)->pos = m_mou_cur;
		m_mou_cur = ccp(-1,-1);
		return true;
	} while (0);
	return false;
}

void BattleMap::a_star()		// <结果被存储在vc_path中。
{
	vc_path.clear();
	list<StepNode> lsn = getSearchPath(m_con_cur,m_mou_cur);
	BattleField::sharedBattleField()->ChessMoved((EChesses*)m_controller, m_con_cur, m_mou_cur);
	vc_path.push_back(m_mou_cur);
	lsn.pop_back();

	int t_idirect = 0;
	StepNode t_sn = lsn.back();
	int dx = m_mou_cur.x - t_sn.x;
	int dy = m_mou_cur.y - t_sn.y;
	//[0803]CCLog(">BEGIN POINT:%f,%f.",m_mou_cur.x,m_mou_cur.y);

	if(dx > 0) t_idirect = 3;
	else if(dx < 0) t_idirect = 2;
	else if(dy > 0) t_idirect = 4;
	else t_idirect = 1;

	//t_idirect = t_sn.status;

	dx = t_sn.x;
	dy = t_sn.y;
	bool turn_lock = true;		//The flag identity that last move is a turn.

	set<pair<int,int>> t_ps;
	t_ps.insert(make_pair(m_mou_cur.x,m_mou_cur.y));
	for(list<StepNode>::reverse_iterator it = lsn.rbegin(); it != lsn.rend(); ++it){
		if(it->x != dx || it->y != dy) continue;
		if(t_ps.count(make_pair(dx,dy)) > 0) continue;
		t_ps.insert(make_pair(dx,dy));
		t_sn = *it;
		//[0803]CCLog(">NextParentGot:%d,%d G:%d H:%d F:%d",dx,dy,t_sn.G,t_sn.H,t_sn.getF());


		if(t_sn.status != t_idirect){
			vc_path.push_back(ccp(t_sn.x,t_sn.y));
			
			t_idirect = t_sn.status;
			turn_lock = true;
			//[0803]CCLog(">Turned:%d,%d.",dx,dy);
		}else{
			//[0803]CCLog(">UnTurned:%d,%d.",dx,dy);
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

	//[0803]CCLog(">A_STAR is over. Try to check the path.");
	for(vector<CCPoint>::iterator it = vc_path.begin(); it != vc_path.end(); ++it){
		//[0803]CCLog(">Point:%f,%f.",it->x,it->y);
	}
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
	//查找steps格范围内的目标
	int steps = ASTAR_DEPTH;			//20x20 = 400; IN:小心
	int last_f = 500;
	//初始点
	StepNode startNode;
	startNode.x = startPos.x;
	startNode.y = startPos.y;
	startNode.G = 0;
	startNode.H = getNodeH(to, startNode);;
	startNode.status = 1;
	//[0803]CCLog(">[BM]StartNode Began>Start End---[%d,%d]-[%f,%f]",startNode.x,startNode.y,to.x,to.y);

	//记录点
	openNodeVec.push_back(startNode);

	bool isFind = false;
	
	StepNode nNod;
	nNod = startNode;

	while (!isFind && steps > 0){
		//以F值从大到小 降序排列
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
			//[0803]CCLog(">[BM]Layer count:%d||The Next Node is Added:%d,%d G:%d H:%f F:%f",ASTAR_DEPTH-steps,curNode.x,curNode.y,curNode.G,curNode.H,curNode.getF());
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
					break;
				}
				/*CHECK POINT 非阻挡点 是连续移动点*/
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
	}
	//[0803]CCLog(">[BM]A star is over, depth:%d",ASTAR_DEPTH-steps);
	return pathList;
}

bool BattleMap::as_checkpoint( int x, int y )
{
	bool bRet;
	bRet = cs_y.count(make_pair(x,y)) > 0;
	if(!bRet) CCLOG(">Get:%d,%d",x,y);
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

void BattleMap::draw_mouse_range(CCPoint a_cp)
{
	
	switch(m_mouse_type)
	{
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
	case(2):
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
				//////////////////////////////////////////////////////////////////////////
			}
			break;
		}
	}
}

bool BattleMap::arange_targetwithtest( int a_type )
{
	do 
	{
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


void BattleMap::find_target_arrage(int a_type) 
{
	CCDictElement* t_cde = NULL;
	CC_SAFE_RELEASE_NULL(m_caTarget);
	CC_SAFE_RELEASE_NULL(m_caTarCharas);
	m_caTarCharas = new CCArray();
	m_caTarget = new CCArray();
	int g_id = m_controller->group_id;
	if(a_type>0){
		CCDICT_FOREACH(m_itemlist,t_cde){
			EChesses* t_cfie = (EChesses*) t_cde->getObject();
			if(ts_last.count(make_pair(t_cfie->pos.x,t_cfie->pos.y)) > 0){

				if(! (t_cfie->group_mask & g_id)){
					m_caTarget->addObject(t_cfie);
					m_caTarCharas->addObject(t_cfie->m_pChara);
				}
			}

		}

	}else{
		CCDICT_FOREACH(m_itemlist,t_cde){
			EChesses* t_cfie = (EChesses*) t_cde->getObject();
			if(ts_last.count(make_pair(t_cfie->pos.x,t_cfie->pos.y)) > 0){
				if(t_cfie->group_mask & g_id){
					m_caTarget->addObject(t_cfie);
					m_caTarCharas->addObject(t_cfie->m_pChara);
				}
			}

		}
	}		//TODO: a_type < 0 ;
}

// <检查范围内是否有指定的单位类型
bool BattleMap::arange_target( int a_type )
{
	find_target_arrage(a_type);

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

void BattleMap::HandleScriptor( Scriptor* asp )
{
	// m_caTarget	<被选中的目标单位
	// ts_last		<被选中的所有点的集合
	CCArray* t_caS = asp->m_caScript;
	m_controller->ChangeFace(((EChesses*) m_caTarget->objectAtIndex(0))->pos);
	CCLog(">[BM]Tying to pass sp to owner unit....");
	Script* tsc;
	if(t_caS->count()<3){
		tsc = NULL;
	}else{
		tsc = (Script*) t_caS->objectAtIndex(2);
	}
	
	BattleField::sharedBattleField()->SetUp((EChesses*) m_controller,m_caTarget,tsc);
	((EChessComp*) m_controller->getComponent("controller"))->RunScript((Script*) t_caS->objectAtIndex(0));
	CCLog(">[BM]Passing owner is over...");
	for(int i = 0; i< m_caTarget->count(); ++i){
		CCLog(">[BM]Tying to pass sp to getter unit-%d....",i);
		((EChesses*) m_caTarget->objectAtIndex(i))->setState(3);
		((EChesses*) m_caTarget->objectAtIndex(i))->ChangeFace(m_con_cur);
		((EChessComp*) ((EChesses*) m_caTarget->objectAtIndex(i))->getComponent("controller"))->RunScript((Script*) t_caS->objectAtIndex(1));		
	}
	asp->re_init();
}

bool BattleMap::ArrangePoint( CCPoint a )
{
	return ts_last.count(make_pair(a.x,a.y)) > 0;
}

void BattleMap::fAutoCamara()
{
	if(b_battle == 2 || b_battle == -1) return;

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

	this->runAction(TFollow::create(mFolCamara, CCRectMake(-dx, -dy, rw, rh)));

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
				int radiu = a_ran[0];
				dps_rect(a_cp, ts_range, radiu);
			}
			break;
		}
	}

	return ts_range.count(make_pair(a.x,a.y)) > 0;
}

