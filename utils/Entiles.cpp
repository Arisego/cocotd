#include "Entiles.h"
#include "GameManager.h"

Entiles::~Entiles()
{
	//m_sprite->removeFromParent();
	//m_sprite->release();
}

Entiles::Entiles()
{
	autorelease();
	name.clear();
	m_sprite = new CCSprite();

	group_id = 0;
	group_mask = 0;

	//test only
	b_CanMove = true;
	direc = MS_STOP;
	stand = MS_LEFT;
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
	CC_SAFE_RELEASE_NULL(m_sprite);
	m_sprite = CCSprite::createWithSpriteFrameName("blank.png");
	m_sprite->setAnchorPoint(ccp(0.5,0.5));
	m_sprite->setPosition(CCPointZero);

	m_sprite->setOpacity(0);

	//CCSpriteFrameCache *cache = CCSpriteFrameCache::sharedSpriteFrameCache();
	//CCSpriteBatchNode *sheet = CCSpriteBatchNode::create("sprite/gongbin.png");
	//cache->addSpriteFramesWithFile("sprite/gongbin.plist");
	//SplashLayer->addChild(sheet);
	//CCSpriterX *animator;
	//animator = CCSpriterX::create("sprite/gongbin.SCML");

	CCSpriteFrameCache *cache = CCSpriteFrameCache::sharedSpriteFrameCache();
	CCSpriteBatchNode *sheet = CCSpriteBatchNode::create(CCString::createWithFormat("%s.png",pszFileName)->getCString());
	cache->addSpriteFramesWithFile(CCString::createWithFormat("%s.plist",pszFileName)->getCString());

	m_animator = CCSpriterX::create(CCString::createWithFormat("%s.SCML",pszFileName)->getCString());


	m_animator->setAnchorPoint(ccp(0.5,0.5));
	m_animator->setPosition(ccp(16,155));
//	animator->setScale(0.8f);
//	animator->PlayAnim("Idle");

	sheet->addChild(m_animator);

	m_sprite->addChild(sheet);

	addChild(m_sprite);
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

void Entiles::setState( int ai )
{
	state = ai;
}

void Entiles::playAnimate( string name,int times )
{
	//[0803]CCLog(">[ET]The given direct:%d",direc);
	m_animator->PlayAnim(CCString::createWithFormat("%s_%s",name.c_str(),sDirect[direc])->getCString(),times);
}

void Entiles::ChangeFace( const CCPoint ac ){
	;
}

void Entiles::ShowDamage()
{
	CCLabelBMFont* c_ttlbmf;
	if(miAvgFlag == 0){
		c_ttlbmf = CCLabelBMFont::create(CCString::createWithFormat("-%d",miDamage)->getCString(),FNT_CHN);
		runAction(CCBlink::create(0.5,2));
	}else{
		c_ttlbmf = CCLabelBMFont::create(CCString::createWithFormat("MISS")->getCString(),FNT_CHN);
	}
	c_ttlbmf->setColor(ccRED);
	c_ttlbmf->setAnchorPoint(CCPointZero);
	c_ttlbmf->setVertexZ(getVertexZ());
	c_ttlbmf->setPosition(CCPointZero);

	addChild(c_ttlbmf,11);

	CCActionInterval* t_cai = CCSpawn::createWithTwoActions(CCMoveTo::create(0.7,ccpAdd(c_ttlbmf->getPosition(),ccp(0,100))),CCFadeOut::create(0.7));
	c_ttlbmf->runAction(t_cai);
}
