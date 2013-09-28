#include "EChess.h"

#include "GameManager.h"
#include "GameScene.h"

#define SLOW_ZONE 0

//void EChesses::initFiles(const char *pszFileName, const CCRect& rect)
//{
//	m_sprite->initWithFile(pszFileName, rect);
//}
//
//void EChesses::initFiles(const char *pszFileName)
//{
//	CC_SAFE_RELEASE_NULL(m_sprite);
//	m_sprite = CCSprite::createWithSpriteFrameName(pszFileName);
//	m_sprite->retain();
//	psz = pszFileName;
//}


void EChesses::ELoad(){
	if(state == 1) 
		SCTarget(GameManager::sharedLogicCenter()->ml->tm->m_getEntile(m_sTarget.c_str()));
}

void EChesses::lin(){
	Entiles::lin();
	if(b_CanMove){
		b2Vec2 vel = m_body->GetLinearVelocity();
		
		b2Vec2 desiredVel = b2Vec2(0,0);

		switch(state){
		case 0:
			{
				switch ( direc )
				{
#if (WM_TYPE == MAP_TYPE_ISO)
				case MS_LEFT:  desiredVel = b2Vec2(-4,2); break;
				case MS_STOP:  desiredVel =  b2Vec2(0,0); break;
				case MS_RIGHT: desiredVel =  b2Vec2(4,-2); break;
				case MS_UP: desiredVel = b2Vec2(4,2); break;
				case MS_DOWN: desiredVel = b2Vec2(-4,-2); break;
#else
				case MS_LEFT:  desiredVel = b2Vec2(-4,0); break;
				case MS_STOP:  desiredVel =  b2Vec2(0,0); break;
				case MS_RIGHT: desiredVel =  b2Vec2(4,0); break;
				case MS_UP: desiredVel = b2Vec2(0,4); break;
				case MS_DOWN: desiredVel = b2Vec2(0,-4); break;
#endif
				}
				//////////////////////////////////////////////////////////////////////////
				if(direc != MS_STOP){
					stand = direc;
					m_animator->PlayAnim(CCString::createWithFormat("paodong_%s",sDirect[direc])->getCString());
				}else{
					m_animator->PlayAnim(CCString::createWithFormat("stand_%s",sDirect[stand])->getCString());
				}
				break;
			}
		case 1:
			{
				CCPoint cur = GameManager::sharedLogicCenter()->ml->tm->m_checkPoint(ccp(m_body->GetPosition().x,m_body->GetPosition().y));
				DecideDirect(cur,desiredVel);
				//////////////////////////////////////////////////////////////////////////
				if(direc != MS_STOP){
					stand = direc;
					m_animator->PlayAnim(CCString::createWithFormat("paodong_%s",sDirect[direc])->getCString());
				}else{
					m_animator->PlayAnim(CCString::createWithFormat("stand_%s",sDirect[stand])->getCString());
				}
				break;
			}
		}



		float velChangeX = desiredVel.x - vel.x;
		float velChangeY = desiredVel.y - vel.y;
		float impulseX = m_body->GetMass() * velChangeX; //disregard time factor
		float impulseY = m_body->GetMass() * velChangeY;
		m_body->ApplyLinearImpulse( b2Vec2(impulseX,impulseY), m_body->GetWorldCenter() );


		b_Dirty = true;
	}

}

b2AABB* EChesses::f_search()
{
	b2AABB* ab = new b2AABB();
	float x,y;
	b2Vec2 p = m_body->GetPosition();
//	b_C = !b_C;

	x = p.x;
	y = p.y;

	switch(stand){
	case MS_LEFT: ab->lowerBound = b2Vec2(x - 1.5*S_WID,y-S_HEI); ab->upperBound = b2Vec2(x - S_WID,y+S_HEI); break;
	case MS_RIGHT:ab->lowerBound = b2Vec2(x + S_WID,y-S_HEI); ab->upperBound = b2Vec2(x +1.5*S_WID,y+S_HEI);  break;
	case MS_DOWN: ab->lowerBound = b2Vec2(x - S_WID,y- 1.5 * S_HEI); ab->upperBound = b2Vec2(x+S_HEI,y-S_HEI); break;
	case MS_UP: ab->lowerBound =  b2Vec2(x - S_WID,y+S_HEI ); ab->upperBound = b2Vec2(x+S_WID,y+1.5*S_HEI); break;	
	}
	return ab;
}

