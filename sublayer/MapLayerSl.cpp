#include "MapLayer.h"
#include "SingleTon\BattleFiled.h"

#define BM_Z 2

void MapLayer::LoadBattle(Script* ascp){

	m_iMLState = 2;				// <TODO |��EMERGENT | ʹ�ýŲ����г�ʼ�� [START] ����ת��ʼ�����ӿڿ��ƣ�ͬʱ����ͷ��������ű��ӿ�(f_setcamera || ��ʵ��)
	if(wm){
		wm->setVisible(false);
		wm->f_state_circle(false);
		wm->DeActive();

		show_hud();

		f_pauseall();
		f_stopcontrol();
	}

	const char* pszN = ascp->getstring("map");
	int i_egr = 0;				// <������|�����ͼ�������߼�ͳһ������޸�
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