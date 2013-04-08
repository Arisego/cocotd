#ifndef __EBULLET_H__
#define __EBULLET_H__

#include "utils/Entiles.h"


USING_NS_CC;
 
//Bullet is a class for bullet.
//It need to be enhanced due to the design.
class EBullet : public Entiles
{
public:
	bool b_C;			//test only . change the control mode.
	CCPoint tar;
	
	int m_iDisable;		//TODO:通过碰撞来决定禁用的移动方向。
	bool m_bMoving;		//moving

	~EBullet(){
		m_sprite->removeFromParent();
		m_body->GetWorld()->DestroyBody(m_body);
	}
	

	EBullet(){
		state = 0;
		m_bMoving = false;
	}


	void lin();
	b2AABB* f_search();
	void SCMoveto(CCPoint c);
	void DecideDirect(CCPoint cur, b2Vec2 &bv);
	virtual void SCGoto(CCPoint c);
	virtual void SCTarget(SimControl* tar);
	
	virtual bool SCContact(b2Fixture* self, b2Fixture* tar,bool bLeave = false);
	virtual bool SCContacted(b2Fixture* self,b2Fixture* tar,bool bLeave = false);
	virtual bool SCTouch(b2Fixture* self, b2Fixture* tar,bool bLeave = false);

	void StopFollow();

	virtual void ELoad();

	virtual void initFiles(const char *pszFileName, const CCRect& rect);			//Do not use this.
	virtual void initFiles(const char *pszFileName);

};

#endif