void EChesses::RefreshMovingState(CCPoint c){
	state = 1;

	CCPoint cur = GameManager::sharedLogicCenter()->ml->tm->m_checkPoint(ccp(m_body->GetPosition().x,m_body->GetPosition().y));
	if(cur.y > tar.y) m_mdH = MS_UP;
	else if(cur.y < tar.y) m_mdH = MS_DOWN;
	else m_mdH = MS_STOP;

	if(cur.x > tar.x) m_mdV = MS_LEFT;
	else if(cur.x < tar.x) m_mdV = MS_RIGHT;
	else m_mdV = MS_STOP;
	
}

void EChesses::SCMoveto(CCPoint c){
	CCLOG(">Move to:%f,%f",c.x,c.y);
	m_bMoving = true;
	tar = c;
	RefreshMovingState(c);
}

void EChesses::SCGoto(CCPoint c){
	SCMoveto(c);			// <当前没有AI ?????????
}

void EChesses::SCTarget(SimControl* tar){		//不会更新目标的位置，如果有这种需求可以使用schedule来动态调整目标
	SCGoto(GameManager::sharedLogicCenter()->ml->tm->m_checkPoint(tar->SCGetPosition()));
	//m_sTarget = ((Entiles*) tar)->group + "_" + ((Entiles*) tar)->name;
	m_sTarget = ((Entiles*) tar)->GetFullName();
}

void EChesses::DecideDirect(CCPoint cur,b2Vec2 &bv){				//TODO: modify it to realize bullet following.
#if (WM_TYPE == MAP_TYPE_ISO)
	switch(m_mdH)
	{
	case(MS_DOWN):
		{
			if(cur.y > tar.y) 
			{
				bv = b2Vec2(0,0);
				m_mdH = MS_STOP;
				break;
			}else{
				bv = b2Vec2(-4,-2);
				direc = MS_DOWN;	
			}
			return;
		}
	case(MS_UP):
		{
			if(tar.y > cur.y){
				bv = b2Vec2(0,0);
				m_mdH = MS_STOP;
				break;
			}else{
				bv = b2Vec2(4,2);
				direc = MS_UP;
			}
			return;
		}
	}

	switch(m_mdV){
	case(MS_LEFT):
		{
			if(tar.x > cur.x){
				bv = b2Vec2(0,0);
				m_mdV = MS_STOP;
				break;
			}else{
				bv = b2Vec2(-4,2);
				direc = m_mdV;
			}
			return;
		}
	case(MS_RIGHT):
		{
			if(cur.x > tar.x){
				bv = b2Vec2(0,0);
				m_mdV = MS_STOP;
				break;
			}else{
				bv = b2Vec2(4,-2);
				direc = m_mdV;
			}
			return;
		}
	}

	bv = b2Vec2(0,0);
	direc = MS_STOP;
	state = -1;
#else
	switch(m_mdH)
	{
	case(MS_DOWN):
		{
			if(cur.y > tar.y) 
			{
				bv = b2Vec2(0,0);
				m_mdH = MS_STOP;
				break;
			}else{
				bv = VS_DOWN;
				direc = MS_DOWN;	
			}
			return;
		}
	case(MS_UP):
		{
			if(tar.y > cur.y){
				bv = b2Vec2(0,0);
				m_mdH = MS_STOP;
				break;
			}else{
				bv = VS_UP;
				direc = MS_UP;
			}
			return;
		}
	}

	switch(m_mdV){
	case(MS_LEFT):
		{
			if(tar.x > cur.x){
				bv = b2Vec2(0,0);
				m_mdV = MS_STOP;
				break;
			}else{
				bv = VS_LEFT;
				direc = m_mdV;
			}
			return;
		}
	case(MS_RIGHT):
		{
			if(cur.x > tar.x){
				bv = b2Vec2(0,0);
				m_mdV = MS_STOP;
				break;
			}else{
				bv = VS_RIGHT;
				direc = m_mdV;
			}
			return;
		}
	}

	bv = b2Vec2(0,0);
	direc = MS_STOP;
	state = -1;
#endif

}



