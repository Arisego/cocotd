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
	//miDame = 9990; // <����

	EChesses* tESrc = (EChesses*) aSrc;
	EChesses* tETar = (EChesses*) aTar;
	Chara* tCSrc = tESrc->m_pChara;
	Chara* tCTar = tETar->m_pChara;

	meSrc = tESrc;
	meTar = tETar;
	//////////////////////////////////////////////////////////////////////////
	// <֪ͨ���˺��ж�
	tCTar->Trig(TR_BEHURT);
	tCSrc->Trig(TR_SRDAMA);

	//////////////////////////////////////////////////////////////////////////
	// <���Լӳ� - mtype = ����

	//////////////////////////////////////////////////////////////////////////
	// <�������
	int tExp;
	CCLog(">[EGroup] FactDamage() | Exp.");
	
	// <����������
	tExp = 0;
	if(CheckGrp(aSrc)){		// <����ҿ��Ƶ�λ����û�����壬�ʶ���ʱ���ṩ����ֵ
		switch (atype)
		{
		case 0:	// <������
			{
				// <�Է��ȼ�-�ҷ��ȼ�+4+�Է�boss�ȼ�
				tExp = 4 + iBossExp[tCTar->getvalue("boss_class")] + tCTar->getvalue("lv") - tCSrc->getvalue("lv");
				tExp = max(tExp,1);
				break;
			}
		default:	// <Ĭ�ϣ� ħ������
			{
				// <���ĵĻ�����(ExpHunp())+�Է��ȼ�-�ҷ��ȼ�+5+�Է�boss�׼���2,5,8,10�������Ϊ1�����и������˵�������ۼӣ��������ĵĻ�����ֻ��һ�μ��㡣
				tExp = 5+ iBossExp[tCTar->getvalue("boss_class")] + tCTar->getvalue("lv") - tCSrc->getvalue("lv"); 
				tExp = max(tExp,1);
				break;
			}
			
		}
		tCSrc->miEXP += tExp;
		CCLog(">[EGroup] FactDamage() | --> SRC:%d",tExp);
	}

	// <������������
	tExp = 1;
	if(CheckGrp(aTar)){
		tCTar->miEXP += tExp;
		CCLog(">[EGroup] FactDamage() | --> TAR:%d",tExp);
	}

	//////////////////////////////////////////////////////////////////////////
	// <���ս��
	CCLog(">[EGroup] FactDamage() | Damage:%d", miDame);

	tETar->miDamage	 =	miDame;
	tETar->miFianDamage += miDame;
	tETar->meRS = tESrc;

	//tETar->miAvgFlag = 1;		// <����

	tESrc->m_pChara->setHit(tETar->m_pChara->getvalue("boss_class") + 1);
	if(tETar->miFianDamage > tETar->m_pChara->gethp() && tETar->miFianDamage>0) tESrc->m_pChara->mbToKill = true;
	tESrc->m_pChara->miToDamage = miDame;
	tESrc->m_pChara->mcTar = tETar->m_pChara;
	//tESrc->m_pChara->Trig(TR_DAMAGE);
}

bool EGroup::CheckGrp(CCObject* aTar)
{
	EChesses* teTar = ((EChesses*) aTar);
	return (teTar->group_id == 0x01);	/* <��ҵ�ID��Զ��0x01 */
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
	EChesses* tETar = tESrc->meRS;		// <�˺���Դ�;�������
	if(!CheckGrp(tETar)) return;		// <�˺���Դ����Ҫ����

	Chara*	  tCSrc = tESrc->m_pChara;
	Chara*	  tCTar = tETar->m_pChara;

	int tiBossClass = tCSrc->getvalue("boss_class");
	int tExp = 0;

	if( 0 == tiBossClass){
		// <���Ƶб�ʱ���׷�Ӿ��飺���Է��ȼ�-�ҷ��ȼ�+4��*5   ���Ϊ1��
		tExp = 4 + tCSrc->getvalue("lv") - tCTar->getvalue("lv");
		tExp *= 5;
		tExp = max(1,tExp);
	}else{
		// <����bossʱ���׷�Ӿ��飺�����з�ս����-�ҷ�ս���������������������2�����ǲ�����-30��+�Է�boss�׼���50��70��100��120��+�����Է��ȼ�-�ҷ��ȼ���*10�����︺�������ȡ0��
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
	// <���ɲ��ŵĵ�λ
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
	// <���ɲ��ŵĵ�λ
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
	// <TEST | ... | ��õ�ǰ�Ļغ���
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

		if(tESrc->miHitFlag){	// <������������
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