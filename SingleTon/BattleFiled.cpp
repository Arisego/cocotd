#include "SingleTon/BattleFiled.h"
#include "packui/CharaS.h"
#include "GameManager.h"

static const int iBossHuiXin[] = {0,3,6,10,15};		// None C - S | 0 -4

BattleField *BattleField::mSharedBattleField = NULL;


BattleField *BattleField::sharedBattleField()
{
	if (mSharedBattleField == NULL)
	{
		mSharedBattleField = new BattleField();
		if (!mSharedBattleField || !mSharedBattleField->init())
		{
			CC_SAFE_DELETE(mSharedBattleField);
		}

	}

	return mSharedBattleField;

}


void BattleField::purgeSharedBattleField()
{
	CC_SAFE_DELETE(mSharedBattleField);

}

bool BattleField::init()
{
	do 
	{
		mspVals = NULL;
		meSrc = NULL;
		meTar = NULL;
		Clean();
		return true;
	} while (false);
	return false;
}

void BattleField::Clean()
{
	mbIsOver = false;

	mspVals = NULL;
	meSrc = NULL;
	meTar = NULL;

	mMapC.clear();

	BackChess1 = NULL;
	BackChess2 = NULL;
	BackChess3 = NULL;

	mbIsInBattle = false;
	//miState = 0;
	//miEnemy = 0;
}

void BattleField::CheckOver()
{
	if(!meTar) return;
	do 
	{
		//CC_BREAK_IF(meTar->count() == 0);


		mbIsOver = false;
		return;
	} while (false);
	mbIsOver = true;
}

/* <战斗设置 */
void BattleField::SetUp(EChesses* aSrc, CCArray* aTar, Script* sp)
{
	Clean();
	meSrc = aSrc;
	meTar = aTar;
	mspVals = sp;
	CCLog(">[BF]Battle Field Receive the battle:%d",aTar->count());

	if(sp=NULL) 
		CCLog(">[BF]Battle with an empty values....");
}

void BattleField::SetSrc( EChesses* aSrc )
{
	Clean();
	meSrc = aSrc;
	meOrig = aSrc;
	miState = 0;

	GameManager::sharedLogicCenter()->ml->m_lsb->SetContent(meSrc);
}


void BattleField::SetTars( CCArray* aTar )
{
	if(!aTar) {
		GameManager::sharedLogicCenter()->ml->m_rsb->setVisible(false);
		GameManager::sharedLogicCenter()->ml->m_lsb->SetNullAct();
		return;
	}
	meTar = aTar;
	if(!mbIsInBattle) {
		return;
	}
	CheckBackCh();
	PreJudge((EChesses*) aTar->objectAtIndex(0));
	GameManager::sharedLogicCenter()->ml->m_rsb->setVisible(true);
}


bool BattleField::IsOver()
{
	return mbIsOver;
}

void BattleField::AMOver()
{
	CheckOver();
}

//////////////////////////////////////////////////////////////////////////
// <战场设置
void BattleField::SetChess( EChesses* ae,int ax, int ay )
{
	mMapC.insert(make_pair(make_pair(ax,ay),ae));
	ae->m_pChara->ClearLead();
}

void BattleField::InitChessRefreh()
{
	EChesses* te;
	int tiLead;
	// <扩散统帅属性 --< 需要修改
	for(map<pair<int,int>,EChesses*>::iterator it = mMapC.begin(); it != mMapC.end(); ++it){
		te = it->second;
		tiLead = te->m_pChara->getLead();
		CCLog(">[BF]Initing the chess lead values...");
		DerLead(tiLead,it->first.first,it->first.second);
	}

}
//////////////////////////////////////////////////////////////////////////
// <统帅
void BattleField::DepLead( int centx, int centy, int range, int val )
{
	EChesses* tec;
	int ny;
	CCLog(">[BF]DepLead, need test:range-%d,val-%d...",range,val);
	for(int i = 0; i <= range; ++i){
		ny = centy + i;
		for(int j = centx - range + i; j <= centx + range - i; ++j)
		{
			tec = mMapC[make_pair(j,ny)];
			if(tec){
				tec->m_pChara->AddLead(val);
			}
		}
	}

	for(int i = 1; i <= range; ++i){
		ny = centy - i;
		for(int j = centx - range + i; j <= centx + range - i; ++j)
		{
			tec = mMapC[make_pair(j,ny)];
			if(tec){
				tec->m_pChara->AddLead(val);
			}
		}
	}
}

void BattleField::DepRemoveLead( int centx, int centy, int range, int val )
{
	EChesses* tec;
	int ny;
	for(int i = 0; i <= range; ++i){
		ny = centy + i;
		for(int j = centx - range + i; j <= centx + range - i; ++j)
		{
			tec = mMapC[make_pair(j,ny)];
			if(tec){
				tec->m_pChara->RemoveLead(val);
				tec->m_pChara->RefreshLead();
			}
		}
	}

	for(int i = 1; i <= range; ++i){
		ny = centy - i;
		for(int j = centx - range + i; j <= centx + range - i; ++j)
		{
			tec = mMapC[make_pair(j,ny)];
			if(tec){
				tec->m_pChara->RemoveLead(val);
				tec->m_pChara->RefreshLead();
			}
		}
	}
}

