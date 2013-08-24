#include "TileMap.h"
#include "AppMacros.h"


#include <cmath>
//#include "utils/SpotLight.h"


#include "GameManager.h"
#include "EventCenter.h"
#include "hud/TFollow.h"


TileMap::~TileMap(){
		//EventCenter::sharedEventCenter()->setController(NULL);
	removeAllChildren();
	CC_SAFE_DELETE(sp);

 	CC_SAFE_RELEASE_NULL(m_ea);
	CC_SAFE_RELEASE_NULL(m_itemlist);
	CC_SAFE_RELEASE_NULL(m_notuseditems);

	if(colse){
		for (vector<CollideInfo*>::iterator iter= colse->bc.begin();iter!= colse->bc.end();iter++)  
		{
			(*iter)->release();
		}
		colse->bc.clear();
	}


	CC_SAFE_DELETE(colse);
	CC_SAFE_DELETE(_world);		

}

//
//bool l_stop;
//Called by GS. & Apply magic effect here.[TODO_0722]
void TileMap::DerMapAct(Script* s){
	int type = s->getint("type");
	switch(type){
	case 0:				//transform to fa
		{
			Entiles* ta = m_getEntile(s->getstring("content"));
			CCPoint p = m_getViewc(ccp(s->getfloat("x"),s->getfloat("y")));
			b2Body* fia = ta->m_body;
			fia->SetTransform(b2Vec2((p.x+dtx)/PTM_RATIO,(p.y+dty)/PTM_RATIO),0);
			break;
		}
	case 1:				//控制的转移	-- 关闭控制可以在很多地方做到，必须根据脚本的特性做一个选择。
		{
			f_setcontroller(m_getEntile(s->getstring("content")));	//ITEMMANAGER设置控制器和镜头
			f_setcamara(m_getEntile(s->getstring("content")));		
			break;
		}
	case 2:				//光罩
		{
			mSpotLight::setRenderColor(ccc4f(s->getfloat("r"),s->getfloat("g"),s->getfloat("b"),s->getfloat("a")));
			break;
		}
	case 3:				//Follow,Attack,or something else . Controled by msp for better effect. Msp is now moved into entiles.
		{
			Entiles* ta = NULL;
			Entiles* tb = NULL;
			ta =  m_getEntile(s->getstring("a"));
			tb = m_getEntile(s->getstring("b"));
			//ta = ((Entiles*) fa->GetBody()->GetUserData());
			//tb = ((Entiles*) fb->GetBody()->GetUserData());			//IN:这个机制将会被msp取代

			ta->SCTarget(tb);
			break;
		}

	}
}


void TileMap::f_worldsearch(b2AABB* aabb){
	MyQueryCallback queryCallback;
	_world->QueryAABB( &queryCallback, *aabb );
	vector<b2Fixture*>::iterator iter;  
	for (iter= queryCallback.foundBodies.begin();iter!= queryCallback.foundBodies.end();iter++)  
	{  
		if((Entiles*) (*iter)->GetBody()->GetUserData() != m_controller)		//地图复杂时需要测量最近距离
		{	
			//TODO?: 处理调查类，添加传送功能到这里 --> ms已经交由sp处理
			b2Fixture* f = (*iter);
			Script* s = f_scrfind("_",getfname(f),0);
			if(s){
				fa = f;
				GameManager::sharedLogicCenter()->e_tmpscript(s->scriptnodes,s->getint("total"));
			}

		} //end of if
	} //end of for_iter
}

Script* TileMap::f_scrfind(const string na, const string nb, int t){
	if(na.size()<1 || nb.size()<1) return NULL;

	return f_scrfind(na+"_"+nb, t);
}

Script* TileMap::f_scrfind(const string gn, int t){
	Script* ret = (Script*) m_elist->objectForKey(gn);
	CCLOG(">Find for Script:%s",gn.c_str());

	if(ret)
	{
		ret = (Script*) ((CCDictionary*) ret->mapscpnodes)->objectForKey(t);
		if(ret){
			m_sNaScp = gn;
			m_iNaScp = t;
		}
		return ret;		//PB:可能会保存错误的内容
	}else 
		return NULL;
}

