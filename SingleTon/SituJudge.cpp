#include "SituJudge.h"
#include "SingleTon/BattleFiled.h"
#include "Component/AIComponent.h"
#include "GameManager.h"
#include "EGroup.h"

SituJudge *SituJudge::mSharedSituJudge = NULL;

SituJudge::SituJudge()
{
	meThink = NULL;
}

SituJudge::~SituJudge()
{

}
SituJudge *SituJudge::sharedSituJudge()
{
	if (mSharedSituJudge == NULL)
	{
		mSharedSituJudge = new SituJudge();
		if (!mSharedSituJudge || !mSharedSituJudge->init())
		{
			CC_SAFE_DELETE(mSharedSituJudge);
		}

	}

	return mSharedSituJudge;

}


void SituJudge::purgeSharedSituJudge()
{
	CC_SAFE_DELETE(mSharedSituJudge);

}

bool SituJudge::init()
{
	do 
	{
		mVals.clear();
		return true;
	} while (0);
	return false;
}

void SituJudge::JudgeMove( EChesses* tar )
{
	meThink = tar;

	if(!meThink) return;
	mVals.clear();
	m_Locks.clear();

	CCLog(">[SituJudge] JudgeMove | Thinker at : %0.0f,%0.0f", meThink->pos.x, meThink->pos.y);

	int tiMove = meThink->m_pChara->getMoveDis();		// Distance of move.

	int cx = tar->pos.x;
	int cy = tar->pos.y;

	//CCLog("+++");
	//////////////////////////////////////////////////////////////////////////
	for(map<pair<int,int>,EChesses*>::iterator it = BattleField::sharedBattleField()->mMapC.begin(); it != BattleField::sharedBattleField()->mMapC.end(); ++it){
		int tx = it->first.first;
		int ty = it->first.second;
		if(tx<0) continue;
		if(ty<0) continue;
		EChesses* te = it->second;

		if(te == tar) continue;
		m_Locks.insert(it->first);
		//[TODO] Try to get it from EChesses. <影响扩散
		int ads = 4;
		if((abs(tx-cx) + abs(ty-cy)) < (ads+tiMove)){
			DpsVals(te);
		}
	}

	FilterVals();
	float tfMax = 0;
	int tnx = 0;
	int tny = 0;
	GameManager::sharedLogicCenter()->ml->bm->draw_moving_tile();
	for(map<pair<int,int>, float>::iterator it = mVals.begin(); it != mVals.end(); ++it){		
		if(it->first.first<0) continue;
		if(it->first.second<0) continue;
		//if(abs(cx- (it->first.first))+abs(cy-(it->first.second))>tiMove) continue;
		if(GameManager::sharedLogicCenter()->ml->bm->cs_y.count(make_pair(it->first.first,it->first.second)) == 0 && GameManager::sharedLogicCenter()->ml->bm->cs_hit_block.count(make_pair(it->first.first,it->first.second)) == 0 ) continue;
		if(GameManager::sharedLogicCenter()->ml->bm->mBlockSet.count(make_pair(it->first.first,it->first.second)) > 0) continue;

		CCLog(">[SituJudge]JudgeMove() Point Comp:%d, %d || %0.2f", it->first.first, it->first.second, it->second);
		CCLog(">[SituJudge]JudgeMove() Check:%0.2f, %0.2f", tfMax, it->second);
		if(it->second > tfMax){		
			if(m_Locks.count(it->first)){
				CCLog(">[SituJudge]JudgeMove() You can not stand here, because somebody is here...");
				continue;
			}
			tfMax = it->second;
			tnx = it->first.first;
			tny = it->first.second;
		}
		
	}

	if(tfMax == 0){
		CCLog(">[SituJudge]JudgeMove() || No Point Fit.");
		tnx = cx;
		tny = cy;
	}
	CCLog(">[SituJudge]JudgeMove() | Owner: %s | Find:%d,%d match best|Value:%f", tar->name.c_str(), tnx, tny, tfMax);
	((AIComponent*) meThink->getComponent("ai"))->setTargetPoint(tnx,tny,tfMax);		// Test ONly
	//exit(97);
	//GameManager::sharedLogicCenter()->ml->bm->clean_allcs();
}

