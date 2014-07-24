#include "EGroup.h"
#include "utils\EChess.h"
#include "battle\BBuffer.h"
#include "ccMacros.h"
#include "BattleFiled.h"
#include "GameManager.h"

//////////////////////////////////////////////////////////////////////////
// Init

EGroup *EGroup::mSharedEGroup = NULL;

EGroup *EGroup::sharedEGroup()
{
	if (mSharedEGroup == NULL)
	{
		mSharedEGroup = new EGroup();
		if (!mSharedEGroup || !mSharedEGroup->init())
		{
			CC_SAFE_DELETE(mSharedEGroup);
		}

	}

	return mSharedEGroup;

}


void EGroup::purgeSharedEGroup()
{
	CC_SAFE_DELETE(mSharedEGroup);

}

//////////////////////////////////////////////////////////////////////////

EGroup::EGroup()
{
	meSrc = NULL;
	meTar = NULL;
}

EGroup::~EGroup()
{

}

bool EGroup::init()
{
	do 
	{
		return true;
	} while (false);
	return false;
}
//////////////////////////////////////////////////////////////////////////
static const int iBossExp[] = {0, 2, 5, 8, 10};		// None C - S | 0 2 - 10
void EGroup::FactDamage(CCObject* aSrc, CCObject* aTar, int adamv, int atype, int mtype)
{
	CCLog(">[EGroup] FactDamage() || Calculate Damage...");

	if(mtype>=0){
		adamv -= ((EChesses*) aTar)->m_pChara->getFixValue("def");
	}else{
		adamv -= ((EChesses*) aTar)->m_pChara->getFixValue("rst");
	}

	miDame = max(0,adamv);
	//miDame = 9990; // <调试

	EChesses* tESrc = (EChesses*) aSrc;
	EChesses* tETar = (EChesses*) aTar;
	Chara* tCSrc = tESrc->m_pChara;
	Chara* tCTar = tETar->m_pChara;

	meSrc = tESrc;
	meTar = tETar;
	//////////////////////////////////////////////////////////////////////////
	// <通知被伤害判定
	tCTar->Trig(TR_BEHURT);
	tCSrc->Trig(TR_SRDAMA);

	//////////////////////////////////////////////////////////////////////////
	// <属性加成 - mtype = 属性

	//////////////////////////////////////////////////////////////////////////
	// <经验计算
	int tExp;
	CCLog(">[EGroup] FactDamage() | Exp.");
	
	// <攻击方经验
	tExp = 0;
	if(CheckGrp(aSrc)){		// <非玩家控制单位升级没有意义，故而暂时不提供经验值
		switch (atype)
		{
		case 0:	// <物理攻击
			{
				// <对方等级-我方等级+4+对方boss等级
				tExp = 4 + iBossExp[tCTar->getvalue("boss_class")] + tCTar->getvalue("lv") - tCSrc->getvalue("lv");
				tExp = max(tExp,1);
				break;
			}
		default:	// <默认： 魔法攻击
			{
				// <消耗的魂魄数(ExpHunp())+对方等级-我方等级+5+对方boss阶级（2,5,8,10）。最低为1。击中复数敌人的情况，累加，但是消耗的魂魄数只做一次计算。
				tExp = 5+ iBossExp[tCTar->getvalue("boss_class")] + tCTar->getvalue("lv") - tCSrc->getvalue("lv"); 
				tExp = max(tExp,1);
				break;
			}
			
		}
		tCSrc->miEXP += tExp;
		CCLog(">[EGroup] FactDamage() | --> SRC:%d",tExp);
	}

	// <被攻击方经验
	tExp = 1;
	if(CheckGrp(aTar)){
		tCTar->miEXP += tExp;
		CCLog(">[EGroup] FactDamage() | --> TAR:%d",tExp);
	}

	//////////////////////////////////////////////////////////////////////////
	// <最终结果
	CCLog(">[EGroup] FactDamage() | Damage:%d", miDame);

	tETar->miDamage	 =	miDame;
	tETar->miFianDamage += miDame;
	tETar->meRS = tESrc;

	//tETar->miAvgFlag = 1;		// <调试

	tESrc->m_pChara->setHit(tETar->m_pChara->getvalue("boss_class") + 1);
	if(tETar->miFianDamage > tETar->m_pChara->gethp() && tETar->miFianDamage>0) tESrc->m_pChara->mbToKill = true;
	tESrc->m_pChara->miToDamage = miDame;
	tESrc->m_pChara->mcTar = tETar->m_pChara;
	//tESrc->m_pChara->Trig(TR_DAMAGE);
}

bool EGroup::CheckGrp(CCObject* aTar)
{
	EChesses* teTar = ((EChesses*) aTar);
	return (teTar->group_id == 0x01);	/* <玩家的ID永远是0x01 */
}

void EGroup::ExpHunp(CCObject* aSrc, int aiV)
{
	if(!CheckGrp(aSrc)) return ;
	CCLog(">[EGruop] ExpHunp() | %d", aiV);
	((EChesses*) aSrc)->m_pChara->miEXP += aiV;
}

