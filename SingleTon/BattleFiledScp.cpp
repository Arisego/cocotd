#include "SingleTon/BattleFiled.h"
#include "GameManager.h"
#include "TOUI/LogicTree.h"
#include "utils/Scriptor.h"

/*
	<BattleField | �ű�ѭ�����
*/

void BattleField::InitBattleFiled()
{
	InitChessRefreh();
}

void BattleField::InitCheck()
{
	if(meTar->count() > 0){
		CCObject* tar_o;
		CCARRAY_FOREACH(meTar,tar_o){
			Chara* ttar = ((EChesses*) tar_o)->m_pChara;
			ttar->TurnOver();
		}
	}
	CCLog(">[BattleFieldScp] InitCheck() | sum:%d", mMapC.size());
	m_EIt = mMapC.begin();
	ShowChess(NULL);
}

bool BattleField::ActChack()
{
	CCLog(">[BattleFieldScp]ActCheck() | Is SomeOne Dead.");

	//return false;
	EChesses* tEc = NULL;
	for (;m_EIt != mMapC.end();++m_EIt)
	{
		//
		tEc = m_EIt->second;
		//tEc->m_pChara->checkVals();

		CCLog(">[BattleFieldScp]ActCheck() | +1");
		if(!tEc) continue;
		CCLog(">[BattleFieldScp]ActCheck() | +2");
		if(!tEc->m_pChara->m_bIsDead) continue;
		CCLog(">[BattleFieldScp]ActCheck() | +3");
		UnitDead();
		return true;
	}
	return false;
}

// <State || ��λ���� - ���ȣ�
void BattleField::UnitDead()
{
	EChesses* tEc = NULL;
	tEc = m_EIt->second;

	if(!tEc) return;
	CCLog(">[BattleFiledScp]UnitDead()");
	//////////////////////////////////////////////////////////////////////////
	// <�����ж�������ȷ��

	//////////////////////////////////////////////////////////////////////////
	// <����������λ

	mMapC.erase(m_EIt++);	// <�ӵ�ͼ�Ǽ���ɾ��
	((EChessComp*) tEc->getComponent("controller"))->onChessDead();
}

// <��λ�Ƿ��ǹ�������
bool BattleField::ECIsSrc(EChesses* aTar)
{
	return (meSrc == aTar);
}

//////////////////////////////////////////////////////////////////////////

bool BattleField::ExpCheck()
{
	CCLog(">[BattleFieldScp]ActCheck() | Check for possible Exp.");
	EChesses* tEc = NULL;
	for (;m_EIt != mMapC.end();++m_EIt)
	{
		tEc = m_EIt->second;
		CCLog(">[BattleFieldScp]ExpCheck() | +1");
		if(!tEc) continue;
		CCLog(">[BattleFieldScp]ExpCheck() | +2");
		if(tEc->m_pChara->miEXP == 0) continue;
		CCLog(">[BattleFieldScp]ExpCheck() | +3");
		ExpShow();
		return true;
	}
	return false;
}

void BattleField::ExpShow(){


	EChesses* tEc = NULL;
	tEc = m_EIt->second;

	if(!tEc) return;
	CCLog(">[BattleFiledScp]ExpShow()");
	((EChessComp*) tEc->getComponent("controller"))->onAddExp();

}

//////////////////////////////////////////////////////////////////////////
// <�ű��Խ�

Script* BattleField::Search_script(const char* asScp)
{
	if(mSpEvList)
		return (Script*) mSpEvList->objectForKey(asScp);
	return NULL;
}

void BattleField::ReadLogicPointScpW(LogicCell* atar, Script* ascp){
	if(!ascp){
		atar = NULL;
		return;
	}

	atar->miType = ascp->getint("type");
	if(atar->miType == 0){
		atar->msText = ascp->getstring("value");
		atar->msName = ascp->getstring("name");

		mss_Win.insert(atar->msName);
		msb_Win.insert(make_pair(atar->msName,atar));
	}
	
	CCLog(">[BattleFieldScp] ReadLogicPointScp | %d - %s", atar->miType, atar->msText.c_str());

	CCArray* tca = ascp->scriptnodes;
	CCObject* tco = NULL;
	CCARRAY_FOREACH(tca,tco){
		LogicCell* tlp = new LogicCell();
		ReadLogicPointScpW(tlp,(Script*) tco);
		atar->mvChilds.push_back(tlp);
	}

}

