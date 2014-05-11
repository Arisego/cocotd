#include "AIComponent.h"
#include "GameManager.h"

#include "SingleTon/SituJudge.h"
#include "utils/EChess.h"
#include "SingleTon/SkillJudge.h"
#include "SingleTon/BattleFiled.h"

#define AI_STATE_EMPTY		-1
#define AI_STATE_WAITING	0
#define AI_STATE_MOVING		98
#define AI_STATE_SKILLUSING 99

int AIComponent::miDebugCount = 0;

AIComponent::AIComponent():
	miState(AI_STATE_EMPTY),
	mbMoving(false)
{
	m_strName = "ai";
	mpChara = NULL;
	msSkillDis.clear();
	miSkillP = ccp(0,0);
}

AIComponent::~AIComponent()
{
	CCLog(">[AIComponent] ~Deco.");
	switch (miState)
	{
	case AI_STATE_SKILLUSING:
		{
			GameManager::sharedLogicCenter()->ml->Dissmiss_Arrows();
			CCLog(">[AIComponent] ReleaseControl manually.");
			releaseControl();
			break;
		}
	default:
		break;
	}
}

void AIComponent::giveControl()
{
	CCLog(">[AIComponent] Token get. Lock.");

	
	GameManager::sharedLogicCenter()->ml->AILock();
	if(!mpChara){	
		mpChara =((EChesses*) getOwner())->m_pChara;
	}
	if(!CheckXuDong()){
		GiveOut();
		return;
	}

	BattleField::sharedBattleField()->SetSrc((EChesses*) getOwner());

	SkillJudge::sharedSkillJudge()->JudgeSkill(this);					/* <判断下次使用技能 */
	CCLog("[AIComponent] giveControl() | JudgeSkill over.");
	SituJudge::sharedSituJudge()->JudgeMove((EChesses*) getOwner());	/* <判断最优移动点 || 注意结合两者 */
	CCLog("[AIComponent] giveControl() | JudgeMove over.");
}

void AIComponent::releaseControl()
{
	GameManager::sharedLogicCenter()->ml->releaseControl();
}

void AIComponent::setTargetPoint( int ax, int ay )
{
	mbMoving = true;
	miState = AI_STATE_MOVING;
	CCLog(">[AIComponent] setTargetPoint( int %d, int %d )", ax, ay);
	if((ax == ((EChesses*) getOwner())->pos.x) & (ay == ((EChesses*) getOwner())->pos.y)){
		CCLog(">[AIComponent] No need to move");
		MoveOver();
	}else{
		miAtx = ax;
		miAty = ay;

		CCLog(">[AIComponent] Ask BM to move our chess to %d,%d", ax, ay);		
		GameManager::sharedLogicCenter()->ml->bm->draw_moving_tile();
		GameManager::sharedLogicCenter()->ml->bm->move_control(ax,ay);
		
	}
}

void AIComponent::MoveOver()
{
	if(!mbMoving) return;
	miState = 123;
	mbMoving = !mbMoving;
	CCLog(">[AIComponent] MoveOver().Do some cleaning...");
	GameManager::sharedLogicCenter()->ml->bm->clean_allcs();
	UseSkill();
}

void AIComponent::UseSkill()
{
	
	CCLog(">[AIComponent] UseSkill() | Begin.");
	if(!CheckXuDong()){
		GiveOut();
		return;
	}

	CCLog(">[AIComponent] UseSkill() | CanAct");
	mpChara->setvalue("b_xudong",mpChara->getvalue("b_xudong")-1);
	bool ret = false;
	msSkillDis.clear();

	do 
	{
		
		ret = SkillJudge::sharedSkillJudge()->CheckSkill(this);
		
		if(ret) break;				// <返回假表示该技能无法使用，为真则找到可使用的技能
		msSkillDis.insert(miNextSkill);
		SkillJudge::sharedSkillJudge()->JudgeSkill(this);
		GameManager::sharedLogicCenter()->ml->bm->clean_allcs();
		if(miNextSkill<0) break;	// <未能找到有效的技能，则放弃寻找
	} while (true);
	
	CCLog(">[AIComponent] UseSkill() | Pass");
	if(!ret){
		GameManager::sharedLogicCenter()->ml->bm->clean_allcs();
		
		if(!SkillJudge::sharedSkillJudge()->CheckNorM(m_pOwner)){
			GameManager::sharedLogicCenter()->ml->bm->clean_allcs();
			SkillOver();
		}
		
		return;		// <没有找到可用的技能
	}
	
	BattleField::sharedBattleField()->setBattle(true);
	ActSkill();
}