static const int iBossDeadExp[] = {0, 50, 70, 100, 120};		// None C - S | 0 50 - 120
void EGroup::ExpDead(CCObject* aSrc)
{
	CCLog(">[EGroup] ExpDead()");
	EChesses* tESrc = (EChesses*) aSrc;
	EChesses* tETar = tESrc->meRS;		// <伤害来源和经验获得者
	if(!CheckGrp(tETar)) return;		// <伤害来源不需要经验

	Chara*	  tCSrc = tESrc->m_pChara;
	Chara*	  tCTar = tETar->m_pChara;

	int tiBossClass = tCSrc->getvalue("boss_class");
	int tExp = 0;

	if( 0 == tiBossClass){
		// <击破敌兵时候的追加经验：（对方等级-我方等级+4）*5   最低为1。
		tExp = 4 + tCSrc->getvalue("lv") - tCTar->getvalue("lv");
		tExp *= 5;
		tExp = max(1,tExp);
	}else{
		// <击破boss时候的追加经验：【（敌方战斗力-我方战斗力），负数的情况除以2，但是不低于-30】+对方boss阶级（50，70，100，120）+【（对方等级-我方等级）*10，这里负数的情况取0】
		tExp += tCSrc->getZDL();
		tExp -= tCTar->getZDL();
		if(tExp<0) tExp /= 2;
		tExp = max(-30,tExp);

		tExp += iBossDeadExp[tiBossClass];

		tExp += max((tCSrc->getvalue("lv") - tCTar->getvalue("lv")),0)*10;
	}

	CCLog(">[EGroup] ExpDead() | --> %d", tExp);
	tCTar->miEXP += tExp;
}

bool EGroup::ValChange(Script* asp)
{
	float ar = asp->getfloat("rate");

	if(CCRANDOM_0_1() < ar) return false;
	Chara* tcSrc = ((EChesses*) meSrc)->m_pChara;
	Chara* tcTar = ((EChesses*) meTar)->m_pChara;

	string msk = asp->getstring("name");
	int	ti = asp->getint("value");

	if(ti == -1){
		ti = ((EChesses*) meTar)->miDamage;
	}

	CCLog(">[EGroup] ValChange() | %s,%d", msk.c_str(), ti);
	if(tcTar->getvalue(msk) > 0){
		tcTar->implyVal(msk, -ti);
		tcSrc->implyVal(msk, ti);
		tcTar->checkVals();
		tcSrc->checkVals();
	}

	GameManager::sharedLogicCenter()->ml->m_rsb->SetContent((EChesses*) meTar);
	BattleField::sharedBattleField()->RefreshStats();
	
	return true;
}

bool EGroup::AtkPlay(Script* asp, CCObject* aesrc, CCObject* aetar)
{
	CCLog("++++ AtkPlay");
	// <生成播放的单位
	EChesses* teSrc = (EChesses*) aesrc;
	if(!teSrc) teSrc = (EChesses*) meTar;
	EChesses* teTar = (EChesses*) aetar;
	if(!teTar) teTar = (EChesses*) meSrc;

	if(teSrc != BattleField::sharedBattleField()->meOrig){
		teTar = BattleField::sharedBattleField()->meOrig;
	}
	
	if(teTar == teSrc) {
		teTar =  (EChesses*) meTar;
	}

	if(!teTar) return false;

	teSrc->ChangeFace(teTar->pos);
	teTar->ChangeFace(teSrc->pos);	

	CCLog("[EGroup] AtkPlay() | DB Ready.");
	if(((EChessComp*) teSrc->getComponent("controller"))->TestRange(teTar, true)){
		BattleField::sharedBattleField()->Trigged();
		return true;
	}
	return false;
}

bool EGroup::AtkSkill(Script* asp, CCObject* aesrc /*= NULL*/, CCObject* aetar /*= NULL*/)
{
	CCLog("++++ AtkPlay");
	// <生成播放的单位
	EChesses* teSrc = (EChesses*) aesrc;
	if(!teSrc) teSrc = (EChesses*) meTar;
	EChesses* teTar = (EChesses*) aetar;
	if(!teTar) teTar = (EChesses*) meSrc;

	if(teSrc != BattleField::sharedBattleField()->meOrig){
		teTar = BattleField::sharedBattleField()->meOrig;
	}

	if(teTar == teSrc) {
		teTar =  (EChesses*) meTar;
	}

	return BattleField::sharedBattleField()->PUSkillFilt(teSrc, teTar, 1);
}

int EGroup::getRound()
{
	// <TEST | ... | 获得当前的回合数
	int ret = 0;
	ret = GameManager::sharedLogicCenter()->ml->m_iTurn / 2;
	return ret;
}

int EGroup::getTurn()
{
	return GameManager::sharedLogicCenter()->ml->m_iTurn;
}

bool EGroup::ManuAlrange(Script* asp, CCObject* aSrc, CCObject* aTar /*= NULL*/)
{
	bool ret = false;
	do 
	{
	} while (false);

	return ret;
}

bool EGroup::MakDeath(Script* asp)
{
	bool ret = false;
	do 
	{
		EChesses* tESrc = (EChesses*) meSrc;
		EChesses* tETar = (EChesses*) meTar;
		Chara* tCSrc = tESrc->m_pChara;
		Chara* tCTar = tETar->m_pChara;

		if(tESrc->miHitFlag){	// <发生致命暴击
			if(CCRANDOM_0_1() < asp->getfloat("rate2")) break;
		}else{
			if(CCRANDOM_0_1() < asp->getfloat("rate")) break;
		}
		
		miDame = (tCTar->gethp() + 1);
		ret = true;
	} while (false);
	return ret;
}

bool EGroup::IsEnemy(CCObject* aSrc, CCObject* aTar){
	bool ret = false;
	EChesses* teSrc = (EChesses*) aSrc;
	EChesses* teTar = (EChesses*) aTar;

	ret = teSrc->group_mask & teTar->group_id;
	return ret;
}