string TileMap::getfname(b2Fixture* f){
	if(f->IsSensor()){
		Entiles* e = (Entiles*) f->GetUserData();
		if(e) return e->name;
	}else{
		Entiles* e = (Entiles*) f->GetBody()->GetUserData();
		if(e) return	e->name;
	}
	return "";
}


TileMap::TileMap(const char* a, const char* b){
	map_path = a;
	scr_path = b;
	m_isLoad = false;
	lsp = NULL;
	init();
};

TileMap::TileMap(const char* a, const char* b, Script* asp){
	map_path = a;
	scr_path = b;
	m_isLoad = true;
	lsp = asp;
	init();
}

void TileMap::onEnter()
{
	CCLayer::onEnter();
	EventCenter::sharedEventCenter()->setController(this);

	// TIP: 2d projection should be used
	CCDirector::sharedDirector()->setProjection(kCCDirectorProjection2D);
	CCDirector::sharedDirector()->setDepthTest(true);
}

void TileMap::draw(){
#ifdef DEBUGDRAW
		
	_world->DrawDebugData();
	
#endif
}


void TileMap::ELoad(){
	for(int i = 0;i<m_ea->count();++i){
		Entiles* te = (Entiles*) m_ea->objectAtIndex(i);
		te->ELoad();
	}
	//CC_SAFE_RELEASE_NULL(m_ea);
}