/* <关于group的补充设计，行动时使用自己的MASK和对方的ID对比，不为0则进行攻击类 */
void SituJudge::DpsVals( EChesses* aSrc )
{
	// <太远的单位不计算影响
	int dis = abs(aSrc->pos.x - meThink->pos.x) + abs(aSrc->pos.y - meThink->pos.y);
	if(dis > 10) return;		

	int tiLink = 4; // < 影响范围有效值 | 暂时不做区分
	int tx = aSrc->pos.x;
	int ty = aSrc->pos.y;
	CCLog(">[SituJudge] DpsVals() | %0.0f, %0.0f", tx, ty);

	
	// <预先演算覆盖区域
	Chara* tpchara = aSrc->m_pChara;
	if(tpchara->msAttakAttrs.size() < 1){
		int t_iType = tpchara->getvalue("attack");		
		GameManager::sharedLogicCenter()->ml->vdata.clear();
		DBUtil::initDB("save.db");
		CCString* t_csSql = CCString::createWithFormat("select * from attr_attact where id = %d",t_iType);
		CCLog("%s",t_csSql->getCString());
		GameManager::sharedLogicCenter()->ml->vdata = DBUtil::getDataInfo(t_csSql->getCString(),NULL);
		int m_number = GameManager::sharedLogicCenter()->ml->vdata.size();
		DBUtil::closeDB(); 

		map<string,string> t_ssm = (map<string,string>) GameManager::sharedLogicCenter()->ml->vdata.at(0);
		tpchara->msAttakAttrs = t_ssm.at("additional");
	}

	string tsAddition = tpchara->msAttakAttrs;
	GameManager::sharedLogicCenter()->ml->bm->f_setcontroller(meThink);
	GameManager::sharedLogicCenter()->ml->bm->clean_allcs();
	GameManager::sharedLogicCenter()->ml->BMAtDrawMove(tsAddition);

	// <输入
	CCLog(">[SituJudge] DpsVals() | Take In.");
	if(EGroup::sharedEGroup()->IsEnemy(aSrc, meThink)){   /* <对方会攻击自己的情况 */
		//tiLink = aSrc->getAlink();		< Test || 请从chara上读取这个值
		DpsValues(-JudgeDamageSc(aSrc,meThink) * 0.5, tx, ty, tiLink);
	}else{					/* <提供辅助的情况 */
		DpsValues(JudgeHelpSc(aSrc,meThink), tx, ty, tiLink);
	}

	// <预先演算覆盖区域
	tpchara = meThink->m_pChara;
	if(tpchara->msAttakAttrs.size() < 1){
		int t_iType = tpchara->getvalue("attack");		
		GameManager::sharedLogicCenter()->ml->vdata.clear();
		DBUtil::initDB("save.db");
		CCString* t_csSql = CCString::createWithFormat("select * from attr_attact where id = %d",t_iType);
		CCLog("%s",t_csSql->getCString());
		GameManager::sharedLogicCenter()->ml->vdata = DBUtil::getDataInfo(t_csSql->getCString(),NULL);
		int m_number = GameManager::sharedLogicCenter()->ml->vdata.size();
		DBUtil::closeDB(); 

		map<string,string> t_ssm = (map<string,string>) GameManager::sharedLogicCenter()->ml->vdata.at(0);
		tpchara->msAttakAttrs = t_ssm.at("additional");
	}

	tsAddition = tpchara->msAttakAttrs;
	GameManager::sharedLogicCenter()->ml->bm->f_setcontroller(aSrc);
	GameManager::sharedLogicCenter()->ml->bm->clean_allcs();
	GameManager::sharedLogicCenter()->ml->BMAtDrawMove(tsAddition);

	// <输出
	CCLog(">[SituJudge] DpsVals() | Give Out.");
	if(EGroup::sharedEGroup()->IsEnemy(meThink, aSrc)){
		DpsValues(JudgeDamageSc(meThink,aSrc) * 100, tx, ty, tiLink);
	}else{
		DpsValues(JudgeHelpSc(meThink,aSrc), tx, ty, tiLink);
	}

	// <清理
	GameManager::sharedLogicCenter()->ml->bm->clean_allcs();
	GameManager::sharedLogicCenter()->ml->bm->f_setcontroller(meThink);

}

int SituJudge::JudgeDamageSc( EChesses* aSrc, EChesses* aTar )
{
	int adam = aSrc->m_pChara->getvalue("atk");
	return adam;
}

int SituJudge::JudgeHelpSc( EChesses* aSrc, EChesses* aTar )
{
	int ahel = aSrc->m_pChara->getvalue("def");
	return ahel;
}

void SituJudge::DpsValues( int av, int ax, int ay, int arange )
{
	int nx, ny;
	float tfd;
	CCLog(">[SituJudge] DpsValues() | C:(%d,%d) T:%d", ax, ay, av);

	for(set<pair<int,int>>::iterator it = GameManager::sharedLogicCenter()->ml->bm->cs_y.begin();it != GameManager::sharedLogicCenter()->ml->bm->cs_y.end(); ++it)
	{
		nx = it->first;
		ny = it->second;

		tfd = (av/(abs(nx - ax) + abs(ny - ay) + 1));
		mVals[make_pair(nx,ny)] += tfd;
		CCLog(">[SituJudge] DpsValues() | (%d,%d)<<%f", nx, ny, tfd);
	}

	//for(int i = 0; i <= arange; ++i){
	//	ny = ay + i;
	//	for(int j = ax - arange + i; j <= ax + arange - i; ++j)
	//	{
	//		tfd = (av/(abs(j - ax) + i + 1));
	//		CCLog(">[SituJudge] DpsValues() | (%d,%d)<<%f", j, ny, tfd);
	//		mVals[make_pair(j,ny)] += tfd;
	//	}
	//}

	//for(int i = 1; i <= arange; ++i){
	//	ny = ay - i;
	//	for(int j = ax - arange + i; j <= ax + arange - i; ++j)
	//	{
	//		tfd = (av/(abs(j - ax) + i + 1));
	//		CCLog(">[SituJudge] DpsValues() | (%d,%d)<<%f", j, ny, tfd);
	//		mVals[make_pair(j,ny)] += tfd;
	//	}
	//}

}

void SituJudge::FilterVals()
{
	;
}