bool EChesses::SCContact(b2Fixture* self, b2Fixture* tar,bool bLeave){			//如果需要在这里维护Contact list.
	Entiles* tse = (Entiles*) self->GetUserData();
	Entiles* tae = (Entiles*) tar->GetBody()->GetUserData();
	if(!tse || !tae) return false;

	m_sprite->removeFromParent();
	Script* sp = NULL;
	if(!bLeave) sp = GameManager::sharedLogicCenter()->ml->tm->f_scrfind(tse->name,tae->name,1);
	else sp = GameManager::sharedLogicCenter()->ml->tm->f_scrfind(tse->name,tae->name,2);
	if(sp) {
		GameManager::sharedLogicCenter()->ml->tm->fa = self;
		GameManager::sharedLogicCenter()->ml->tm->fb = tar;
		GameManager::sharedLogicCenter()->e_tmpscript(sp->scriptnodes,sp->getint("total"));
	}
	return true;
}

bool EChesses::SCContacted(b2Fixture* self,b2Fixture* tar,bool bLeave){
	Entiles* tar_s = (Entiles*) tar->GetUserData();
	if(!tar_s) return false;
	Script* sp = NULL;

	if(!bLeave) sp =  GameManager::sharedLogicCenter()->ml->tm->f_scrfind(name,tar_s->name,1);
	else sp =  GameManager::sharedLogicCenter()->ml->tm->f_scrfind(name,tar_s->name,2);

	if(sp) {
		GameManager::sharedLogicCenter()->ml->tm->fa = self;
		GameManager::sharedLogicCenter()->ml->tm->fb = tar;
		GameManager::sharedLogicCenter()->e_tmpscript(sp->scriptnodes,sp->getint("total"));
		
	}
	return true;
}

void EChesses::StopFollow(){
	direc = MS_STOP;
	m_sTarget.clear();
	state = 0;
	//CCLOG(">Follow Stop.");
}

bool EChesses::SCTouch(b2Fixture* self, b2Fixture* tar,bool bLeave){			//EChess do not handle touch.
	return true;
}

void EChesses::load_chara_dbsp( Script* sp )
{
	CC_SAFE_RELEASE_NULL(m_pChara);
	int i_id = sp->getint("cid");
	m_pChara = new Chara();
	m_pChara->m_iCharaID = i_id;

	m_pChara->autorelease();
	m_pChara->retain();

	for(int j = 0;j<sp->m_snum;++j){
		Script* t_cp = (Script*) sp->scriptnodes->objectAtIndex(j);
		int t_it = t_cp->getint("type");

		switch(t_it){

		case(0):
			{
				for(int k = 0; k<t_cp->m_snum;++k){
					Script* t_kcp = (Script*) t_cp->scriptnodes->objectAtIndex(k);
					m_pChara->m_iiAttrs.insert(pair<string,int>(t_kcp->getstring("name"),t_kcp->getint("value")));
				}
				break;
			}
		case(1):
			{
				for(int k = 0; k<t_cp->m_snum;++k){
					Script* t_kcp = (Script*) t_cp->scriptnodes->objectAtIndex(k);
					m_pChara->m_miiEquips.insert(pair<int,int>(t_kcp->getint("name"),t_kcp->getint("value")));
				}
				break;
			}
		case(2):
			{
				for(int k = 0; k<t_cp->m_snum;++k){
					Script* t_kcp = (Script*) t_cp->scriptnodes->objectAtIndex(k);
					m_pChara->m_viSkills.insert(make_pair(t_kcp->getint("lock"),t_kcp->getint("id")));						
				}
				break;
			}

		}

	}
}

EChesses::EChesses(){	
	m_pChara = NULL;
	state = 0;
	m_bMoving = false;
	EChessComp* ecp = new EChessComp();
	ecp->autorelease();
	addComponent(ecp);
	miHitGroup = 0;
	//scheduleUpdate();	
}

EChesses::~EChesses()
{
	CC_SAFE_RELEASE_NULL(m_pChara);
	CCLOG(">");
}

void EChesses::ChangeFace( const CCPoint ac )
{
	CCPoint cur = GameManager::sharedLogicCenter()->ml->tm->m_checkPoint(ccp(m_body->GetPosition().x,m_body->GetPosition().y));
	CCLog("ChageFace:%f,%f||%f,%f",cur.x,cur.y,ac.x,ac.y);
	float k = abs(cur.y - ac.y) - abs(cur.x - ac.x);

	if(k>0){
		if(cur.y > ac.y) direc = MS_UP;
		else if(cur.y < ac.y) direc = MS_DOWN;
	}else{
		if(cur.x > ac.x) direc = MS_LEFT;
		else if(cur.x < ac.x) direc = MS_RIGHT;
	}
	//[0803]CCLog("Direct Change To:%d",direc);

}