bool TileMap::init()
{
	vx = 0;vy = 0;ax = 0;ay = 0;
	//////////////////////////////////////////////////////////////////////////
	// 1. super init first
	if ( !CCLayer::init() )
	{
		return false;
	}

	cancontrol = false;
	m_notuseditems = new CCArray();
	//////////////////////////////////////////////////////////////////////////
	// 2. TileMap初始化
	m_tilemap = CCTMXTiledMap::create(CCString::createWithFormat("map/%s.tmx",map_path.c_str())->getCString());
	addChild(m_tilemap, MapdDepth, kTagMap);
	CCSize s = m_tilemap->getContentSize();
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	m_tilemap->setAnchorPoint(ccp(0, 0));
	CCSize ms = m_tilemap->getMapSize();
	CCSize ts = m_tilemap->getTileSize();
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	m_tilemap->setPosition(ccp(0,0));

	
	sp = new Scriptor();
	sp->parse_file(CCString::createWithFormat("script/%s.xml",scr_path.c_str())->getCString());

	CCSpriteFrameCache *cache = CCSpriteFrameCache::sharedSpriteFrameCache();		//TODO:绘制将转交给Entile
	sheet = CCSpriteBatchNode::create("Images/test.png");
	cache->addSpriteFramesWithFile("Images/test.plist");



	//3. b2world初始化
	//////////////////////////////////////////////////////////////////////////
#if (WM_TYPE == MAP_TYPE_ISO )
	float r = (ms.width + ms.height) / 2;
	rw = r * ts.width;
	rh = r * ts.height;
	int mapWidth = m_tilemap->getMapSize().width * m_tilemap->getTileSize().width;
	int mapHeight = m_tilemap->getMapSize().height * m_tilemap->getTileSize().height;
	x0 = (ms.height - 1) * ts.width / 2;
	y0 = rh - ts.height;
	dtx = ts.width / 2;
	dty = ts.height / 2;
	dx = rw-mapWidth;
	dy = rh-mapHeight;
	x0 -= dx;
	y0 -= dy;
#else
	rw	= ms.width	* ts.width;
	rh	= ms.height	* ts.height;
	x0	= 0;
	y0	= 0;
	dtx	= ts.width / 2;
	dty	= ts.height / 2;
	dx	= 0;
	dy	= 0;

#endif

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
#if (WM_TYPE == MAP_TYPE_ISO )
	vs[0].Set((ms.height * ts.width/2 -dx)/ PTM_RATIO, (rh - dy) / PTM_RATIO);
	vs[1].Set((rw - dx) / PTM_RATIO, (ms.height * ts.height/2 - dy) / PTM_RATIO);
	vs[2].Set((ms.width * ts.width/2 - dx) / PTM_RATIO, -dy/PTM_RATIO);
	vs[3].Set(-dx/PTM_RATIO, (ms.width * ts.height/2 - dy )/ PTM_RATIO);
#else
	vs[0].Set(0, 0);
	vs[1].Set(0, rh/PTM_RATIO);
	vs[2].Set(rw/PTM_RATIO, rh/PTM_RATIO);
	vs[3].Set(rw/PTM_RATIO, 0);

#endif
	chain.CreateLoop(vs, 4);

	Entiles* kted = new Entiles();
	kted->name = "edge";
	groundBody->SetUserData(kted);
	groundBody->CreateFixture(&sd);
	m_notuseditems->addObject(kted);
	//////////////////////////////////////////////////////////////////////////
	//
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

	//4. 载入Entile - SP
	//////////////////////////////////////////////////////////////////////////
	b2CircleShape circle; 

	b2BodyDef ballBodyDef;					//pc的默认碰撞body
	ballBodyDef.type = b2_dynamicBody;  	 

	circle.m_radius = 15.5/PTM_RATIO;  
	b2FixtureDef ballShapeDef;				//pc的碰撞定制器
	ballShapeDef.shape = &circle;  
	ballShapeDef.density = 1.0f;  
	ballShapeDef.friction = 1.0f;  
	ballShapeDef.restitution = 0.0f;  


	b2PolygonShape boxShape;
	boxShape.SetAsBox((ts.width-1)/( PTM_RATIO * 4),(ts.height-1)/( PTM_RATIO * 4));

	b2FixtureDef boxFixtureDef;				//地图Tile碰撞矩形
	boxFixtureDef.shape = &boxShape;
	boxFixtureDef.density = 1;
	boxFixtureDef.filter.categoryBits = 0x0002;
	boxFixtureDef.filter.maskBits = 0x0001;

	b2BodyDef collideRectDef;
	////


	b2FixtureDef ballFixtureDef;			//范围检测传感器
	ballFixtureDef.shape = &circle;
	ballFixtureDef.density = 0;
	ballFixtureDef.isSensor = true;
	ballFixtureDef.filter.categoryBits = 0x0004;
	//ballFixtureDef.filter.maskBits = ~0x0002;

	b2BodyDef sencerCircle;

	//// b2body准备工作
	//////////////////////////////////////////////////////////////////////////
	m_elist = sp->mapscps;

	m_itemlist = new CCDictionary();

	m_ea = new CCArray();
	ccColor4F c4f;
	if(m_isLoad){
		if(!lsp) {CCLOG(">MapLoad but no lsp!");return false;}
		CCArray* tca = lsp->scriptnodes;
		c4f = ccc4f(lsp->getfloat("r"),lsp->getfloat("g"),lsp->getfloat("b"),lsp->getfloat("a"));

		for(int i = 0;i<lsp->m_snum;i++){
			Script* mtca = (Script*) tca->objectAtIndex(i);
			Entiles* mte = new EPhsics();

			mte->initFiles(mtca->getstring("psz"));
			mte->group_mask = mtca->getint("gmid");
			mte->group_id = mtca->getint("giid");
			mte->state = mtca->getint("state");
			mte->direc = (MoveDirect) mtca->getint("direct");
			mte->stand = (MoveDirect) mtca->getint("stand");
			//mte->b_CanMove = (mte->group_id == 1); 
			CCPoint d = ccp(mtca->getfloat("x"),mtca->getfloat("y"));

			//mte->m_sprite->setPosition(ccp(d.x * PTM_RATIO - dtx,d.y * PTM_RATIO - dty));
			//mte->retain();
			mte->name = mtca->getstring("name");
			mte->group = mtca->getstring("group");
			mte->setAnchorPoint(ccp(0,0));
			mte->m_sTarget = mtca->getstring("target");
			mte->b_Re = true;


			ballBodyDef.position.Set(d.x, d.y);  
			_body = _world->CreateBody(&ballBodyDef);  
			_body->SetFixedRotation(true);
			_body->CreateFixture(&ballShapeDef); 

			mte->m_body = _body;
			_body->SetUserData(mte);								//生成碰撞体 TODO:通过脚本自定义碰撞体

			//////////////////////////////////////////////////////////////////////////
			//读取传感器
			CCArray* kca = mtca->scriptnodes;
			for(int k=0;k<mtca->m_snum;k++){
				Script* ktca = (Script*) kca->objectAtIndex(k);
				if(ktca->getint("sensor") == 0) continue;
				Entiles* kte = new Entiles();
				kte->name = ktca->getstring("name");

				circle.m_radius = ktca->getfloat("radius");
				ballFixtureDef.userData = kte;
				_body->CreateFixture(&ballFixtureDef);
				m_notuseditems->addObject(kte);

			}




			//////////////////////////////////////////////////////////////////////////

			m_itemlist->setObject(mte,CCString::createWithFormat("%s_%s",mtca->getstring("group"),mtca->getstring("name"))->getCString());
			m_ea->addObject(mte);
			//mte->m_sprite->retain();
			//sheet->
				addChild(mte,i);

		}	//end of for_i_lsp_num;



	}else{
		CCArray* tca = sp->initcs;
		for(int i = 0;i<sp->in;i++){
			Script* tmp = (Script*) tca->objectAtIndex(i);					//script pack
			//CCDictionary* dtmp = new CCDictionary();	
			CCArray* mca = tmp->scriptnodes;
			for(int j = 0;j<tmp->m_snum;j++){
				Script* mtca = (Script*) mca->objectAtIndex(j);				//single item script
				Entiles* mte = new EPhsics();

				mte->initFiles(mtca->getstring("file"));
				mte->group_mask = mtca->getint("mask");
				mte->group_id = mtca->getint("group");
				//mte->b_CanMove = (mte->group_id == 1); 
				CCPoint d = m_getViewc(ccp(mtca->getfloat("x"),mtca->getfloat("y")));

				//mte->m_sprite->setPosition(ccp(d.x,d.y));
				//mte->retain();
				mte->name = mtca->getstring("name");
				mte->group = tmp->getstring("name");
				mte->setAnchorPoint(ccp(0,0));
				mte->b_Re = true;
				mte->b_Dirty = true;



				ballBodyDef.position.Set((dtx+d.x)/PTM_RATIO, (d.y+dty)/PTM_RATIO);  
				_body = _world->CreateBody(&ballBodyDef);  
				_body->SetFixedRotation(true);
				_body->CreateFixture(&ballShapeDef); 

				mte->m_body = _body;
				_body->SetUserData(mte);								//生成碰撞体 TODO:通过脚本自定义碰撞体

				//////////////////////////////////////////////////////////////////////////
				//读取传感器
				CCArray* kca = mtca->scriptnodes;
				for(int k=0;k<mtca->m_snum;k++){
					Script* ktca = (Script*) kca->objectAtIndex(k);
					Entiles* kte = new Entiles();
					kte->name = ktca->getstring("name");

					circle.m_radius = ktca->getint("radio")/PTM_RATIO;
					ballFixtureDef.userData = kte;
					_body->CreateFixture(&ballFixtureDef);
					m_notuseditems->addObject(kte);

				}




				//////////////////////////////////////////////////////////////////////////

				m_itemlist->setObject(mte,CCString::createWithFormat("%s_%s",tmp->getstring("name"),mtca->getstring("name"))->getCString());
				//mte->scheduleUpdate();				//DEC: 统一遍历链表进行更新操作
				//m_tilemap->
				//mte->m_sprite->retain();
				m_ea->addObject(mte);
				//sheet->
					addChild(mte,j);
			}
			//m_itemlist->setObject(dtmp,tmp->getstring("name"));
		}

		if(m_itemlist->count() < 1) CC_SAFE_RELEASE_NULL(m_itemlist);
	}


	//5. 载入地图物品 - TMX
	//////////////////////////////////////////////////////////////////////////
	// Block:
	// 0 --	name radio
	// 1 -- (block only)


	CCTMXLayer* blocks = m_tilemap->layerNamed("Block");
	for(int i = 0;i<ms.width;i++)
		for(int j = 0;j<ms.height;j++)
		{
			unsigned int GID = blocks->tileGIDAt(ccp(i,j));		
			CCDictionary* dic;
			dic = m_tilemap->propertiesForGID(GID);
			if(dic){
				CCPoint d = m_getViewc(ccp(i,j));
				int b_type = dic->valueForKey("block")->intValue();
				switch(b_type){
				case 0:		//object with sence
					{
						string sFilez = dic->valueForKey("item")->getCString();
						circle.m_radius = dic->valueForKey("radio")->floatValue()/PTM_RATIO;			//检测半径


						if(sFilez.length() > 0)				//物体传感器		<-- TODO:更进一步的碰撞形体设置/或者可以考虑和已经初始化的物体进行连接
						{									//为什么Tile物体需要传感器？
							//collideRectDef.type = b2_dynamicBody;


						}else{								//静态传感器		<-- TODO:改造成雷达/目前不参与碰撞
							Entiles* e = new ETiles();
							e->name = dic->valueForKey("name")->getCString();

							sencerCircle.type = b2_staticBody;	
							sencerCircle.position.Set((d.x+dtx)/PTM_RATIO,(d.y+dty)/PTM_RATIO);
							b2Body* b = _world->CreateBody(&sencerCircle);
							ballFixtureDef.userData = e;
							b->CreateFixture(&ballFixtureDef);

							e->m_body = b;
							b->SetUserData(e);
							m_notuseditems->addObject(e);
						}
						break;
					}
				case 1:		//静态阻挡物 可破坏 注意销毁关联的b2body
					{

						//CCSprite* ms =  blocks->tileAt( ccp(i,j) );
						collideRectDef.type = b2_staticBody;
						collideRectDef.position.Set((d.x+dtx)/PTM_RATIO,(d.y+dty)/PTM_RATIO);
						b2Body* b = _world->CreateBody(&collideRectDef);				//TODO:圆形碰撞可能会掉入两个矩形之间
						b->CreateFixture(&boxFixtureDef);

						Entiles* t = new Entiles();
						t->name = "tile";
						t->m_body = b;
						b->SetUserData(t);
						m_notuseditems->addObject(t);

						break;
					}
				default:
					CCLOG("cannt reach here.");
					break;
				}


			}//end of if_dic


		}//end of for_ij


		//CCSize winSize = CCDirector::sharedDirector()->getVisibleSize();


		m_controller = NULL;

		if(m_isLoad){
			m_sNaCam = lsp->getstring("camera");
			m_sNaCon = lsp->getstring("control");
			f_setcontroller(m_getEntile(m_sNaCon.c_str()));
			f_setcamara(m_getEntile(m_sNaCam.c_str()));

			m_iNaScp = lsp->getint("iscript");
			m_sNaScp = lsp->getstring("sscript");

			mSpotLight::setRenderColor(c4f);
		}else{

			f_setcontroller(m_getEntile("pc","control"));	//ITEMMANAGER设置控制器和镜头
			f_setcamara(m_getEntile("pc","control"));
		}
		//spotLight->runAction(TFollow::create(m_controller->m_sprite, CCRectMake(-dx, -dy, rw, rh)));

		//CCSprite* p = CCSprite::createWithSpriteFrameName("grossinis_sister2.png");
		//p->setPosition(ccp(1200,200));
		//sheet->addChild(p);

		addChild(sheet);

		this->scheduleUpdate();
		update_b2world(0);
		return true;
}


