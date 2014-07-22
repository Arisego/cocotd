#include "BattleFiled.h"
#include "GameManager.h"

/* <战场存档类 */

void BattleField::setSaveState(){
	CCLog(">[BattleField] setSaveState()");
	mbNeedSave = false;
	if(GameManager::sharedLogicCenter()->ml->m_iMLState < 2) return;
	GameManager::sharedLogicCenter()->ml->tm->DeActive();
	GameManager::sharedLogicCenter()->ml->tm = GameManager::sharedLogicCenter()->ml->wm;
	mbNeedSave = true;
}

void BattleField::unsetSaveState(){
	CCLog(">[BattleField] unsetSaveState()");
	if(GameManager::sharedLogicCenter()->ml->m_iMLState < 2) return;
	GameManager::sharedLogicCenter()->ml->tm = GameManager::sharedLogicCenter()->ml->bm;
	GameManager::sharedLogicCenter()->ml->tm->EnActive();
}

void BattleField::setLoadState(){
	//if(GameManager::sharedLogicCenter()->ml) GameManager::sharedLogicCenter()->ml->tm->DeActive();
	CCLog(">[BattleField] setLoadState()");
	if(GameManager::sharedGameManager()->state == GameManager::SCENE_PLAY){
		if(GameManager::sharedLogicCenter()->ml->m_iMLState < 2) return;	/* <没有地图载入时退出这个过程 */
		miState = -99;
		GameManager::sharedLogicCenter()->ml->tm->DeActive();
		GameManager::sharedLogicCenter()->ml->clearBattle();
		
		
		
		BattleField::sharedBattleField()->Clean();
		BattleField::sharedBattleField()->ClearBF();
		
	}
}

Script* sm_Scp;
void BattleField::Load_Script(Script* ascp){
	mbNeedSave = true;
	CCLog(">[BattleFieldSl] Load_Script() | Scp Handle.");
	sm_Scp = ascp;
}

void BattleField::Load_Over(){
	if(!mbNeedSave) return;
	CCLog(">[BattleFieldSl] Load_Script() | Load BattleField Begins.");

	GameManager::sharedLogicCenter()->ml->LoadBattle(sm_Scp);
	sm_Scp = nullptr;
	//GameManager::sharedLogicCenter()->ml->m_iMLState = 2;		// <其他的战斗地图状态都是不允许存档的
}