void BattleField::ReadLogicPointScpL(LogicCell* atar, Script* ascp){
	if(!ascp){
		atar = NULL;
		return;
	}

	atar->miType = ascp->getint("type");
	if(atar->miType == 0){
		atar->msText = ascp->getstring("value");
		atar->msName = ascp->getstring("name");

		mss_Lose.insert(atar->msName);
		msb_Lose.insert(make_pair(atar->msName,atar));
	}

	CCLog(">[BattleFieldScp] ReadLogicPointScp | %d - %s", atar->miType, atar->msText.c_str());

	CCArray* tca = ascp->scriptnodes;
	CCObject* tco = NULL;
	CCARRAY_FOREACH(tca,tco){
		LogicCell* tlp = new LogicCell();
		ReadLogicPointScpL(tlp,(Script*) tco);
		atar->mvChilds.push_back(tlp);
	}

}
void BattleField::InitBF()
{
	((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->MlLock();

	// <��ȡʤ������
	Script* ts = Search_script("win");

	mlp_Win = new LogicCell();
	ReadLogicPointScpW(mlp_Win, (Script*) ts->mapscpnodes->objectForKey(1));
	mlp_Lose = new LogicCell();
	ReadLogicPointScpL(mlp_Lose, (Script*) ts->mapscpnodes->objectForKey(2));

	// <��ȡ���ܵĵ�һ�ű���ִ��
	if(!GameManager::sharedLogicCenter()->m_bLoadProtect)
		onEvent("init");

	//GameManager::sharedLogicCenter()->ml->m_iMLState = 3;

}

void BattleField::Ex_script(Script* aScp,const char* asScp)
{
	GameManager::sharedLogicCenter()->ml->tm->m_sNaScp = asScp;
	GameManager::sharedLogicCenter()->ml->tm->m_iNaScp = -1;

	GameManager::sharedLogicCenter()->e_tmpscript(aScp->scriptnodes,aScp->getint("total"));
}

void BattleField::onEvent(const char* asEventName)
{
	// <����Ƿ�ʤ��
	if(mss_Win.count(asEventName)>0){
		CCLog(">[BattleFieldScp] onEvent | Win Node Meets.");
		msb_Win.at(asEventName)->isTrue = true;
		if(mlp_Win->checkValue()){
			CCLog(">[BattleFieldScp] onEvent | You Win!!!!");
			mbIsOver = true;
			return;
		}
	}

	// <����Ƿ�ʧ��

	// <����Ƿ��д���
	 Script* tscp = Search_script(asEventName);
	 if(tscp){
		 Ex_script((Script*) tscp->mapscpnodes->objectForKey(1), asEventName);
	 }
}

//////////////////////////////////////////////////////////////////////////
//

void BattleField::onDeadEvent(EChesses* tar)
{
	CCLog(">[BattleFieldScp] onDeadEvent.");
	if(meOrig == tar) {
		meOrig = nullptr;
	}

	if(BackChess1 == tar) BackChess1 = nullptr;
	if(BackChess2 == tar) BackChess2 = nullptr;
	if(BackChess3 == tar) BackChess3 = nullptr;
	if(meLastRe == tar)	  meLastRe = nullptr;

	if(GameManager::sharedLogicCenter()->ml->m_lsb->getContent() == tar) 
		GameManager::sharedLogicCenter()->ml->m_lsb->SetContent(NULL);
	if(GameManager::sharedLogicCenter()->ml->m_rsb->getContent() == tar) 
		GameManager::sharedLogicCenter()->ml->m_rsb->SetContent(NULL);

	int tx = tar->pos.x;
	int ty = tar->pos.y;
	int gid = tar->group_id;

	mMapC.erase(make_pair(tx,ty));
	CCLog(">[BattleFieldScp] onDeadEvent() | Remove Chess: (%d, %d) ", tx, ty);

	tar->m_pChara->RemoveRefHalos();
	onEvent((tar->name+"_dead").c_str());

	int ti = 0;
	for(map<pair<int,int>,EChesses*>::iterator it = mMapC.begin(); it != mMapC.end(); ++it){
		// <���û������
		CCLog(">[BattleFieldScp] onDeadEvent() | Check for cacked ptr. | %s", it->second->m_pChara->m_sName.c_str());
		if(it->second->m_pChara->mcTar){
			CCLog(">[BattleFieldScp] onDeadEvent() | Check for cacked ptr. | tar:%s", ((Chara*) it->second->m_pChara->mcTar)->m_sName.c_str() );
			if(((EChesses*) ((Chara*) it->second->m_pChara->mcTar)->mEcOwner) == tar ) 
				it->second->m_pChara->mcTar = NULL;
		}
		
		// <����Ƿ����Ѿ����
		if(it->second->group_id == gid){
			++ti;
			//break;
		}
	}

	if(0 == ti){
		onEvent("all_dead");		// <�����ã�ʵ��ʹ��ʱ��ָ������
	}
}