void TileMap::menuCloseCallback(CCObject* pSender)
{
	CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}



void TileMap::clearcontrol(){
	cancontrol = false;
	vx = 0;
	vy = 0;
}

void TileMap::z_press(bool up){
	if(!cancontrol) return;
	b2AABB* aabb = m_controller->f_search();
	if(aabb) {
		f_worldsearch(aabb);
	}
}

void TileMap::d_press(bool up){
	if(!cancontrol) return;
	if(up) {vx = 0;ax = 0;}
	else {ax = 1;vx = 100;}
}

void TileMap::a_press(bool up){
	if(!cancontrol) return;
	if(up) {vx = 0;ax = 0;}
	else {ax = -1;vx = -100;}
}

void TileMap::w_press(bool up){
	if(!cancontrol) return;
	if(up) {vy = 0;ay = 0;}
	else {ay = 1;vy = 100;}
}

void TileMap::s_press(bool up){
	if(!cancontrol) return;
	if(up) {vy = 0;ay = 0;}
	else {ay = -1;vy = -100;}

}

CCPoint TileMap::m_checkPoint(CCPoint c){

	CCPoint a = m_getTilec(ccpAdd(ccpMult(c,PTM_RATIO),ccp(-dtx,-dty)));
//	CCLOG("coordinate:%f,%f.",a.x,a.y);

	return a;
}

