#include "Entiles.h"

Entiles::Entiles()
{
	name.clear();
	m_sprite = new CCSprite();

	group_id = 0;
	group_mask = 0;

	//test only
	b_CanMove = true;
	direc = MS_STOP;
	stand = MS_STOP;
	b_Dirty = true;
	b_IsControler = false;
	b_Re = false;
}

void Entiles::initFiles(const char *pszFileName, const CCRect& rect)
{
	m_sprite->initWithFile(pszFileName, rect);
}

void Entiles::initFiles(const char *pszFileName)
{
	m_sprite = CCSprite::createWithSpriteFrameName(pszFileName);
	m_sprite->setTag(9);
	psz = pszFileName;
}

void Entiles::lin()
{
	b_Dirty = false;
}

b2AABB*  Entiles::f_search()
{
	return NULL;
}

void Entiles::SCTarget(SimControl* tar){
	CCLOG("Setting Target!!!!");
}
void Entiles::DecidePath(SimControl* tar){
	CCLOG("Deciding Path!!!!");
}
bool Entiles::SCTouch(b2Fixture* self, b2Fixture* tar,bool bLeave){
	CCLOG("Attact Target!!!!");
	return true;
}

bool Entiles::SCAttacted(SimControl* src){
	CCLOG("Attacted BY!!!!");
	return true;
}
bool Entiles::SCSearched(SimControl* src){
	CCLOG("Searched . Talk or Give.");
	return true;
}
bool Entiles::SCContact(b2Fixture* self, b2Fixture* tar,bool bLeave){
	CCLOG("Contat. Follow or Attact.");
	return true;
}
bool Entiles::SCContacted(b2Fixture* self,b2Fixture* tar,bool bLeave){
	CCLOG("Contacted. Maybe Nothing.");
	return true;
}


void Entiles::SCGoto(CCPoint c){
};
void Entiles::SCMoveto(CCPoint c){
};

void Entiles::ELoad(){
};

CCPoint Entiles::SCGetPosition(){
	return ccp(m_body->GetPosition().x,m_body->GetPosition().y);
}

string Entiles::GetFullName(){			//存储Fullname，如果影响效率的话
	return CCString::createWithFormat("%s_%s",group.c_str(),name.c_str())->getCString();
}