void BattleField::ChessMoved( EChesses* ae, CCPoint astart, CCPoint aend )
{
	CCLog(">[BF]Chess Moved");
	int tiLead = ae->m_pChara->getLead();
	UnDerLead(tiLead,astart.x,astart.y);
	DerLead(tiLead,aend.x,aend.y);
	
}

void BattleField::DerLead( int val,int cx, int cy )
{
	switch (val)
	{
	case 0:
		{
			break;
		}
	case 1:
		{
			DepLead(cx, cy, 1, 1);
			break;
		}
	case 2:
		{
			DepLead(cx, cy, 2, 1);
			break;
		}
	case 3:
		{
			DepLead(cx, cy, 4, 1);
			break;
		}
	case 4:
		{
			DepLead(cx, cy, 5, 2);
			break;
		}
	case 5:
		{
			DepLead(cx, cy, 6, 3);
			break;
		}
	default:
		break;
	}
}

void BattleField::UnDerLead( int val,int cx, int cy )
{

	//////////////////////////////////////////////////////////////////////////
	switch (val)
	{
	case 0:
		{
			break;
		}
	case 1:
		{
			DepLead(cx, cy, 1, 1);
			break;
		}
	case 2:
		{
			DepLead(cx, cy, 2, 1);
			break;
		}
	case 3:
		{
			DepLead(cx, cy, 4, 1);
			break;
		}
	case 4:
		{
			DepLead(cx, cy, 5, 2);
			break;
		}
	case 5:
		{
			DepLead(cx, cy, 6, 3);
			break;
		}
	default:
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
// <战斗
//meSrc = aSrc;
//meTar = aTar;
//mspVals = sp;

void BattleField::Judge(){
	//////////////////////////////////////////////////////////////////////////
	// <会心

	int tiHitFlag = 0;
	Chara* src = meSrc->m_pChara;
	CCObject* tar_o = NULL;
	Chara* tar = NULL;
	int hurt;
	bool tbSingle;
	int hit_rate_base = src->getFixValue("hit")*5 + src->getFixValue("luk");
	CCARRAY_FOREACH(meTar,tar_o){
		tar = ((EChesses*) tar_o)->m_pChara;
		

		/* <计算命中率 */
		float hit_rate = (( hit_rate_base - tar->getFixValue("avg") * 5 - tar->getFixValue("luk"))/100) + 0.7 + src->getFixRate();
		CCLog(">[BF]MingZhong-%f",hit_rate);
		tbSingle = (CCRANDOM_0_1()<hit_rate);
		if(!tbSingle) {
			((EChesses*) tar_o)->miAvgFlag = 1;
			CCLog(">[BF]Miss....");
			break;
		}

		((EChesses*) tar_o)->miAvgFlag = 0;
		CCLog(">[BF]Hit");

		/* <计算会心率 */
		hit_rate = (src->getFixValue("base_hit") + src->getFixValue("luk")/5 - iBossHuiXin[tar->getvalue("boss_class")])/100 + src->getFixRate();
		CCLog(">[BF]HuiXing-%f",hit_rate);
		tbSingle = (CCRANDOM_0_1()<hit_rate);

		/* <进行第一轮伤害计算 */	
		if(tbSingle){
			tiHitFlag |= 1;
			CCLog(">[BF]Extra Hurt.");
			 src->getFixValue("atk") * 2.5 + src->getFixValue("a_atk");
			 meSrc->miHitFlag = 1;
		}else{
			meSrc->miHitFlag = 0;
			hurt = src->getFixValue("atk") + src->getFixValue("a_atk");				// hurt = mspVals->getint("damage") + src->getvalue("mag") - tar->getvalue("rst");
		}
		
			
		/* <根据是否玩家控制势力将会弹出格挡判定 */
		// ....
		hurt -= tar->getFixValue("def");

		/* <计算完成导入Entile并计算经验值 */
		// ...
		((EChesses*) tar_o)->miDamage = hurt;
	}
	
	if(tiHitFlag != 0){
		meSrc->miHitFlag = 1;
	}else{
		meSrc->miHitFlag = 0;
	}
	
}

void BattleField::PlayEffectSp( const char* asname, CCPoint end )
{
	CCPoint start = meSrc->getPosition();				// <位置和角度有待修正

	CCSprite* tcs = CCSprite::create(asname);			// <改进时，可在sp中根据asname为基准进行查询
	tcs->setAnchorPoint(CCPointZero);
	tcs->setPosition(start);

	meSrc->getParent()->addChild(tcs);
	tcs->runAction(CCSequence::create(CCMoveTo::create(0.7,end),CCRemoveSelf::create(),NULL));
}

bool BattleField::LogicContinue()
{
	bool ret = false;
	switch (miState)
	{
	case 0:		// <第一次攻守交换
	case 1:		// <完成数据读取
		{
			if(!mspVals){		// <没有参数脚本时作为普通攻击对待
				ret = NormalAttackC();				
			}
			break;
		}
	case 3:		// <攻击发起方可反击的情况
		{
			if(!mspVals){		
				// <第三次攻击只有可能是普通攻击
				GameManager::sharedLogicCenter()->ml->bm->m_caTarget->removeAllObjects();
				GameManager::sharedLogicCenter()->ml->bm->m_caTarget->addObject(meSrc);
				GameManager::sharedLogicCenter()->ml->bm->m_controller = meOrig;
				GameManager::sharedLogicCenter()->ml->RePack();
				
				miState = 1;			// <回到初始反击查询状态
				ret = true;
			}
			break;
		}
	default:
		break;
	}
	return ret;

}

bool BattleField::NormalAttackC()
{
	do 
	{
		//meTar->
		// <首先挑选出
		if(!meTar) return false;
		

		// <弹出选择将交由EChessComp负责
		if(BackChess1){
			if(TestBackCh(BackChess1)){
				if(miState != 2) BackChess1 = NULL;
				else miState = 1;
				break;
			}
		}

		if(BackChess2){
			if(TestBackCh(BackChess2)) {
				if(miState != 2) BackChess2 = NULL;
				else miState = 1;
				break;
			}
		}

		if(BackChess3){
			if(TestBackCh(BackChess3)) {
				if(miState != 2) BackChess3 = NULL;
				else miState = 1;
				break;
			}
		}
		miState = 5;
		return false;
	} while (0);
	return true;
}

void BattleField::CheckBackCh()
{
	if(miState >= 1) return;
	BackChess1 = NULL;
	BackChess2 = NULL;
	BackChess3 = NULL;

	int m1 = 0;
	int m2 = 0;
	int m3 = 0;
	int t = 0;

	CCObject* tco;
	CCARRAY_FOREACH(meTar,tco){
		t = ((EChesses*) tco)->m_pChara->GetRePrioer();
		if(t<=m3){
			continue;
		}else if(t<=m2){
			BackChess3 = (EChesses*) tco;
			m3 = t;
		}else if(t<= m1){
			BackChess3 = BackChess2;
			BackChess2 = (EChesses*) tco;
			m2 = t;
		}else{
			BackChess3 = BackChess2;
			BackChess2 = BackChess1;
			BackChess1 = (EChesses*) tco;
			m1 = t;
		}
	}
	miState = 1;
	CCLog(">[BF]Get ReBack Chess Over with prior:%d,%d,%d",m3,m2,m1);
}

bool BattleField::TestBackCh(EChesses* atar)
{
	do 
	{
		int delta_spd = (atar->m_pChara->getvalue("spd") - meOrig->m_pChara->getvalue("spd"));
		if( delta_spd >= 5){			// <被攻击对象速度较快
			miState = 2;
			CC_BREAK_IF(((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,2));		// <FindFitRe会根据自己的状态对miState进行修改
		}else if(delta_spd <= -5){		// <攻击发起对象速度较快
			miState = 3;
			CC_BREAK_IF(((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,1));
		}else{							// <速度在平衡范围内，不发起第三次攻击
			CC_BREAK_IF(((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,1));
		}
		

		// <RunScript()  --> Judge()
		return false;
	} while (0);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// <预先判定
void BattleField::PreJudge(EChesses* atar)
{
	if(!atar) return;
	Chara* src = meOrig->m_pChara;
	Chara* tar = atar->m_pChara;

	int hit_rate = src->getFixValue("hit")*5 + src->getFixValue("luk") - tar->getFixValue("avg") * 5 - tar->getFixValue("luk") + 70 + src->getFixRate()*100;
	int hx_rate = (src->getFixValue("base_hit") + src->getFixValue("luk")/5 - iBossHuiXin[tar->getvalue("boss_class")])/100 + src->getFixRate();
	int damage =src->getFixValue("atk") + src->getFixValue("a_atk");	

	GameManager::sharedLogicCenter()->ml->m_lsb->RefreshAct(hit_rate, hx_rate, damage);

	do 
	{
		
		if(atar == BackChess1){
			break;
		}
		if(atar == BackChess2){
			break;
		}
		if(atar == BackChess3){
			break;
		}
		return;
	} while (0);
	//////////////////////////////////////////////////////////////////////////
	// <该单位是反击单位的情况
	int delta_spd = (atar->m_pChara->getvalue("spd") - meOrig->m_pChara->getvalue("spd"));
	if( delta_spd >= 5){			// <被攻击对象速度较快
		miState = 2;
		//((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,2);		// <FindFitRe会根据自己的状态对miState进行修改
	}else if(delta_spd <= -5){		// <攻击发起对象速度较快
		miState = 3;
		//((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,1);
	}else{							// <速度在平衡范围内，不发起第三次攻击
		//((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,1);
	}
	
}

void BattleField::setBattle( bool ab )
{
	mbIsInBattle = ab;
}

