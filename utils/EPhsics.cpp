#include "EPhsics.h"

#include "GameManager.h"
#include "GameScene.h"

#define SLOW_ZONE 0.068

void EPhsics::ELoad(){
	if(state == 1) 
		SCTarget(GameManager::sharedLogicCenter()->ml->tm->m_getEntile(m_sTarget.c_str()));
}

void EPhsics::lin(){
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

				break;
			}
		case 1:
			{
				CCPoint cur = GameManager::sharedLogicCenter()->ml->tm->m_checkPoint(ccp(m_body->GetPosition().x,m_body->GetPosition().y));
				DecideDirect(cur,desiredVel);
				break;
			}
		}

		if(direc != MS_STOP) stand = direc;

		float velChangeX = desiredVel.x - vel.x;
		float velChangeY = desiredVel.y - vel.y;
		float impulseX = m_body->GetMass() * velChangeX; //disregard time factor
		float impulseY = m_body->GetMass() * velChangeY;
		m_body->ApplyLinearImpulse( b2Vec2(impulseX,impulseY), m_body->GetWorldCenter() );


		b_Dirty = true;
	}

}

b2AABB* EPhsics::f_search()
{
	b2AABB* ab = new b2AABB();
	float x,y;
	b2Vec2 p = m_body->GetPosition();
	b_C = !b_C;

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

void EPhsics::SCMoveto(CCPoint c){
	m_bMoving = true;
	tar = c;
	state = 1;
}

void EPhsics::SCGoto(CCPoint c){
	SCMoveto(c);			//当前没有AI
}

void EPhsics::SCTarget(SimControl* tar){		//不会更新目标的位置，如果有这种需求可以使用schedule来动态调整目标
	SCGoto(GameManager::sharedLogicCenter()->ml->tm->m_checkPoint(tar->SCGetPosition()));
	//m_sTarget = ((Entiles*) tar)->group + "_" + ((Entiles*) tar)->name;
	m_sTarget = ((Entiles*) tar)->GetFullName();
}

void EPhsics::DecideDirect(CCPoint cur,b2Vec2 &bv){
#if (WM_TYPE == MAP_TYPE_ISO)
	float fac;
	//[FOLLWOINGDEBUG]CCLOG(">decide:%f,%f,%f,%f.",cur.x,cur.y,tar.x,tar.y);
	if(abs(cur.x - tar.x)<0.01){
		if(cur.y > tar.y){
			direc = MS_UP;
			fac = cur.y - tar.y ;

			//[FOLLWOINGDEBUG]CCLOG("up:%f",fac);
			if(fac < SLOW_ZONE){
				bv = b2Vec2(4 * fac,2 * fac);
			}else{
				bv = b2Vec2(4,2);
			}

		}else if(cur.y < tar.y){
			direc = MS_DOWN;		
			fac = tar.y - cur.y ;
			//[FOLLWOINGDEBUG]CCLOG("down:%f",fac);
			if(fac < SLOW_ZONE){
				bv = b2Vec2(-4 * fac, -2 * fac);
			}else{
				bv = b2Vec2(-4,-2);
			}
		}

		if(fac<0.01){
			bv = b2Vec2(0,0);
			direc = MS_STOP;
			m_bMoving = false;
		}


	}else{
		if(cur.x > tar.x){
			direc = MS_UP;
			fac = cur.x - tar.x ;

			//[FOLLWOINGDEBUG]CCLOG("left:%f",fac);
			if(fac < SLOW_ZONE){
				bv = b2Vec2(-4 * fac,2 * fac);
			}else{
				bv = b2Vec2(-4,2);
			}

		}else if(cur.x < tar.x){
			direc = MS_RIGHT;
			fac = tar.x - cur.x;

			//[FOLLWOINGDEBUG]CCLOG("right:%f",fac);
			if(fac < SLOW_ZONE){
				bv = b2Vec2(4 * fac, -2 * fac);
			}else{
				bv = b2Vec2(4,-2);
			}


		}
	}

#else
	float fac;
	//[FOLLWOINGDEBUG]CCLOG(">decide:%f,%f,%f,%f.",cur.x,cur.y,tar.x,tar.y);
	if(abs(cur.x - tar.x)<0.01){
		if(cur.y < tar.y){
			direc = MS_UP;
			fac = tar.y - cur.y;

			//[FOLLWOINGDEBUG]CCLOG("up:%f",fac);
			if(fac < SLOW_ZONE){
				bv = b2Vec2(0,4 * fac);
			}else{
				bv = b2Vec2(0,4);
			}

		}else if(cur.y > tar.y){
			direc = MS_DOWN;		
			fac =cur.y - tar.y;
			//[FOLLWOINGDEBUG]CCLOG("down:%f",fac);
			if(fac < SLOW_ZONE){
				bv = b2Vec2(0, -4 * fac);
			}else{
				bv = b2Vec2(0,-4);
			}
		}

		if(fac<0.01){
			bv = b2Vec2(0,0);
			direc = MS_STOP;
			m_bMoving = false;
		}


	}else{
		if(cur.x > tar.x){
			direc = MS_UP;
			fac = cur.x - tar.x ;

			//[FOLLWOINGDEBUG]CCLOG("left:%f",fac);
			if(fac < SLOW_ZONE){
				bv = b2Vec2(-4 * fac,0);
			}else{
				bv = b2Vec2(-4,0);
			}

		}else if(cur.x < tar.x){
			direc = MS_RIGHT;
			fac = tar.x - cur.x;

			//[FOLLWOINGDEBUG]CCLOG("right:%f",fac);
			if(fac < SLOW_ZONE){
				bv = b2Vec2(4 * fac, 0);
			}else{
				bv = b2Vec2(4,0);
			}


		}
	}
#endif
}

bool EPhsics::SCContact(b2Fixture* self, b2Fixture* tar,bool bLeave){			//如果需要在这里维护Contact list.
	Entiles* tse = (Entiles*) self->GetUserData();
	Entiles* tae = (Entiles*) tar->GetBody()->GetUserData();
	if(!tse || !tae) return false;

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

bool EPhsics::SCContacted(b2Fixture* self,b2Fixture* tar,bool bLeave){
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

void EPhsics::StopFollow(){
	direc = MS_STOP;
	m_sTarget.clear();
	state = 0;
	CCLOG(">Follow Stop.");
}

bool EPhsics::SCTouch(b2Fixture* self, b2Fixture* tar,bool bLeave){
	CC_UNUSED_PARAM(self);					//备用
	Entiles* tar_b = (Entiles*) tar->GetBody()->GetUserData();

	if(tar_b->GetFullName() == m_sTarget) StopFollow();

	Script* sp = NULL;
	if(!bLeave) sp = GameManager::sharedLogicCenter()->ml->tm->f_scrfind(name,tar_b->name,1);
	else sp = GameManager::sharedLogicCenter()->ml->tm->f_scrfind(name,tar_b->name,2);
	if(sp) {
		GameManager::sharedLogicCenter()->ml->tm->fa = self;
		GameManager::sharedLogicCenter()->ml->tm->fb = tar;
		GameManager::sharedLogicCenter()->e_tmpscript(sp->scriptnodes,sp->getint("total"));
	}
	return true;
}