CCPoint TileMap::m_getTilec(CCPoint c){	//坐标转换 --> iso
	CCSize ts = m_tilemap->getTileSize();
#if (WM_TYPE == MAP_TYPE_ISO )
	return ccp(( (c.x - x0) / ts.width) + ( (y0 - c.y) / ts.height), ( (x0 - c.x) / ts.width) + ( (y0 - c.y) / ts.height));
#else
	return ccp(c.x / ts.width,(rh - c.y - ts.height) /ts.height);
#endif	
	
}

CCPoint TileMap::m_getViewc(CCPoint c){	//iso tile --> screen base
	CCSize ts = m_tilemap->getTileSize();
	
#if (WM_TYPE == MAP_TYPE_ISO )		
	return ccp(x0 + ((c.x - c.y) * ts.width) / 2 , y0 - ((c.x + c.y) * ts.height) / 2);
#else		
	return ccp(c.x* ts.width ,rh - c.y * ts.height - ts.height);
#endif		

}

float TileMap::round(float r) {
	r += 0.5;
	return (r > 0.0) ? floor(r) : 0;
}

void TileMap::f_setcontroller(Entiles* e){
	if(!e) return;
	if(m_controller){
		m_controller->b_IsControler = false;
	}

	m_sNaCon = e->GetFullName();
	m_controller = e;
	e->b_IsControler = true;

}

