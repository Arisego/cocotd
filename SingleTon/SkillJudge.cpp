#include "SkillJudge.h"
#include "DBUtil.h"
#include "utils\EChess.h"
#include "GameManager.h"
#include "BattleFiled.h"
#include "EGroup.h"

//////////////////////////////////////////////////////////////////////////
// Init

SkillJudge *SkillJudge::mSharedSkillJudge = NULL;


bool SkillJudge::init()
{
	sp = new Scriptor();
	mbJudging = false;
	return true;
}

SkillJudge::~SkillJudge()
{
	CC_SAFE_DELETE(sp);
}

SkillJudge *SkillJudge::sharedSkillJudge()
{
	if (mSharedSkillJudge == NULL)
	{
		mSharedSkillJudge = new SkillJudge();
		if (!mSharedSkillJudge || !mSharedSkillJudge->init())
		{
			CC_SAFE_DELETE(mSharedSkillJudge);
		}

	}

	return mSharedSkillJudge;

}


void SkillJudge::purgeSharedSkillJudge()
{
	CC_SAFE_DELETE(mSharedSkillJudge);

}

//////////////////////////////////////////////////////////////////////////
// /* <功能函数 */

void SkillJudge::JudgeSkill( AIComponent* tai )
{
	mpchara = tai->mpChara;
	
	miJudSkillRes = -1;
	tai->SetNextSkill(miJudSkillRes);
	//////////////////////////////////////////////////////////////////////////

	msi_SkillSet.clear();
	if(mpchara->mSkillList->size() == 0){
		NoSkillFit();
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// /* <读取八个主动技能 */
	string t_sMask;
	for(int i = 0; i<8; ++i)
	{
		int t_id = mpchara->m_viSkills[i];
		if(t_id != 0 && tai->msSkillDis.count(t_id) == 0)
			msi_SkillSet.insert(t_id);
	}

	//////////////////////////////////////////////////////////////////////////
	// /* <筛选可用技能 */
	CCLOG(">[SJ] before skill filter...");
	FilterSkills();
	if(msi_SkillSet.size() == 0) {
		CCLOG(">[SJ] skill all filtered...");
		NoSkillFit();
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	miJudSkillRes = -1;
	FindSkill();
	CCLog(">[SJ_FS] FindSkill id:%d",miJudSkillRes);
	/* < miJudSkillRes为最终生成的 下次攻击技能 || 简化设计：攻击预判将使用这个技能 */
	tai->SetNextSkill(miJudSkillRes);
	CCLog(">[SJ_FS] FindSkill set over");
}

void SkillJudge::NoSkillFit()
{
	CCLog(">[SJ] No Skill Fit.");
}

void SkillJudge::FindSkill()
{

	int ti = -1;
	int tiMax = 0;
	for(set<int>::iterator it = msi_SkillSet.begin(); it != msi_SkillSet.end(); ++it){
		int tvs = ValueSkill(*it);
		if(tvs>tiMax)
		{
			tiMax = tvs;
			ti = *it;
		}	
	}
	miJudSkillRes = ti;

}


int SkillJudge::ValueSkill(int aid)
{
	BSkill* tbs = mpchara->mSkillList->getSkill(aid);
	CCLOG(">SkillJudgeValue:%s",tbs->msName.c_str());
	int ret = 0;
	int tidam = tbs->miDamage;
	ret = tidam;
	CCLOG("[SJ_VS] Skill valued:%d",ret);

	return ret;
}


void SkillJudge::FilterSkills()
{
	int ti;
	CCArray* t_caLocks;
	EffectControler::sharedEffectControler()->f_clear();
	EffectControler::sharedEffectControler()->m_src = mpchara;
	for(set<int>::iterator it = msi_SkillSet.begin(); it != msi_SkillSet.end(); ++it)
	{
		ti = *it;
		if(ti !=0 ){			/* <有技能存在的情况 */
			sp->re_init();
			
			BSkill* tbs = mpchara->mSkillList->getSkill(ti);
			sp->parse_string(tbs->msSideEffect.c_str());
			CCLog(">[SJ_FS] %s", tbs->msName.c_str());
			t_caLocks = sp->m_caScript;
			sp->m_caScript = NULL;

			EffectControler::sharedEffectControler()->m_caLocks = t_caLocks;
			
			if(!EffectControler::sharedEffectControler()->DerLock()) continue;

			msi_SkillSet.insert(ti);
		}
	}

}

bool SkillJudge::CheckSkill( AIComponent* tai )
{
	do 
	{
		mpchara = tai->mpChara;
		GameManager::sharedLogicCenter()->ml->m_iFuncType = 8;
		EChesses* tec = (EChesses*) tai->getOwner();
		BSkill* tbs = mpchara->mSkillList->getSkill(tai->getNextSkill());

		if(!tbs) break;
		if(!(tbs->miType & 1)) break;

		int tiUseCase = tbs->m_iUseCase;
		GameManager::sharedLogicCenter()->ml->m_iSUseCase = tiUseCase;		
		GameManager::sharedLogicCenter()->ml->bm->miRangeType = tiUseCase;

		GameManager::sharedLogicCenter()->ml->bm->clear_mcaTarget();
		GameManager::sharedLogicCenter()->ml->bm->clear_Arrange();
		
		//////////////////////////////////////////////////////////////////////////
		// <锁检测
		sp->re_init();
		sp->parse_string(tbs->msSideEffect);
		CCArray* t_caLocks = sp->m_caScript;
		sp->m_caScript = NULL;

		EffectControler::sharedEffectControler()->f_clear();
		EffectControler::sharedEffectControler()->m_caLocks = t_caLocks;
		EffectControler::sharedEffectControler()->m_src = mpchara;
		//t_caLocks->retain();
		
		if(!EffectControler::sharedEffectControler()->DerLock()) break;

		//////////////////////////////////////////////////////////////////////////	
		GameManager::sharedLogicCenter()->ml->bm->setLink(tbs->m_iLink);
		GameManager::sharedLogicCenter()->ml->BMSkDrawMove(tbs->msAddition);

		//////////////////////////////////////////////////////////////////////////
		int tiMax = 0;
		int tiX = 0;
		int tiY = 0;
		
		CCLog(">[SkllJudge]CheckSkill() || Name:%s |SkillName:%s", tec->m_pChara->m_sName.c_str(), tbs->msName.c_str());
		CCLog(">[SkillJudge][1225] -------------------------------------------------------------- Begins ----------------------------------------------------------------");
		if(GameManager::sharedLogicCenter()->ml->mbIsDirect){
			CCLog(">[SkllJudge] Buffer Like Direct Skill.");		// <暂时AI不要释放这种技能
			break;
		}else{
			for(set<pair<int,int>>::iterator it = GameManager::sharedLogicCenter()->ml->bm->cs_y.begin();it != GameManager::sharedLogicCenter()->ml->bm->cs_y.end(); ++it)
			{
				CCLog(">[SkillJudge][1225] TestPoint: (%d,%d).", it->first, it->second);
				if(it->first<1) continue;
				if(it->second<1) continue;

				GameManager::sharedLogicCenter()->ml->bm->f_decide(it->first, it->second);
				if(GameManager::sharedLogicCenter()->ml->bm->testLink()){
					CCArray* tca = GameManager::sharedLogicCenter()->ml->bm->m_caTarCharas;
					if(!tca) continue;
					CCObject* tco = NULL;
					int ta =  0;

					CCARRAY_FOREACH(tca,tco){
						if( GameManager::sharedLogicCenter()->ml->bm->ts_last.count(make_pair(((EChesses*) ((Chara*) tco)->mEcOwner)->pos.x, ((EChesses*) ((Chara*) tco)->mEcOwner)->pos.y)) == 0) continue;

						 if(EGroup::sharedEGroup()->IsEnemy(tec, ((Chara*) tco)->mEcOwner)) ta += (999 - ((Chara*) tco)->gethp());
						 else ta -= 10;		// <友军伤害惩罚
					}
					if(ta>tiMax){
						tiMax = ta;
						tiX = it->first;
						tiY = it->second;
					}

					//////////////////////////////////////////////////////////////////////////
				}
				GameManager::sharedLogicCenter()->ml->bm->clear_mcaTarget();
				GameManager::sharedLogicCenter()->ml->bm->clear_Arrange();
				//////////////////////////////////////////////////////////////////////////
			}


			CCLog(">[SkillJudge][1225] -------------------------------------------------------------- End ----------------------------------------------------------------");
			if(tiMax <= 0) break;
			CCLog(">[SkillJudge] Skill Find with value:%d || %d,%d", tiMax, tiX, tiY);
			tai->setSkillTargetPoint(tiX,tiY);
		}


		return true;
	} while (false);
	return false;
}

/************************************************************************/
/* 
	< - [ec, Null]	确立可攻击范围并寻找最佳的攻击单位 | 不成立则返回false
	< - [ec, ec]	查看是否可对第二个单位[位置]发起攻击，如可能则发起攻击
*/
/************************************************************************/
bool SkillJudge::CheckNorM(CCObject* tai, CCObject* tar, bool abact)	
{
	EChesses* tte = (EChesses*) tai;

	if(!tte->m_pChara->CanAct()) return false;
	
	mbJudging = true;

	do 
	{
		//////////////////////////////////////////////////////////////////////////
		GameManager::sharedLogicCenter()->ml->m_iFuncType = 16;
		mpchara = tte->m_pChara;
		
		int t_iType = mpchara->getvalue("attack");		// <[TODO]从单位中获得攻击属性,具体的来源需要根据设计进行变更，注意默认取得的是0

		GameManager::sharedLogicCenter()->ml->vdata.clear();
		DBUtil::initDB("save.db");
		CCString* t_csSql = CCString::createWithFormat("select * from attr_attact where id = %d",t_iType);
		CCLog("%s",t_csSql->getCString());
		GameManager::sharedLogicCenter()->ml->vdata = DBUtil::getDataInfo(t_csSql->getCString(),NULL);
		int m_number = GameManager::sharedLogicCenter()->ml->vdata.size();
		DBUtil::closeDB(); 

		map<string,string> t_ssm = (map<string,string>) GameManager::sharedLogicCenter()->ml->vdata.at(0);
		string tsAddition = t_ssm.at("additional");

		GameManager::sharedLogicCenter()->ml->bm->f_setcontroller(tte);
		GameManager::sharedLogicCenter()->ml->bm->clean_allcs();
		GameManager::sharedLogicCenter()->ml->BMAtDrawMove(tsAddition);

		BattleField::sharedBattleField()->setBattle(true);

		//////////////////////////////////////////////////////////////////////////
		int tiMax = 0;
		int tiX = 0;
		int tiY = 0;

		if(!tar){
			CCLog(">[SkllJudge]CheckNorM() || Type:%d", t_iType);
			if(GameManager::sharedLogicCenter()->ml->bm->cs_y.size() <= 0){
				break;
			}else
			{
				GameManager::sharedLogicCenter()->ml->bm->cs_y.erase(make_pair(tte->pos.x, tte->pos.y));
				CCLog(">[SkillJudge] CheckNorm() || cs_y.size == %d", GameManager::sharedLogicCenter()->ml->bm->cs_y.size());
				CCLog(">[SkillJudge]CheckNorM() -------------------------------------------------------------- Begins ----------------------------------------------------------------");

				for(set<pair<int,int>>::iterator it = GameManager::sharedLogicCenter()->ml->bm->cs_y.begin();it != GameManager::sharedLogicCenter()->ml->bm->cs_y.end(); ++it)
				{
					CCLog(">[SkillJudge][1225] TestPoint: (%d,%d).", it->first, it->second);
					if(it->first<1) continue;
					if(it->second<1) continue;

					GameManager::sharedLogicCenter()->ml->bm->clear_mcaTarget();
					GameManager::sharedLogicCenter()->ml->bm->clear_Arrange();
					GameManager::sharedLogicCenter()->ml->bm->miRangeType = 0;

					GameManager::sharedLogicCenter()->ml->bm->f_decide(it->first, it->second);
					if(GameManager::sharedLogicCenter()->ml->bm->arange_targetwithtest(0)){
						CCArray* tca = GameManager::sharedLogicCenter()->ml->bm->m_caTarCharas;
						if(!tca) continue;
						CCObject* tco = NULL;
						int ta =  0;

						CCARRAY_FOREACH(tca,tco){
							ta += (999 - ((Chara*) tco)->gethp());
						}
						if(ta>tiMax){
							tiMax = ta;
							tiX = it->first;
							tiY = it->second;
						}

						//////////////////////////////////////////////////////////////////////////
					}
					//////////////////////////////////////////////////////////////////////////
				}
				CCLog(">[SkillJudge]CheckNorM() -------------------------------------------------------------- End ----------------------------------------------------------------");

				GameManager::sharedLogicCenter()->ml->bm->cs_y.insert(make_pair(tte->pos.x, tte->pos.y));
				GameManager::sharedLogicCenter()->ml->bm->clear_mcaTarget();
				GameManager::sharedLogicCenter()->ml->bm->clear_Arrange();

				if(tiMax <= 0) break;
				mbJudging = false;
				//CCLog(">[SkillJudge] Skill Find with value:%d || %d,%d", tiMax, tiX, tiY);

				AIComponent* ttai = ((AIComponent*) tte->getComponent("ai"));
				ttai->setSkillTargetPoint(tiX,tiY);	
				ttai->ActAttact(t_ssm.at("action_sp").c_str());		
			}

		}else{
			if(!EGroup::sharedEGroup()->IsEnemy(tte,tar)) break;
			tiMax = GameManager::sharedLogicCenter()->ml->bm->cs_y.count(make_pair(((EChesses*) tar)->pos.x, ((EChesses*) tar)->pos.y));

			CCLog(">[SkillJudge] checkNormc tiMax:%d", tiMax);
			GameManager::sharedLogicCenter()->ml->bm->clean_allcs();
			if(tiMax <= 0) break;
			

			if(abact){
				Scriptor* scp = new Scriptor();
				scp->parse_string(t_ssm.at("action_sp"));
				GameManager::sharedLogicCenter()->ml->bm->f_setcontroller(tte);
				GameManager::sharedLogicCenter()->ml->bm->m_caTarget->removeAllObjects();
				GameManager::sharedLogicCenter()->ml->bm->m_caTarget->addObject(tar);

				tte->ChangeFace(((EChesses*) tar)->pos);
				((EChesses*) tar)->ChangeFace(tte->pos);

				BattleField::sharedBattleField()->mbCurMagic = false;
				GameManager::sharedLogicCenter()->ml->bm->HandleScriptor(scp);

				scp->re_init();
				delete scp;
			}
		}


		
		mbJudging = false;
		GameManager::sharedLogicCenter()->ml->bm->clean_allcs();
		return true;
	} while (false);
	mbJudging = false;
	GameManager::sharedLogicCenter()->ml->bm->clean_allcs();
	return false;
}

