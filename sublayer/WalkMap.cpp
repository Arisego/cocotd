#include "WalkMap.h"
#include "utils/EBullet.h"

WalkMap::WalkMap(const char *a,const char* b){
	map_path = a;
	scr_path = b;
	m_isLoad = false;
	lsp = NULL;
	init();
}

WalkMap::WalkMap(const char* a, const char* b, Script* asp){
	map_path = a;
	scr_path = b;
	m_isLoad = true;
	lsp = asp;
	init();
}

bool WalkMap::init(){
	do 
	{
		CC_BREAK_IF(!TileMap::init());
		mt_EffectList = NULL;
		mt_iCount = 0;
		//CC_BREAK_IF(!show_hud());

		return true;
	} while (0);
	return false;
}

void WalkMap::show_text(string s)
{
	++mt_iCount;

	CCLabelBMFont* c_ttlbmf = CCLabelBMFont::create(s.c_str(),FNT_CHN);
	c_ttlbmf->setAnchorPoint(CCPointZero);
	c_ttlbmf->setVertexZ(m_controller->getVertexZ());
	c_ttlbmf->setPosition(m_controller->getPosition());
	c_ttlbmf->setZOrder(m_controller->getZOrder());
//	c_ttlbmf->setTag(0x299);
	addChild(c_ttlbmf,11);
//	mt_EffectList->addObject(c_ttlbmf);

	CCActionInterval* t_cai = CCSpawn::createWithTwoActions(CCMoveTo::create(0.3,ccpAdd(c_ttlbmf->getPosition(),ccp(0,100))),CCFadeOut::create(0.3));
	c_ttlbmf->runAction(CCSequence::createWithTwoActions(t_cai,CCCallFuncO::create(this,callfuncO_selector(WalkMap::effectback),c_ttlbmf)));
}

void WalkMap::effectback(CCObject* pSender)
{
	((CCNode*) pSender)->removeFromParent();
}


void WalkMap::addbullet(Script* mtca){										//bullet is a sensor. It only care whether an enemy is contacted.
	b2CircleShape circle; 
	b2BodyDef ballBodyDef;					//pc的默认碰撞body
	ballBodyDef.type = b2_dynamicBody;  

	circle.m_radius = mtca->getfloat("radius")/PTM_RATIO;  

	b2FixtureDef ballShapeDef;				//pc的碰撞定制器
	ballShapeDef.shape = &circle;  
	ballShapeDef.density = 1.0f;  
	ballShapeDef.friction = 1.0f;  
	ballShapeDef.restitution = 0.0f;  


	b2BodyDef sencerCircle;

	Entiles* mte = new EBullet();
	
	addChild(mte);
	mte->initFiles(mtca->getstring("psz"));				//动画来源
	
	mte->group_mask = mtca->getint("gmid");				//组掩码
	mte->group_id = mtca->getint("giid");				//组标记
	mte->state = mtca->getint("state");

	mte->direc = m_controller->stand;
	mte->stand = m_controller->stand;
	//mte->b_CanMove = (mte->group_id == 1); 
	CCPoint d = m_controller->getPosition();
	float ds = circle.m_radius + 16;
	switch(mte->direc){
	case(MS_LEFT):
		{
			d.x -= ds;
			d.y += ds * 0.5;
			break;
		}
	case(MS_RIGHT):
		{
			d.x += ds;
			d.y -= ds * 0.5;
			break;
		}
	case(MS_UP):
		{
			d.x += ds;
			d.y += ds * 0.5;
			break;
		}
	case(MS_DOWN):
		{
			d.x -= ds;
			d.y -= ds * 0.5;
			break;
		}
	default:
		{
			exit(0x5007); //We do not want a bullet which is stop...
			break;
		}
	}

	//mte->setPosition(d);
	//mte->retain();
	mte->name = mtca->getstring("name");				//Bullet name?
	mte->group = mtca->getstring("group");				//Bullet group?

	mte->setAnchorPoint(ccp(0,0));
	mte->m_sTarget = mtca->getstring("target");			//maybe needed?
	mte->b_Re = true;


	ballBodyDef.position.Set((d.x + dtx)/PTM_RATIO, (d.y + dty)/PTM_RATIO);  
	_body = _world->CreateBody(&ballBodyDef);  
	_body->SetFixedRotation(true);
	_body->CreateFixture(&ballShapeDef); 

	mte->m_body = _body;
	_body->SetUserData(mte);								

	//////////////////////////////////////////////////////////////////////////

	//m_itemlist->setObject(mte,CCString::createWithFormat("%s_%s",mtca->getstring("group"),mtca->getstring("name"))->getCString());
	//m_ea->addObject(mte);
	//mte->retain();
	//sheet->
		addChild(mte);
}