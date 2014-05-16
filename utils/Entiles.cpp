#include "Entiles.h"
#include "GameManager.h"
#include "SingleTon\BattleFiled.h"
#include "battle\BBuffer.h"
#include "SingleTon\EGroup.h"

Entiles::~Entiles()
{
	//m_sprite->removeFromParent();
	//m_sprite->release();
}

Entiles::Entiles()
{
	autorelease();
	name.clear();
	m_sprite = NULL;

	group_id = 0;
	group_mask = 0;

	//test only
	b_CanMove = true;
	direc = MS_STOP;
	stand = MS_LEFT;
	b_Dirty = true;
	b_IsControler = false;
	b_Re = false;

	miFianDamage = 0;
	miGreyState = 0;
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
	m_animator->setPosition(ccp(-48,92));
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

void Entiles::AnimateNoLastO(){
	m_animator->SetNoLast(false);
}

void Entiles::playAnimate( string name,int times, float afBegin /*= 0.0 */, float afEnd /*= -1.0*/)
{
	//[0803]
	CCLog(">[Entiles] The given direct:%d",direc);
	CCLog(">[Entiles] PlayAnimate(%s,%d)", name.c_str(), times);
	m_animator->PlayAnim(CCString::createWithFormat("%s_%s",name.c_str(),sDirect[direc])->getCString(),times, "", afBegin, afEnd);
	m_animator->SetNoLast(true);
}

void Entiles::playAnimateDi(string name,int times, float afBegin /*= 0.0*/, float afEnd /*= -1.0*/)
{
	CCLog(">[Entiles] The given direct:%d",direc);
	CCLog(">[Entiles] PlayAnimate(%s,%d)", name.c_str(), times);
	m_animator->PlayAnim(name.c_str(), times, "", afBegin, afEnd);
	m_animator->SetNoLast(true);
}

void Entiles::ChangeFace( const CCPoint ac ){
	;
}

void Entiles::ShowDamage()
{
	Chara* mpc = ((EChesses*) this)->m_pChara;

	mpc->Trig(TR_BEDAMA);

	//((EChessComp*) getComponent("controller"))->ELock();
	// <伤害的格挡判定
	while(miAvgFlag == 0 && !mbCanNotShield){
		bool tbGd = false;
		CCLog(">[Entiles] ShowDamage() | Hit and Judge. GD.");
		if(((EChessComp*) getComponent("controller"))->HaveAI())
		{
			CCLog(">[Entiles] ShowDamage() | AI Component.");
			tbGd = true; // <AI控制
		}else{
			CCLog(">[Entiles] ShowDamage() | PC Component.");
			tbGd = EventCenter::sharedEventCenter()->mbDefendLock;
		}

		if(!tbGd) break;
		if(!mpc->CanAct()) break;
		

		int tiGdVal = mpc->getvalue("b_gedang");

		BattleField::sharedBattleField()->ClearTrigFlag();
		mpc->Trig(TR_GEDANG);
		if(!BattleField::sharedBattleField()->mbTrigged){
			CCLog(">[Entiles] ShowDamage() | GD:%d", tiGdVal);
			if(tiGdVal<=0) break;
			mpc->setvalue("b_gedang" , max(0, tiGdVal - 1));
		}
	
		CCLog(">[Entiles] ShowDamage() | Show the GD.");
		int tiGd = 0 - mpc->getFixValue("def");
		miFianDamage += tiGd;
		

		playAnimate("gedang",1);
		
		AnimateNoLastO();

		break;
	}

	// <显示伤害
	miFianDamage = max(0, miFianDamage);
	CCLog(">[Entiles] ShowDamage() | %d", miFianDamage);
	stringstream tsShow;
	
	if(miAvgFlag == 0 || mbCanNotAvg){
		CCLog(">[Entiles] ShowDamage() | >>> SHOW");
		tsShow << "-"; 
		tsShow << miFianDamage;
		
		/*CCActionInterval*  action1 = CCTintBy::create(0.12, -255, -255, -255);
		CCActionInterval*  action2Back = action1->reverse();*/
		//runAction(CCSequence::createWithTwoActions(CCRever);

		CCActionInstant* action1 = CCCallFunc::create(this, callfunc_selector(Entiles::DamIn));
		CCActionInstant* action2Back = CCCallFunc::create(this, callfunc_selector(Entiles::DamOut));

		CCActionInterval* a1 = CCTintBy::create(0.,122,122,122);
		CCActionInterval* a2 = a1->reverse();

		/*((EChessComp*) getComponent("controller"))->ELock();*/
		m_animator->runAction(CCSequence::create(/*CCCallFunc::create(((EChessComp*) getComponent("controller")),callfunc_selector(EChessComp::ELock)), */
			//CCDelayTime::create(0.07),
			 action1, CCFadeTo::create(0.01,160),CCDelayTime::create(0.12), CCFadeTo::create(0.01,255),action2Back, 
			/*CCBlink::create(0.12,1),*/
			/*CCCallFunc::create(((EChessComp*) getComponent("controller")),callfunc_selector(EChessComp::EUnLock)),*/ NULL));

		mpc->sethp(mpc->gethp()-miFianDamage);
	
		mpc->checkVals();
		if(mpc->m_bIsDead){
			EGroup::sharedEGroup()->meLast = mpc;
			((EChesses*) this)->meRS->m_pChara->Trig(TR_DEATH);
		}
		((EChesses*) this)->meRS->m_pChara->Trig(TR_DAMAGE);
		((EChesses*) this)->m_pChara->miBeDamage = miFianDamage;	// <输入累计的伤害而不是最后一次
	}else{
		CCLog(">[Entiles] ShowDamage() | >>> MISS");
		tsShow << "MISS";
		((EChesses*) this)->m_pChara->miBeDamage  = 0;
	}

	ShowTextS(tsShow.str().c_str());

	miFianDamage = 0;
	miDamage = 0;

	CCLog(">[Entiles] ShowDamage() | All Over");
}

void Entiles::ShowTextS(const char* ast, ccColor3B ac /*= ccRED*/)
{
	if(strlen(ast) < 1) return;
	CCLog(">[Entiles] ShowTextS() :%s", ast);
	CCLabelTTF* c_ttlbmf;
	c_ttlbmf = CCLabelTTF::create(ast,FNT_UI_TTF, 20);

	addChild(c_ttlbmf,11);
	c_ttlbmf->setColor(ac);
	c_ttlbmf->setAnchorPoint(CCPointZero);
	c_ttlbmf->setPosition(ccp(0,40));

	/*((EChessComp*) getComponent("controller"))->ELock();*/
	CCActionInterval* t_cai = CCSequence::create(/*CCCallFunc::create(((EChessComp*) getComponent("controller")),callfunc_selector(EChessComp::ELock)), */
		/*CCDelayTime::create(4.9),*/
		CCFadeIn::create(0.15), 
		CCSpawn::createWithTwoActions( CCMoveBy::create(0.22, ccp(0,20)) , CCSequence::createWithTwoActions(CCDelayTime::create(0.06), CCFadeOut::create(0.25)))
		, CCRemoveSelf::create(), 
		/*CCCallFunc::create(((EChessComp*) getComponent("controller")),callfunc_selector(EChessComp::EUnLock)),*/ NULL);
	c_ttlbmf->runAction(t_cai);
}

void Entiles::ShowTestF(const char* ast, int tic)
{
	ShowTextS(ast, sColors[tic]);
}


void Entiles::PauseMe()
{
	m_animator->PauseMe();
}

void Entiles::ResuMe()
{
	m_animator->ResuMe();
}

void Entiles::FixPosition()
{
	//
}

void Entiles::setGrey(int ai)
{
	if(GameManager::sharedLogicCenter()->ml->m_iCurGroup != this->group_id) return;
	setGreys(ai);
}

void Entiles::setGreys(int ai)
{
	if(miGreyState == ai) return;
	miGreyState = ai;

	switch (miGreyState)
	{
	case 1:
		m_animator->runAction(CCTintBy::create(0,148,148,148));
		break;
	case 0:
		m_animator->runAction(CCTintBy::create(0,148,148,148)->reverse());
		break;
	default:
		break;
	}
}

void Entiles::DamOut()
{
	ccBlendFunc t_sBlendFunc;
	t_sBlendFunc.src = CC_BLEND_SRC;
	t_sBlendFunc.dst = CC_BLEND_DST;

	((CCSpriteBatchNode*) m_animator->getParent())->setBlendFunc(t_sBlendFunc);
}

void Entiles::DamIn()
{
	ccBlendFunc brightnessBlend;
	brightnessBlend.src = GL_SRC_ALPHA;
	brightnessBlend.dst = GL_ONE;

	((CCSpriteBatchNode*) m_animator->getParent())->setBlendFunc(brightnessBlend);
}