void TileMap::f_setcamara(Entiles* e){
	if(!e) return;
	m_sNaCam = e->GetFullName();
	this->runAction(TFollow::create(e, CCRectMake(-dx, -dy, rw, rh)));
}

Entiles* TileMap::m_getEntile(const char* group,const char* name){
	if(m_itemlist)
		return (Entiles*) ((CCDictionary*) m_itemlist->objectForKey(CCString::createWithFormat("%s_%s",group,name)->getCString()));
	else return NULL;
}

Entiles* TileMap::m_getEntile(const char* gn){
	if(m_itemlist)
		return (Entiles*) ((CCDictionary*) m_itemlist->objectForKey(gn));
	else return NULL;
}

void TileMap::update_b2world(float dt){
	_world->Step(dt, dt * 600, 7);					// [Change since M4.0.3] 防止不同配置引起的帧率起伏
	for(b2Body *b = _world->GetBodyList(); b; b=b->GetNext()) {				//TODO:当地图太大时引入区域查询(region queries)
		if (b->GetUserData() != NULL) {  
			Entiles *ballData = (Entiles *)b->GetUserData();  

			if(ballData->b_Dirty){
				ballData->update(dt);
				ballData->setPosition(ccp(b->GetPosition().x * PTM_RATIO- dtx,  
					b->GetPosition().y * PTM_RATIO- dty));  

				CCPoint p = ballData->getPosition();
				p = CC_POINT_POINTS_TO_PIXELS(p);
				float newZ = -(p.y+ 32+ dy) /16;
				ballData->setVertexZ( newZ );			//IN:使用set或者更大的碰撞矩形来避免同z同vertex的bug

				if(ballData->b_Re) //sheet->
					reorderChild(ballData,ceil(-p.y));	//IM:重排子次序
				//CCLOG("newz:%f",newZ);

				ballData->lin();
			}


			//ballData->setRotation( -1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));		

		}   
	}  
}

