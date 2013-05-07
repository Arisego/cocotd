#ifndef __ECHESS_H__
#define __ECHESS_H__

#include "utils/Entiles.h"
#include "packui/CharaS.h"


USING_NS_CC;

// The Entitle For Battle Map. 
// As BattleMap has limited functions, we only use EChesses as the main object.
// You may not need a bullet or some other thing, the only thing need by BattleMap is animation.
// [IN] EChesses modified from bullet.
class EChesses : public Entiles
{
public:
	Chara* m_pChara;

	CCPoint tar,pos;
	
	int m_iDisable;		//TODO:通过碰撞来决定禁用的移动方向。
	bool m_bMoving;		//moving

	MoveDirect m_mdV,m_mdH;

	void onDestroy(){
		//m_sprite->removeFromParent();
		m_body->GetWorld()->DestroyBody(m_body);
		
	}
	
	~EChesses();
	EChesses(){
		m_pChara = NULL;
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

	//////////////////////////////////////////////////////////////////////////
	void load_chara_dbsp(Script* sp);
	void RefreshMovingState(CCPoint c);				//Refresh all the state needed before moving.
};

#endif