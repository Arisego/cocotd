#include "MapLayer.h"
#include "SingleTon\BattleFiled.h"

#define BM_Z 2

void MapLayer::LoadBattle(Script* ascp){

	m_iMLState = 2;				// <TODO |　EMERGENT | 使用脚步进行初始化 [START] 将轮转开始交给接口控制，同时将镜头控制纳入脚本接口(f_setcamera || 已实现)
	if(wm){
		wm->setVisible(false);
		wm->f_state_circle(false);
		wm->DeActive();

		show_hud();

		f_pauseall();
		f_stopcontrol();
	}

	const char* pszN = ascp->getstring("map");
	int i_egr = 0;				// <调试用|载入地图　｜　逻辑统一后进行修改
	if(bm){
		bm->removeFromParent();
		CC_SAFE_RELEASE_NULL(bm);
	}
	bm = new BattleMap(pszN);

	bm->f_generateEnemy(i_egr);
	bm->f_load_chara();
	bm->PlaceEChesses(ascp);
	BattleField::sharedBattleField()->InitBattleFiled();	 
	bm->init();

	addChild(bm,BM_Z);
	tm = bm;
	f_init_battle();
	bm->EnActive();

	m_Spliter->f_open();
	m_Spliter->ShowClock();

	bm->cancontrol = true;
	init_Stat();

	BattleField::sharedBattleField()->InitBF();
	m_iTurn = ascp->getint("turn");
	switch_control();
}