void TileMap::update_collide(){
	vector<CollideInfo*>::iterator iter;  
	fa = NULL;
	fb = NULL;
	for (iter= colse->bc.begin();iter!= colse->bc.end();iter++)  
	{  
		b2Fixture* t_f1 = (*iter)->f1;
		b2Fixture* t_f2 = (*iter)->f2;
		//if(t_f1->GetType());

		string s1 = getfname((*iter)->f1);
		string s2 = getfname((*iter)->f2);
		int type = (*iter)->type;
		CCLOG("Preparing Events:%s,%s,%d.",s1.c_str(),s2.c_str(),type);
		bool ret = true;
		do 
		{
			
			b2Fixture* t_f1 = (*iter)->f1;
			b2Fixture* t_f2 = (*iter)->f2;
			Entiles* t1 = (Entiles*) t_f1->GetBody()->GetUserData();
			Entiles* t2 = (Entiles*) t_f2->GetBody()->GetUserData();
			if(t_f1->IsSensor()){
				ret &= t1->SCContact(t_f1,t_f2,type == 2);
				ret &= t2->SCContacted(t_f2,t_f1,type == 2);
				break;
			}
			if(t_f2->IsSensor()){
				ret &= t2->SCContact(t_f2,t_f1,type == 2);
				ret &= t1->SCContacted(t_f1,t_f2,type == 2);
				break;
			}

			ret &= t1->SCTouch(t_f1,t_f2,type == 2);
			if(!ret) break;
			ret &= t2->SCTouch(t_f2,t_f1,type == 2);

		} while (0);

		if(!ret) break;						//SEL: 实体删除策略 - 1
	} 
	for (iter= colse->bc.begin();iter!= colse->bc.end();iter++)  
	{
		(*iter)->release();
	}
	colse->bc.clear();
}


void TileMap::update(float dt)
{

	if(m_controller){

		if(vx>0) m_controller->direc = MS_RIGHT;
		else if(vx<0) m_controller->direc = MS_LEFT;
		else if(vy>0) m_controller->direc = MS_UP;
		else if(vy<0) m_controller->direc = MS_DOWN; //test only , modify it in envent ;
		else m_controller->direc = MS_STOP;
	}

	if(cancontrol){
		update_collide();
		update_b2world(dt);
	}



	if (m_controller)
	{
		CCPoint p = m_controller->getPosition();

		p = this->convertToWorldSpace(p);
		//p = CCDirector::sharedDirector()->convertToUI(worldPoint);
		
		//p = CC_POINT_POINTS_TO_PIXELS(p);
		mSpotLight::setRenderRect(p.x,p.y);
	}


}

void TileMap::b_click()
{
	if(!cancontrol || !CharaS::sharedCharaS()->getdispchara()) return;
	GameManager::sharedLogicCenter()->ml->switch_to_battle("ortbattle");
}

void TileMap::test_disable_all_entiles()
{
	CCDictElement* pCde = NULL;
	CCDICT_FOREACH(m_itemlist,pCde){
		Entiles* t_e = (Entiles*) pCde->getObject();
		t_e->setVisible(false);
	}
	m_controller = NULL;

	renderLayer->removeFromParent();

	CC_SAFE_RELEASE_NULL(renderLayer);  

}

void TileMap::HandleScriptor( Scriptor* asp )
{
	CCLOG(">[TileMap]Super Class doesnot hanl");
}