void AIComponent::ActSkill()
{
	miState = AI_STATE_SKILLUSING;
	++miDebugCount;
	if(!BattleField::sharedBattleField()->mbIsInBattle) BattleField::sharedBattleField()->mbIsMagic = true;
	BattleField::sharedBattleField()->mbCurMagic = true;

	CCLog(">[AIComponent][DE:%d] ActSkill() |  | State:%d", miDebugCount, miState);
	
	GameManager::sharedLogicCenter()->ml->bm->clear_mcaTarget();
	GameManager::sharedLogicCenter()->ml->bm->clear_Arrange();
	GameManager::sharedLogicCenter()->ml->Dissmiss_Arrows();
	GameManager::sharedLogicCenter()->ml->bm->m_mou_cur = miSkillP;
	GameManager::sharedLogicCenter()->ml->bm->f_decide(miSkillP.x,miSkillP.y);

	CCLog(">[AIComponent] ActSkill() | P[%f,%f] - %d", miSkillP.x,miSkillP.y, miNextSkill);
	EffectControler::sharedEffectControler()->md_use_skill(GameManager::sharedLogicCenter()->ml,miNextSkill,mpChara);			// <[TODO]技能修改入口点，尝试让EC吐出所有的sp？

	CCLog(">[AIComponent] ActSkill() | Use Skill Over");
	GameManager::sharedLogicCenter()->ml->bm->set_bbattle(5);
	GameManager::sharedLogicCenter()->ml->bm->m_bAnimateOver = false;

	// <设定技能标志位 || 可接续的技能将会存储在script中
	mpChara->cur_sid = miNextSkill;
	mpChara->cur_shit = false;

	
	GameManager::sharedLogicCenter()->ml->bm->clean_allcs();
	// Link out to circle
	//BattleField::sharedBattleField()->ActionFac();
}

void AIComponent::SetNextSkill( int ais )
{
	miNextSkill = ais;
}

int AIComponent::getNextSkill()
{
	return miNextSkill;
}

void AIComponent::SkillOver()
{
	
	GameManager::sharedLogicCenter()->ml->Dissmiss_Arrows();
	CCLog(">[AIComponent] Skill is Over");
	GiveOut();
}

void AIComponent::ActOver()
{
	CCLog(">[AIComponent][DE:%d] ActOver() | State:%d", miDebugCount, miState);

	switch (miState)
	{
	case AI_STATE_SKILLUSING:
		SkillOver();
		break;
	default:
		break;
	}

	miState = AI_STATE_WAITING;
}

bool AIComponent::CheckXuDong()
{
	do 
	{
		if(mpChara->checkXuDong()) break;
		return true;
	} while (false);
	return false;
}

void AIComponent::GiveOut()
{
	CCLog(">[AIComponent] Token release. And goto unLock.");
	if(miState != AI_STATE_SKILLUSING){
		if(((EChesses*) m_pOwner)->m_pChara->checkXuDong())
			((EChesses*) m_pOwner)->setGrey(1);
	}
	releaseControl();
}

void AIComponent::setSkillTargetPoint( int tiX, int tiY )
{
	CCLog(">[AIComponent] New Skill Target Got: (%d,%d)", tiX, tiY);
	miSkillP = ccp(tiX,tiY);
}

void AIComponent::ActAttact(const char* aScp)
{
	CCLog(">[AIComponent] ActAttact() | Prepare for AI-NAttack.");

	if(!BattleField::sharedBattleField()->mbIsInBattle) BattleField::sharedBattleField()->mbIsMagic = false;
	BattleField::sharedBattleField()->mbCurMagic = false;
	
	GameManager::sharedLogicCenter()->ml->bm->m_mou_cur = miSkillP;
	GameManager::sharedLogicCenter()->ml->bm->miRangeType = 0;
	GameManager::sharedLogicCenter()->ml->bm->set_bbattle(8);

	GameManager::sharedLogicCenter()->ml->bm->f_decide(miSkillP.x,miSkillP.y);
	
	
	GameManager::sharedLogicCenter()->ml->BFsp->re_init();
	GameManager::sharedLogicCenter()->ml->BFsp->parse_string(aScp);

	GameManager::sharedLogicCenter()->ml->bm->HandleScriptor(GameManager::sharedLogicCenter()->ml->BFsp);

	GameManager::sharedLogicCenter()->ml->bm->set_bbattle(5);
	GameManager::sharedLogicCenter()->ml->bm->m_bAnimateOver = false;

	// <设定技能标志位 || 可接续的技能将会存储在script中
	if(mpChara){
		mpChara->cur_sid = miNextSkill;
		mpChara->cur_shit = false;
	}

	miState = AI_STATE_SKILLUSING;
	GameManager::sharedLogicCenter()->ml->bm->clean_allcs();
	GameManager::sharedLogicCenter()->ml->Dissmiss_Arrows();
}
