#include "SingleTon/BattleFiled.h"
#include "packui/CharaS.h"

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
	//miEnemy = 0;
}

void BattleField::CheckOver()
{
	if(!meTar) return;
	do 
	{
		CC_BREAK_IF(meTar->count() == 0);


		mbIsOver = false;
		return;
	} while (false);
	mbIsOver = true;
}

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
	// <扩散统帅属性
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
