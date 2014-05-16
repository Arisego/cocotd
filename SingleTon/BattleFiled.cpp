#include "SingleTon/BattleFiled.h"
#include "packui/CharaS.h"
#include "GameManager.h"
#include "Component/AIComponent.h"
#include "EGroup.h"
#include "battle/BBuffer.h"
#include "SkillJudge.h"

static const int iBossHuiXin[] = {0,3,6,10,15};		// None C - S | 0 1 - 4


//////////////////////////////////////////////////////////////////////////

int DisTPoints(CCPoint src,CCPoint dst);

//////////////////////////////////////////////////////////////////////////

BattleField *BattleField::mSharedBattleField = NULL;


BattleField *BattleField::sharedBattleField()
{
	//if(GameManager::sharedLogicCenter()->ml->m_iMLState < 2){	// <���ڲ���ս����ͼ��������ṩbfʵ��
	//	return NULL;
	//}
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
		mlp_Win  = NULL;
		mlp_Lose = NULL;

		mspVals = NULL;
		meSrc = NULL;
		meTar = new CCArray();
		Clean();
		mSpLinker = new Scriptor();
		mCachedJudge = NULL;
		BSkillList::sharedBSkillList();
		mbNeLock = false;
		meCtnTarget = NULL;
		miLastSkillID = 0;

		return true;
	} while (false);
	return false;
}

void BattleField::Clean()
{
	mbIsOver = false;

	CC_SAFE_RELEASE_NULL(mspVals);
	meSrc = NULL;

	CCLog(">[BattleField] LeakCheck . CleanMetar");
	
	if(miState >= 1) return;
	if(meTar->count()>0) {
		GameManager::sharedLogicCenter()->ml->m_rsb->SetContent(NULL);
		//GameManager::sharedLogicCenter()->ml->bm->clear_mcaTarget();
	}
	meTar->removeAllObjects();
	

	//mMapC.clear();

	//BackChess1 = NULL;
	//BackChess2 = NULL;
	//BackChess3 = NULL;

	//mbIsInBattle = false;
	//miState = 0;
	//miEnemy = 0;
}

void BattleField::CheckOver()
{
	//mbIsOver = true;			// FOR_DEBUG
	
	if(meTar->count()==0) return;
	do 
	{
		//CC_BREAK_IF(meTar->count() == 0);


		//////////////////////////////////////////////////////////////////////////
		
		setBattle(false);
		if(mbIsOver) break;
		Clean();
		mbIsOver = false;
		return;
	} while (false);
	mbIsOver = true;
}

/* <ս������ 
		|< �׶�-BF0000����ʼ����ս�����ã�����ű��Ͷ��� [������Դ BattleMap::HandleScriptor]
		|< ����ʵ�ʵ�����ʲô��ֻ���������׼���������������Ը���Component�ĵ��á�
*/
void BattleField::SetUp(EChesses* aSrc, CCArray* aTar, Script* sp)
{
	Clean();

	meSrc = aSrc;
	setMeTar(aTar);

	mspVals = sp;
	if(sp) mspVals->retain();

	mCachedSPp.clear();
	mCachedSPpScs.clear();
	meCtnTarget = NULL;

	if(sp=NULL) 
		CCLog(">[BattleField]Battle with an empty values....");
	else
	{
		CCLog(">[BattleField]Battle Field Prepare: Skill");
	}

	CCLog(">[BattleField] SetUp() | mistate == %d", miState);

	if(!aTar) return;
	if(aTar->count() == 0) return;
	if(miState == 0) return;

	meSrc->m_pChara->Trig(TR_BEGIN);
}

void BattleField::SetSrc( EChesses* aSrc )
{
	if(!aSrc) {
		miSFlag = 0;
		return;
	}
	Clean();
	setBattle(false);
	GameManager::sharedLogicCenter()->ml->bm->clear_mcaTarget();
	GameManager::sharedLogicCenter()->m_bLoadProtect = false;

	meSrc = aSrc;
	meOrig = aSrc;
	miState = 0;
	miActCount = 0;
	meLastRe = NULL;
	mbIsMagic = false;
	mbCurMagic = false;
	meOrig->m_pChara->miSeType = 0;

	/* [SS] <Ŀ�걻ѡ��Ϊ������λ  */
	if(meOrig->m_pChara->getvalue("b_xudong") == meOrig->m_pChara->getvalue("b_maxxudong")) {
		if(EGroup::sharedEGroup()->getRound() < 1) meOrig->m_pChara->PlaySS("select");
		else meOrig->m_pChara->PlaySS("select_s");
	}else{
		meOrig->m_pChara->PlayXuDong();
	}

	GameManager::sharedLogicCenter()->ml->m_lsb->SetContent(meOrig);
	GameManager::sharedLogicCenter()->ml->m_lsb->setVisible(true);
	ShowChess(meOrig);

	miSFlag = 1;
}


void BattleField::SetTars( CCArray* aTar )
{
	if(!aTar) {
		if(meSrc != GameManager::sharedLogicCenter()->ml->bm->m_controller) return;
		if(meSrc != meOrig) return;
		GameManager::sharedLogicCenter()->ml->m_rsb->setVisible(false);
		GameManager::sharedLogicCenter()->ml->Dissmiss_Arrows();
		GameManager::sharedLogicCenter()->ml->m_lsb->SetNullAct();
		return;
	}
	setMeTar(aTar);
	if(mbIsInBattle) {
		return;
	}
	miState = 0;
	meTarget = NULL;

	CheckBackCh();
	PreJudge((EChesses*) aTar->objectAtIndex(0));
	
	
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
// <ս������
void BattleField::SetChess( EChesses* ae,int ax, int ay )
{
	mMapC.insert(make_pair(make_pair(ax,ay),ae));
	ae->m_pChara->ClearLead();
	ae->m_pChara->mEcOwner = ae;
}

void BattleField::InitChessRefreh()
{
	EChesses* te;
	EChesses* ste;
	int tiLead;
	// <��ɢͳ˧���� 
	map<pair<int,int>,EChesses*>::iterator it;
	map<pair<int,int>,EChesses*>::iterator sit;
	for(it = mMapC.begin(); it != mMapC.end(); ++it){
		te = it->second;		
		CCLog(">[BattleField]InitChessRefreh() || Initing the chess lead values...");

		for(sit = mMapC.begin(); sit != mMapC.end(); ++sit){
			CCLog(">[BattleField] InitChessRefresh() || For-For:%d,%d",sit->first.first,sit->first.second);
			ste = sit->second;
			tiLead = ste->m_pChara->getLead();
			te->m_pChara->AddLead(DerLead(tiLead,it->first.first,it->first.second,sit->first.first,sit->first.second));
		}

	}
	CCLog(">[BattleField] InitChessRefres() || Over. Lead.");

}
//////////////////////////////////////////////////////////////////////////
// <ͳ˧

/* <����ͳ˧����ֵ����ͳ˧��� */
int BattleField::DerLead( int val,int cx, int cy,int dx, int dy )
{
	int t = abs(cx-dx) + abs(cy-dy);
	switch (val)
	{
	case 0:
		{

			break;
		}
	case 1:
		{
			if(t<=1) return 1;
			//DepLead(cx, cy, 1, 1);
			break;
		}
	case 2:
		{
			if(t<=2) return 1;
			//DepLead(cx, cy, 2, 1);
			break;
		}
	case 3:
		{
			if(t<=4) return 1;
			//DepLead(cx, cy, 4, 1);
			break;
		}
	case 4:
		{
			if(t<=5) return 2;
			//DepLead(cx, cy, 5, 2);
			break;
		}
	case 5:
		{
			if(t<=6) return 3;
			//DepLead(cx, cy, 6, 3);
			break;
		}
	default:
		break;
	}
	return 0;
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
			}else{
				mMapC.erase(make_pair(j,ny));
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
			}else{
				mMapC.erase(make_pair(j,ny));
			}
		}
	}
}

void BattleField::ChessMoved( EChesses* ae, CCPoint astart, CCPoint aend )
{
	CCLog(">[BattleField]Chess Moved");
	if(ae->pos.x == aend.x && ae->pos.y == aend.y){
		CCLog(">[BattleFiled]ChessMoved() || Duplicate Call. No Need to Move.");
		return;
	}
	int tiLead = ae->m_pChara->getLead();
	mMapC.erase(make_pair(astart.x,astart.y));
	mMapC.insert(make_pair(make_pair(aend.x,aend.y),ae));

	// <�Ƴ����滻���е�ͳ˧���
	UnDerLead(tiLead,astart.x,astart.y);
	DerLead(tiLead,aend.x,aend.y);

	// <�Ƴ��������õ���ͳ˧�ӳ�
	ae->m_pChara->ClearLead();

	EChesses* ste;
	CCLog(">[BattleFiled]ChessMoved() || Initing the chess lead values...");

	// <����µ�ͳ˧�ӳ�
	for(map<pair<int,int>,EChesses*>::iterator sit = mMapC.begin(); sit != mMapC.end(); ++sit){
		ste = sit->second;
		tiLead = ste->m_pChara->getLead();
		ae->m_pChara->AddLead(DerLead(tiLead,aend.x,aend.y,sit->first.first,sit->first.second));
	}
	ae->pos = aend;
	CCLog(">[BattleField] ChessMoved() || Lead values count over...");
	CCLog(">[BattleField] ChessMoved() || Over [%f,%f] -> [%f,%f]", astart.x, astart.y, aend.x, aend.y);
	EChessComp* tecc = (EChessComp*) ae->getComponent("controller");
	if(tecc) tecc->update(0.1);

	// <�⻷���
	for(map<pair<int,int>,EChesses*>::iterator sit = mMapC.begin(); sit != mMapC.end(); ++sit){
		ae->m_pChara->CheckHalo(ae, astart, aend);
	}

	ae->m_pChara->RemoHalo();

}

void BattleField::DepLead( int centx, int centy, int range, int val )
{
	EChesses* tec;
	int ny;
	CCLog(">[BattleField]DepLead, need test:range-%d,val-%d...",range,val);
	for(int i = 0; i <= range; ++i){
		ny = centy + i;
		for(int j = centx - range + i; j <= centx + range - i; ++j)
		{
			tec = mMapC[make_pair(j,ny)];
			if(tec){
				tec->m_pChara->AddLead(val);
			}else{
				mMapC.erase(make_pair(j,ny));
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
			}else{
				mMapC.erase(make_pair(j,ny));
			}
		}
	}
}

/* <ע�⺯����ͬ�����أ���ɢͳ˧���ԡ� */
void BattleField::DerLead( int val,int cx, int cy ){
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
			DepRemoveLead(cx, cy, 1, 1);
			break;
		}
	case 2:
		{
			DepRemoveLead(cx, cy, 2, 1);
			break;
		}
	case 3:
		{
			DepRemoveLead(cx, cy, 4, 1);
			break;
		}
	case 4:
		{
			DepRemoveLead(cx, cy, 5, 2);
			break;
		}
	case 5:
		{
			DepRemoveLead(cx, cy, 6, 3);
			break;
		}
	default:
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
// <ս��
//meSrc = aSrc;
//meTar = aTar;
//mspVals = sp;

bool BattleField::LogicContinue()
{
	bool ret = false;
	CCLog(" --------- Logic Node A-New ---------");
	CCLog(">[BattleField]LogicContinue() | State:%d",miState);
	if(!meOrig && (abs(miState) < 5)){
		miState = -5;
	}

	switch (miState)
	{
	case 0:		// <��һ�ι��ؽ���
	case 1:		// <������ݶ�ȡ || State1 ��Ϊ����״̬ÿ��һ���������֮�󶼻ᱻ��ѯ
	case 2:
		{
			//if(!mspVals){		// <û�в����ű�ʱ��Ϊ��ͨ�����Դ� <<<< ���й��������Է���
				ret = NormalAttackC();				
			//}
			break;
		}
	case 3:		// <�������𷽿ɷ�������� || ���ڶ��ص���ͨ��������ֱ���е����ι���
		{
			if(!mbIsMagic){		
				meOrig->m_pChara->miSeType = 2;

				if(meLastRe)
					ret = SkillJudge::sharedSkillJudge()->CheckNorM(meOrig, meLastRe, true);
				
				miState = 1;			// <�ص���ʼ������ѯ״̬
				if(!ret) ret = LogicContinue();
			}else{
				CCLog(">[BattleField] LogicContinue | mbIsMagec == true");
				miState = 1;
				ret = LogicContinue();
			}
			//ret = LogicContinue();
			break;
		}
	case 5:	// <������ɺ������
		{
			InitCheck();
			miState = -5;
			//break;	// <֮���Ȼ��-5������һ�ε���
		}
	case -5:
		{
			CCLog(">[BattleField]LogicContinue() || Unit State Check...");
			ret = ActChack();
			//////////////////////////////////////////////////////////////////////////
			// <��λ��������Ѿ���ɺ� || ���ž���ֵ
			if(!ret){
				CCLog(">[BattleField]LogicContinue() || Unit State Check over.");
				miState = 6;
				ret = true;
				InitCheck();
			}else{
				break;
			}			
		}
	case 6:	// <����ֵ����
		{
			ret = ExpCheck();
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
		// <������ѡ��
		if(meTar->count()==0 && miState == 0) {
			CCLog(">[BattleField] NormalAttackC() | return false?");
			return false;
		}
			
		CheckBackCh();

		CCLog(">[BattleField] NormalAttackC() | with count num: %d.", miActCount);
		 // <��һ�ι���/���ܲ������
		if(0 == miActCount){
			ClearTrigFlag();
			if(!mbIsMagic){
				meOrig->m_pChara->Trig(ATK_FIREST);
			}

			if(mbTrigged){
				++miActCount;
				return true;
			}
		}
		++miActCount;


		// <����ѡ�񽫽���EChessComp����
		/*
			<miState == 2 ��ζ�ŵз�Ҫ�������� [BackChess1û���ÿջ��ٴ�ִ�м��]
			
			<miState == 3 �ҷ��ɷ���
			<miState == 1 ֱ�ӽ�����һ�ּ��
		*/
		if(BackChess1){
			if(((EChessComp*) BackChess1->getComponent("controller"))->miReState == 0){
				ClearTrigFlag();
				BackChess1->m_pChara->Trig(TR_ATKED);
				((EChessComp*) BackChess1->getComponent("controller"))->miReState = 1;
				if(mbTrigged){
					++miActCount;
					CCLog(">[BattleField] NormalAttackC() | Trigged Success.");
					return true;
				}
			}

			CCLog(">[BattleField] NormalAttackC() | BackChess.");
			if(TestBackCh(BackChess1)){
				if(miState != 2){
					BackChess1 = NULL;
				}else{
					miState = 1;
				}
				break;
			}
		}

		if(BackChess2){
			if(((EChessComp*) BackChess2->getComponent("controller"))->miReState == 0){
				ClearTrigFlag();
				BackChess2->m_pChara->Trig(TR_ATKED);
				((EChessComp*) BackChess2->getComponent("controller"))->miReState = 1;
				if(mbTrigged){
					++miActCount;
					return true;
				}
			}


			if(TestBackCh(BackChess2)) {
				if(miState != 2) BackChess2 = NULL;
				else miState = 1;
				break;
			}
		}

		if(BackChess3){
			if(((EChessComp*) BackChess3->getComponent("controller"))->miReState == 0){
				ClearTrigFlag();
				BackChess3->m_pChara->Trig(TR_ATKED);
				((EChessComp*) BackChess3->getComponent("controller"))->miReState = 1;
				if(mbTrigged){
					++miActCount;
					return true;
				}
			}


			if(TestBackCh(BackChess3)) {
				if(miState != 2) BackChess3 = NULL;
				else miState = 1;
				break;
			}
		}
		miState = 5;
		return LogicContinue();
		//return false;			// <[Chage] ǿ��ִ�е�λ�������
	} while (0);
	return true;
}

/* <ʱ��Σ����㷴����λ-ֻ��һ��ʼ����һ�� */
void BattleField::CheckBackCh()
{
	if(miState >= 1) return;

	// < -- ������Ч | ATK/SKILL
	if(!mbIsMagic){
		meOrig->m_pChara->miSeType = 1;
	}

	//

	meLastRe	= NULL;

	CCLog(">[BattleField] CheckBackCh() | num of chara: %d", meTar->count());
	//miActCount = 0;
	//CCLog("+++++++++++++++++++++++++++++++++++++checkback");
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
			((EChessComp*) BackChess3->getComponent("controller"))->CleanRe();
		}else if(t<= m1){
			BackChess3 = BackChess2;
			BackChess2 = (EChesses*) tco;
			((EChessComp*) BackChess2->getComponent("controller"))->CleanRe();
			m2 = t;
		}else{
			BackChess3 = BackChess2;
			BackChess2 = BackChess1;
			BackChess1 = (EChesses*) tco;
			((EChessComp*) BackChess1->getComponent("controller"))->CleanRe();
			m1 = t;
		}
	}
	miState = 1;
	CCLog(">[BattleField]Get ReBack Chess Over with prior:%d,%d,%d",m3,m2,m1);
}


int BattleField::TestPrior(CCObject* acSrc, CCObject* acTar)
{
	if(!acSrc) acSrc = meSrc->m_pChara;
	CCLog(">[BattleFied] TestPrior() | [PB] Ptr using with no check.Target may be cleaned");
	if(!acTar) acTar = meTarget->m_pChara;
	CCLog(">[BattleFied] TestPrior() | Begin Test.");

	int ret;
	ret = ((Chara*) acSrc)->getvalue("spd") - ((Chara*) acTar)->getvalue("spd");
	do 
	{
		if(ret > 5){
			ret = 1;
			break;
		}

		if(ret < -5){
			ret = -1;
			break;
		}

		ret = 0;
	} while (0);
	return ret;
}

/* 
	|< �׶�-BF0002��ʵ�ʷ�������ִ�У����ٶȵĲ�ֵ���м��
*/
bool BattleField::TestBackCh(EChesses* atar)
{
	do 
	{
		GameManager::sharedLogicCenter()->ml->m_rsb->SetContent(atar);
		if(atar->group_id == meOrig->group_id) return false;
		if(atar == meOrig){
			exit(0x999);
		}
		
		atar->m_pChara->miSeType = 3;

		meTarget = atar;	// <��ǰ����ΪĿ��Ľ�ɫ�趨
		meLastRe = atar;

		int delta_spd = (atar->m_pChara->getvalue("spd") - meOrig->m_pChara->getvalue("spd"));
		if( delta_spd >= 5){			// <�����������ٶȽϿ�
			miState = 2;
			CCLog(" ---- More One | BACK_TAR ----");
			CC_BREAK_IF(((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,2));		// <FindFitRe������Լ���״̬��miState�����޸�
		}else if(delta_spd <= -5){					// <������������ٶȽϿ�
			if(!mbIsMagic){
				miState = 3;
				CCLog(" ---- More One | BACK_SRC ----");
				if(((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,1)) break;
				else{				// <�Է��޷����з�����������ӳ�0.2�벢�ٴι���
					meSrc = atar;
					((EChessComp*) atar->getComponent("controller"))->DelayUnLock(0.2);
					return true;
				}
			}else{
				CC_BREAK_IF(((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,1));
			}


		}else{						// <�ٶ���ƽ�ⷶΧ�ڣ�����������ι���
			CC_BREAK_IF(((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,1));
		}
		

		// <RunScript()  --> Judge()
		return false;
	} while (0);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// <��ָ����Ŀ�굥λ����Ԥ���ж�
void BattleField::PreJudge(EChesses* atar)
{
	CCLog(">[BattleField] PreJudge() | Begin");
	if(!atar) return;
	meTarget = NULL;
	Chara* src = meOrig->m_pChara;
	Chara* tar = atar->m_pChara;

	//////////////////////////////////////////////////////////////////////////
	// <���������˺�Ԥ�� || ���ܵ��ж���ʽ�ڵ���������[mspVal]��
	{
		int hit_rate = src->getFixValue("hit")*5 + src->getFixValue("luk") - tar->getFixValue("avg") * 5 - tar->getFixValue("luk") + 70 + src->getFixRate(0);
		int hx_rate = (src->getFixValue("base_hit") + src->getFixValue("luk")/5 - iBossHuiXin[tar->getvalue("boss_class")])/100 + src->getFixRate(1);
		int damage =src->getFixValue("atk") + src->getvalue("a_atk");	

		GameManager::sharedLogicCenter()->ml->m_lsb->RefreshAct(hit_rate, hx_rate, damage);
	}


	do 
	{
		CCLog(">[BattleField]PreJudge before check target...");
		if(atar == BackChess1){
			break;
		}
		if(atar == BackChess2){
			break;
		}
		if(atar == BackChess3){
			break;
		}
		CCLog(">[BattleField]Check but no fit...");
		GameManager::sharedLogicCenter()->ml->m_rsb->setVisible(false);
		GameManager::sharedLogicCenter()->ml->Dissmiss_Arrows();
		return;
	} while (0);
	CCLog(">[BattleField]Check fits...");
	//////////////////////////////////////////////////////////////////////////
	// <�õ�λ�Ƿ�����λ�����
	// < �׶Σ�Ԥ�н�����ʾ��� || Ԥ��ʱû�н��з�Χ�ļ��
	
	//////////////////////////////////////////////////////////////////////////
	// < 1.��������Ԥ��
//	atar->;

	//////////////////////////////////////////////////////////////////////////
	// < 2.ִ����ʾ

	////////////////////////////////////////////////////////////////////////
	// <��ⲻ�ɴ����� || ��ǰ״̬Ϊ���ɴ�������ʾ���������һ�δ���û�����塣
	int aiu;
	switch(GameManager::sharedLogicCenter()->ml->m_iFuncType){
	case(8):			//type == 4 | using skill
		{
			//int a_type, vector<int> a_ran, CCPoint a_cp,CCPoint a)
			if(!GameManager::sharedLogicCenter()->ml->bm->f_Arange(GameManager::sharedLogicCenter()->ml->m_iSUseCase,atar)){
				aiu = 0;
			}else aiu = 1;
			break;
		}
	case 16:
		{
			if(!GameManager::sharedLogicCenter()->ml->bm->f_Arange(0,atar)){
				aiu = 0;
			}
			else aiu = 1;
			break;
		}
	}	


	//////////////////////////////////////////////////////////////////////////
	// <������ʾ�ļ�ͷ����ɫ��������ʹ�ü���ʱͬ����ʾ���Ƽ�ͷ��
	meTarget = atar;
	int delta_spd = (atar->m_pChara->getvalue("spd") - meOrig->m_pChara->getvalue("spd"));
	if( delta_spd >= 5){			// <�����������ٶȽϿ�
		//miState = 2;
		GameManager::sharedLogicCenter()->ml->Show_Arrows(1*aiu,((EChessComp*) atar->getComponent("controller"))->PreCheckRe(meOrig,2));
		//((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,2);		// <FindFitRe������Լ���״̬��miState�����޸�
	}else if(delta_spd <= -5){		// <������������ٶȽϿ�
		//miState = 3;
		GameManager::sharedLogicCenter()->ml->Show_Arrows(2*aiu,((EChessComp*) atar->getComponent("controller"))->PreCheckRe(meOrig,1));
		//((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,1);
	}else{							// <�ٶ���ƽ�ⷶΧ�ڣ�����������ι���
		GameManager::sharedLogicCenter()->ml->Show_Arrows(1*aiu,((EChessComp*) atar->getComponent("controller"))->PreCheckRe(meOrig,1));
		//((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,1);
	}
	CCLog(">[BattleField] PreJudge() | Prepare for right show.");

	{
		int hit_rate = tar->getFixValue("hit")*5 + tar->getFixValue("luk") - src->getFixValue("avg") * 5 - src->getFixValue("luk") + 70 + tar->getFixRate(0);
		int hx_rate = (tar->getFixValue("base_hit") + tar->getFixValue("luk")/5 - iBossHuiXin[src->getvalue("boss_class")])/100 + tar->getFixRate(1);
		int damage =tar->getFixValue("atk") + tar->getvalue("a_atk");

		GameManager::sharedLogicCenter()->ml->m_rsb->SetContent(meTarget);
		GameManager::sharedLogicCenter()->ml->m_rsb->setVisible(true);
		GameManager::sharedLogicCenter()->ml->m_rsb->RefreshAct(hit_rate, hx_rate, damage);
	}

	
}

void BattleField::setBattle( bool ab )
{
	mbIsInBattle = ab;
}
//////////////////////////////////////////////////////////////////////////
// <ս������
void BattleField::InitBfSp( const char* pSz )
{
	mSpLinker->re_init();
	mSpLinker->parse_file(pSz);

	mSpEvList = mSpLinker->mapscps;
	meConS = nullptr;
	//InitBF();
}

void BattleField::ClearBF()
{
	Clean();

	meConS = nullptr;
	mSpLinker->re_init();
	mMapC.clear();

	CC_SAFE_DELETE(mlp_Win);
	CC_SAFE_DELETE(mlp_Lose);
	//CC_SAFE_RELEASE_NULL(meTar);meorig

	mss_Win.clear();
	mss_Lose.clear();			// <��ֹmap�޶˵ı䳤
	msb_Win.clear();
	msb_Lose.clear();			// <ʤ������
}

BattleField::~BattleField()
{
	ClearBF();
	BSkillList::purgeSharedBSkillList();
	CC_SAFE_DELETE(mSpLinker);
}

void BattleField::ShowChess( EChesses* atar )
{
	bool ret;
	if(miState == -99) return;
	ret = (miState != 0 && miState < 3);

	if(ret) return;
	if(GameManager::sharedLogicCenter()->ml->mbSBProtect && (GameManager::sharedLogicCenter()->ml->mbNeedScroll || meConS)) return;

	if(!atar){
		GameManager::sharedLogicCenter()->ml->m_lsb->setVisible(false);
		GameManager::sharedLogicCenter()->ml->m_rsb->setVisible(false);
		return;
	}
	GameManager::sharedLogicCenter()->ml->m_lsb->SetContent(atar);
}

void BattleField::RefreshStats()
{
	CCLog(">[BattleField]  -- RefreshStats | Left --");
	if(meSrc) 
		GameManager::sharedLogicCenter()->ml->m_lsb->RefreshAll();
	//if(meTar->count()>0) 
	CCLog(">[BattleField]  -- RefreshStats | Right --");
	GameManager::sharedLogicCenter()->ml->m_rsb->RefreshAll();
	
}

void BattleField::ActionFac()
{
	Chara* t_owner = meSrc->m_pChara;

	t_owner->setvalue("b_xudong",t_owner->getvalue("b_xudong")-1);
	t_owner->plusvalue("b_actcout", 1);
	RefreshStats();
	GameManager::sharedLogicCenter()->ml->Dissmiss_Arrows();

	GameManager::sharedLogicCenter()->ml->mbNeedScroll = false;
	GameManager::sharedLogicCenter()->ml->bm->RemoveSCBKState();

	if(!mbIsInBattle) 	{
		if(meOrig->m_pChara->checkXuDong()){
			meOrig->setGrey(1);
		}
		miState = 0;
	}

	if(meOrig->m_pChara->checkXuDong()){
		meConS = NULL;
	}else{
		meConS = meOrig;
	}
	GameManager::sharedLogicCenter()->ml->mbSBProtect = false;
	
}

//////////////////////////////////////////////////////////////////////////
// <Judge�˺����㸨������


int DisTPoints(CCPoint src,CCPoint dst){
	CCLOG(">[BattleField]CDisTPoints:%d",abs(src.x - dst.x) + abs(src.y - dst.y));
	return (abs(src.x - dst.x) + abs(src.y - dst.y));
}

// < ���ʼ��㺯�������кͻ���
bool BattleField::CalRate(EChesses* tar_o,int hit_rate_base)
{
	do 
	{
		Chara* tar = tar_o->m_pChara;
		Chara* src = meSrc->m_pChara;

		float hit_rate = (( hit_rate_base - tar->getFixValue("avg") * 5 - tar->getFixValue("luk"))/100) + 0.7 + src->getFixRate(0)/100;
		CCLog(">[BattleField]MingZhong-%f",hit_rate);
		bool tbSingle = (CCRANDOM_0_1()<hit_rate);
		if(!tbSingle) {
			((EChesses*) tar_o)->miAvgFlag = 1;
			CCLog(">[BattleField]Miss....");
			break;
		}

		((EChesses*) tar_o)->miAvgFlag = 0;
		CCLog(">[BattleField]Hit");

		/* <��������� */
		hit_rate = (src->getFixValue("base_hit") + src->getFixValue("luk")/5.0 - iBossHuiXin[tar->getvalue("boss_class")])/100.0 + src->getFixRate(1)/100.0;
		CCLog(">[BattleField]HuiXing-%f",hit_rate);
		return (CCRANDOM_0_1()<hit_rate);
	} while (0);
	return false;
}

//////////////////////////////////////////////////////////////////////////
// <�Ƿ���ڵ�λ
bool BattleField::HasChess( int ax, int ay )
{
	CCLog(">[BattleFiled] HasChess(%d, %d)", ax, ay);
	bool ret = false;

	EChesses* tec = mMapC[make_pair(ax,ay)];
	if(tec){
		ret = true;
	}else{
		mMapC.erase(make_pair(ax,ay));
	}

	return ret;
}

void BattleField::setMeTar( CCArray* atar )
{
	if(!atar) return;
	meTar->removeAllObjects();
	CCLog(">[BattleField]Battle Field Receive the battle:%d",atar->count());
	EChesses* ttar = nullptr;
	for(int i = 0; i< atar->count(); ++i){

		ttar = ((EChesses*) atar->objectAtIndex(i));
		//EChessComp* ttecc = ((EChessComp*) (ttar->getComponent("controller")));
		//ttecc->miELock = 10000 + i;
		meTar->addObject(ttar);
	}

	meTarget = ttar;
}

//////////////////////////////////////////////////////////////////////////
// < ����

bool BattleField::fGetCont( const char* sname )
{
	bool ret = false;
	do 
	{
		string ts_skills = mspVals->getstring(sname);
		CC_BREAK_IF(ts_skills.length() == 0);

		CCLog(">[BattleField] Ctn Skill mask:%s",ts_skills.c_str());

		if(!meCtnTarget){
			GameManager::sharedGameManager()->sharedLogicCenter()->ml->SC_Popup(ts_skills.c_str());
		}else{
			CC_BREAK_IF(!PUSkillFilt(GameManager::sharedGameManager()->sharedLogicCenter()->ml->bm->m_controller, meCtnTarget, 1, ts_skills.c_str()));
		}
		ret = true;
	} while (0);
	return ret;
}

void BattleField::ActOver()
{
	CCLog(">[BatlleField] ActOver() | Action is over. %d", meTar->count());
	if(meOrig) 
	{
		((AIComponent*) meOrig->getComponent("ai"))->ActOver();
	
		meOrig->m_pChara->TurnOver();

		if(meOrig->m_pChara->checkXuDong()){
			meOrig->setGrey(1);
		}
	}else{
		
		//if(GameManager::sharedLogicCenter()->ml->mbAIControl){
		//	CCLog(">[BattleField] ActOver() | Manually Release.");
		//	GameManager::sharedLogicCenter()->ml->releaseControl();
		//}
	}
	miState = 0;
	GameManager::sharedLogicCenter()->ml->bm->clear_mcaTarget();
	mCachedSPp.clear();
	mCachedSPpScs.clear();
	miLastSkillID = 0;

	BackChess3 = nullptr;
	BackChess1 = nullptr;
	BackChess2 = nullptr;
	meLastRe = nullptr;

	for(map<pair<int,int>,EChesses*>::iterator it = mMapC.begin(); it != mMapC.end(); ++it){
		it->second->m_pChara->miSeType = 0;
	}

	
	//Clean();  <��ת�Ƶ�checkover�У������Ӱ��checkover�Ļ���
	
}

//////////////////////////////////////////////////////////////////////////

void BattleField::ClearTrigFlag()
{
	mbTrigged = false;
}

void BattleField::Trigged()
{
	if(mbTrigged) CCLog(">[BattleField] Trigged() | The Trigger is already trigged check your script. [ERROR]");
	mbTrigged = true;
}

bool BattleField::CheckSkill(CCObject* acSrc, CCObject* acTar, int sid)
{
	bool ret = false;
	do 
	{
		SkillJudge::sharedSkillJudge()->mbJudging = true;
		
		EChesses* teSrc = (EChesses*) acSrc;
		EChesses* teTar = (EChesses*) acTar;
		Chara* mpchara = teSrc->m_pChara;

		CCLog(">[BattleField] CheckSkill() | sid:%d", sid);

		GameManager::sharedLogicCenter()->ml->bm->clear_mcaTarget();
		GameManager::sharedLogicCenter()->ml->m_iFuncType = 8;
		GameManager::sharedLogicCenter()->ml->mbCheckLock = true;

		BSkill* tbs = mpchara->mSkillList->getSkill(sid);
		if(!tbs) break;

		int tiUseCase = tbs->m_iUseCase;
		GameManager::sharedLogicCenter()->ml->m_iSUseCase = tiUseCase;		
		GameManager::sharedLogicCenter()->ml->bm->miRangeType = tiUseCase;

		GameManager::sharedLogicCenter()->ml->bm->setLink(tbs->m_iLink);
		GameManager::sharedLogicCenter()->ml->BMSkDrawMove(tbs->msAddition);

		if(GameManager::sharedLogicCenter()->ml->mbIsDirect){
			CCLog(">[BattleField] CheckSkill() | Direct Skill.");
			//break;						// <[�趨] ֱ���ͷŵ����������Ƿ�ֱ�ӹ���
		}else{
			GameManager::sharedLogicCenter()->ml->bm->clear_Arrange();

			//////////////////////////////////////////////////////////////////////////
			int tiMax = 0;
			int tiX = 0;
			int tiY = 0;

			vector<pair<int,int>> tmp;
			tmp.push_back(make_pair(teTar->pos.x, teTar->pos.y));
			tmp.push_back(make_pair(teTar->pos.x+1, teTar->pos.y));
			tmp.push_back(make_pair(teTar->pos.x-1, teTar->pos.y));
			tmp.push_back(make_pair(teTar->pos.x, teTar->pos.y+1));
			tmp.push_back(make_pair(teTar->pos.x, teTar->pos.y-1));

			for(vector<pair<int,int>>::iterator it = tmp.begin();it != tmp.end(); ++it)
			{
				CCLog(">[BattleField] CheckSkill at (%d,%d).", it->first, it->second);
				if(it->first<1) continue;
				if(it->second<1) continue;
				if(!GameManager::sharedLogicCenter()->ml->bm->cs_y.count(make_pair(it->first, it->second))) continue;
				
				GameManager::sharedLogicCenter()->ml->bm->clear_Arrange();
				GameManager::sharedLogicCenter()->ml->bm->clear_mcaTarget();

				GameManager::sharedLogicCenter()->ml->bm->f_decide(it->first, it->second);
				if(GameManager::sharedLogicCenter()->ml->bm->testLink()){
					CCArray* tca = GameManager::sharedLogicCenter()->ml->bm->m_caTarget;
					if(!tca) continue;
					if(tca->indexOfObject(teTar) == UINT_MAX) continue;
					if(teTar->miHitGroup != 0 && !(teTar->miHitGroup & 3)) continue;
					CCLog(">[BattleField] CheckSkill() | sid:%d | hitgroup:%d", sid, teTar->miHitGroup);

					CCObject* tco = NULL;
					int ta =  0;

					CCARRAY_FOREACH(tca,tco){
						ta += (999 - ((EChesses*) tco)->m_pChara->gethp());
					}
					if(ta>tiMax){
						tiMax = ta;
						tiX = it->first;
						tiY = it->second;
					}
					//GameManager::sharedLogicCenter()->ml->bm->cs_y.erase(make_pair(it->first, it->second));
					break;
					//////////////////////////////////////////////////////////////////////////
				}
				//////////////////////////////////////////////////////////////////////////
				GameManager::sharedLogicCenter()->ml->bm->clear_mcaTarget();
				//GameManager::sharedLogicCenter()->ml->bm->cs_y.erase(make_pair(it->first, it->second));
			}
			

			if(tiMax == 0){
				for(set<pair<int,int>>::iterator it = GameManager::sharedLogicCenter()->ml->bm->cs_y.begin();it != GameManager::sharedLogicCenter()->ml->bm->cs_y.end(); ++it)
				{
					CCLog(">[BattleField] CheckSkill at (%d,%d).", it->first, it->second);
					if(it->first<1) continue;
					if(it->second<1) continue;

					GameManager::sharedLogicCenter()->ml->bm->clear_Arrange();
					GameManager::sharedLogicCenter()->ml->bm->clear_mcaTarget();

					GameManager::sharedLogicCenter()->ml->bm->f_decide(it->first, it->second);
					if(GameManager::sharedLogicCenter()->ml->bm->testLink()){
						CCArray* tca = GameManager::sharedLogicCenter()->ml->bm->m_caTarget;
						if(!tca) continue;
						if(tca->indexOfObject(teTar) == UINT_MAX) continue;
						if(teTar->miHitGroup != 0 && !(teTar->miHitGroup & 3)) continue;
						CCLog(">[BattleField] CheckSkill() | sid:%d | hitgroup:%d", sid, teTar->miHitGroup);

						CCObject* tco = NULL;
						int ta =  0;

						CCARRAY_FOREACH(tca,tco){
							ta += (999 - ((EChesses*) tco)->m_pChara->gethp());
						}
						if(ta>tiMax){
							tiMax = ta;
							tiX = it->first;
							tiY = it->second;
						}
						break;
						//////////////////////////////////////////////////////////////////////////
					}
					//////////////////////////////////////////////////////////////////////////
					GameManager::sharedLogicCenter()->ml->bm->clear_mcaTarget();
				}
			}

			GameManager::sharedLogicCenter()->ml->bm->clear_mcaTarget();

			if(tiMax <= 0) break;
			CCLog(">[SkillJudge] Skill Find with value:%d || %d,%d", tiMax, tiX, tiY);
			mCachedSPp.insert(make_pair(sid, ccp(tiX, tiY)));
			mCachedSPpScs.insert(make_pair(sid, tiMax));
			//////////////////////////////////////////////////////////////////////////

			CCLog(">[BattleField] CheckSkill() | Skill Can Go.");
		}

		ret = true;
	} while (false);
	GameManager::sharedLogicCenter()->ml->mbCheckLock = false;
	GameManager::sharedLogicCenter()->ml->bm->clean_allcs();
	SkillJudge::sharedSkillJudge()->mbJudging = false;
	GameManager::sharedLogicCenter()->ml->bm->clear_Arrange();
	return ret;
}

bool BattleField::PUSkillFilt(CCObject* acSrc, CCObject* acTar, int aiFlag, const char* asmask)
{
	CCLog(">[BattleField] PUSkillFilt()");

	bool ret = false;
	mCachedSPp.clear();
	mCachedSPpScs.clear();

	string tmsk = asmask;

	int ti;
	EChesses* teSrc = (EChesses*) acSrc;
	EChesses* teTar = (EChesses*) acTar;

	if(teSrc != meOrig){
		if(teSrc->group_id == teTar->group_id) return false;
	}

	if(!teSrc->m_pChara->CanAct()) return false;

	stringstream ts;
	GameManager::sharedLogicCenter()->ml->bm->m_controller = teSrc;

	for(int i=0;i<8;++i){
		ti = teSrc->m_pChara->m_viSkills[i];

		if(!ti) continue;
		if(tmsk.size() > 0 && tmsk.find(CCString::createWithFormat("%d",ti)->getCString()) == std::string::npos ) continue;
		if( !(teSrc->m_pChara->mSkillList->getSkill(ti)->miPhase & aiFlag) ) continue;
		int tval = DerLocks(teSrc, teTar, teSrc->m_pChara->mSkillList->getSkill(ti)->msSideEffect.c_str());
		if(!tval) continue;
		if(!CheckSkill(acSrc, acTar, ti)) continue;

		ts << ti;
		ts << ",";
		ret = true;
		CCLog(">[BattleField] Get Skill Out sid:%d || flag:%d.", ti, aiFlag);
		
	}

	if(ret){
		BattleField::sharedBattleField()->mbNeLock = true;
		((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRetain();

		if(teSrc != meOrig) GameManager::sharedLogicCenter()->ml->m_rsb->SetContent(teSrc);
		GameManager::sharedLogicCenter()->ml->SC_Popup(ts.str().c_str(), aiFlag);
		
	}

	return ret;
}

int BattleField::DerLocks(CCObject* acSrc, CCObject* acTar, const char* asLScp)
{
	int ret = 0;
	CCArray* t_caLocks;
	Scriptor* sp;

	do 
	{
		sp = new Scriptor();
		sp->parse_string(asLScp);
		t_caLocks = sp->m_caScript;
		sp->m_caScript = NULL;
		
		EffectControler::sharedEffectControler()->f_clear();
		EffectControler::sharedEffectControler()->m_caLocks = t_caLocks;
		EffectControler::sharedEffectControler()->m_src = ((EChesses*) acSrc)->m_pChara;

		if(!t_caLocks) break;
		t_caLocks->retain();
		if(!EffectControler::sharedEffectControler()->DerLock()) break;

		ret = 1;
		for(int i = 0; i<t_caLocks->count(); ++i){
			Script* t_scp = (Script*) t_caLocks->objectAtIndex(i);
			switch(t_scp->getint("type")){
			case(-1):		// <��鹥�������Ƿ�С��ĳ����ֵ | Ŀǰֻ��0����׼ȷ����� | �Ƿ��ǵ�һ�ι���
				{
					ret = ret&(miActCount <= t_scp->getint("value"));
					break;
				}
			case(-2):		// <�������Ƿ���Ե��� || ctype = 1
				{
					int cx = ((EChesses*) acSrc)->pos.x;
					int cy = ((EChesses*) acSrc)->pos.y;

					CCLog(">[BattleField] DerLocks() | moto:%d, %d", cx, cy);

					int dx = 1;	// <λ�þ���
					
					switch (((EChesses*) acSrc)->direc)
					{
					case MS_LEFT:
						{
							cx += dx;
							break;
						}
					case MS_RIGHT:
						{
							cx -= dx;
							break;
						}
					case MS_UP:
						{
							cy += dx;
							break;
						}
					case MS_DOWN:
						{
							cy -= dx;
							break;
						}
					default:
						break;
					}

					CCLog(">[BattleField] DerLocks() | after:%d, %d", cx, cy);

					ret &= !HasChess(cx, cy);											// <ȷ��û�е�λ
					ret &= GameManager::sharedLogicCenter()->ml->bm->InMap(cx, cy);		// <ȷ���ڿ�վ���ĵ�ͼ��
					break;
				}
			case -3:	// <���˫������
				{
					CCLog(">[BattleField] DerLocks() | Position Check. Begin.");
					if(!acTar) {
						CCLog(">[BattleField] DerLocks() | Position Check. Failed with Null Tar.");
						ret &= false;
						break;
					}
					int dis = abs(((EChesses*) acSrc)->pos.x - ((EChesses*) acTar)->pos.x) + abs(((EChesses*) acSrc)->pos.y - ((EChesses*) acTar)->pos.y);
					
					int tiMax = t_scp->getint("max");
					ret &= (dis <= tiMax);
					CCLog(">[BattleField] DerLocks() | Position Check. tiMax:%d, dis:%d", tiMax, dis);

					break;
				}
			}
		}

	} while (0);
	CC_SAFE_RELEASE_NULL(t_caLocks);
	sp->re_init();
	CC_SAFE_DELETE(sp);
	EffectControler::sharedEffectControler()->f_clear();
	return ret;

}

//void BattleField::CleanAllAct()
//{
//	//((EChessComp*) meSrc->getComponent("controller"))->ELock();
//	//if(meTar->count()>0){
//	//	CCObject* to;
//	//	CCARRAY_FOREACH(meTar, to){
//	//		EChesses* te = (EChesses*) to;
//	//		((EChessComp*) te->getComponent("controller"))->ELock();
//	//	}
//	//}
//
//	GameManager::sharedLogicCenter()->ml->bm->PauseAllActs();
//	GameManager::sharedLogicCenter()->ml->bm->cancontrol = true;
//	GameManager::sharedLogicCenter()->ml->bm->set_bbattle(6);
//}

/*
	|< ����·��  �û�������/�����Զ����� --> BattleMap::HandleScriptor --> EChessComp::RunScript
	|< �׶�-BF0001�����еڹ����ж����˺����㡣��Component���е��á�
		<Ҫ����״̬����LogicContinue
		<��ʽѡ��׶�
		<-[0b000001]�ͷż���֮ǰ���������ж�[����˹-���ơ���] 
		<-[0b100000]������Ч֮ǰ����[����-ɷն������ά˹-���С���]
*/
#define CHECK_GROUP																														\
CCLog(">[BattleField] check_group_match: %d,%d", agrp,  ((EChessComp*) ((EChesses*) tar_o)->getComponent("controller"))->getCurGrp());	\
if( !(((EChessComp*) ((EChesses*) tar_o)->getComponent("controller"))->getCurGrp() & agrp) && agrp) continue;							\
CCLog(">[PASS]");

void BattleField::Judge(Script* ascp){

	int agrp = ascp->getint("groupid");
	if(agrp < 0) 
		agrp = 2 << GameManager::sharedLogicCenter()->ml->bm->m_bSearch->getCount();

	CCObject* tar_o = NULL;
	//////////////////////////////////////////////////////////////////////////
	// <���� || ���ܵ������� | 	

	{
		int tiFlag = 2; // 0b0010
		int ti;
		int val = 0;
		EChesses* teTar;
		EChesses* winNer = NULL;
		stringstream ts;

		//map<EChesses*,string> tsCache;
		// <������ֶ���ɴ������ܵ���������ｫ���û���
		
		CCARRAY_FOREACH(meTar,tar_o){
			teTar = (EChesses*) tar_o;
			ts.clear();
			val = 0;

			for(int i=0;i<8;++i){
				ti = teTar->m_pChara->m_viSkills[i];

				if(!ti) continue;
				CHECK_GROUP;
				if( !(teTar->m_pChara->mSkillList->getSkill(ti)->miPhase & tiFlag) ) continue;
				int tval = DerLocks(tar_o, meSrc, teTar->m_pChara->mSkillList->getSkill(ti)->msSideEffect.c_str());
				if(tval){
					val = ti;
					ts << val;
					ts << ",";
					CCLog(">[BattleField] Get Skill Out , 0b00010");
				}
			}
			// <�ж�ȡ������
			if(val){
				mCachedJudge = ascp;
				CCLog(">[BattleField] -- Popup Call Begin -- | %s", ts.str().c_str());

				//GameManager::sharedLogicCenter()->ml->bm->m_controller = teTar;
				if(PUSkillFilt(teTar, meSrc, 2, ts.str().c_str())){
					CCLog(">[BattleField] -- Popup Call Over --");
					return;
				}

			}

		}
	}

	CCLog(">[BattleField] Judge() | PopUp Filt Over");
	//mCachedJudge = ascp;

	//CleanAllAct();
	//return;
	JudgeEnti(ascp);
}

//void BattleField::PunpOut()
//{
//	
//}

void BattleField::ResumeJudge()
{
	if(!mCachedJudge) return;

	JudgeEnti(mCachedJudge);
}

int BattleField::CalDamage(Script* asp){
	int ret = 0;
	do 
	{
		if(!asp) break;

		
		Chara* src = meSrc->m_pChara;
		stringstream ss;
		string ts;
		int	   ti;

		ti = asp->getint("damage");
		ts = asp->getstring("damages");
		if(ti > 0) return ti;

		CCLog(">[BattleField] CalDamage() | begin");
		ss << ts;
		CCLog(">[BattleField] CalDamage() | %s", ss.str().c_str());
		
		
		float  tf;
		ss >> ts;
		ss >> ti;	// <�����˺� | ��ɰ汾����
		ret += ti;

		ts.clear();
		ss >> ts;
		while(ts.size()>0){
			ti = src->getFixValue(ts);
			ss >> tf;
			ret += ti * tf;

			ts.clear();
			ss >> ts;
		}

	} while (false);
	return ret;
	
}

void BattleField::JudgeEnti(Script* ascp)
{
	// <  if(((EChesses*) tar_o)->miHitGroup == agrp) continue;  ������
	mCachedJudge = NULL;
	CCLog(">[BattleField] JudgeEnti()");
	meSrc->m_pChara->miSHitBClass = 0;

	int agrp = ascp->getint("groupid");
	int adam = CalDamage(ascp);
	int atyp = ascp->getint("atype");
	CCObject* tar_o = NULL;

	CCLog(">[BattleField] JudgeEnti() | adam:%d", adam);
	CCLog(">[BattleField] JudgeEnti() | agrp:%d", agrp);
	if(agrp < 0) 
		agrp = 2 << GameManager::sharedLogicCenter()->ml->bm->m_bSearch->getCount();

	CCLog(">[BattleField] JudgeEnti() | re:agrp:%d", agrp);

	//////////////////////////////////////////////////////////////////////////
	// <����

	int tiHitFlag = 0;
	Chara* src = meSrc->m_pChara;
	src->mbToKill = false;

	Chara* tar = NULL;
	int hurt = 0;
	//	bool tbSingle;
	int hit_rate_base = src->getFixValue("hit")*5 + src->getFixValue("luk");

	//////////////////////////////////////////////////////////////////////////
	if(miSFlag == 1){
		miSFlag = 2;
		//mbIsMagic = (mspVals!=NULL);
		CCLog(">[BattleField]Cached its a magic attack.Remove all !mspVal check for magick.");
	}

	//////////////////////////////////////////////////////////////////////////
	// <�°汾���˺�����

	// <����Typeֵ��type==0��Ϊ���������˺� || tiFlag Ϊ�������
	int tiFlag	 = 0;
	int tiDaType = 0;
	if(ascp){
		tiDaType = ascp->getint("atype");
	}

	if(meTar->count() > 0){
		// <��֧
		switch(tiDaType)
		{
		case 0:	// <�����˺�����
			{
				CCARRAY_FOREACH(meTar,tar_o){
					tar = ((EChesses*) tar_o)->m_pChara;
					CCLog(">[BatlleField] Judge Phisical Damage: %s", meSrc->name.c_str());
					hurt = 0;
					bool tbSingle = CalRate((EChesses*) tar_o,hit_rate_base);

					CCLog(">[BattleField] Before cal damage.");
					if(tbSingle){
						tiHitFlag |= 1;
						CCLog(">[BattleField]Extra Hurt.");
						hurt = src->getFixValue("atk") * 2.5 + src->getvalue("a_atk");
						meSrc->miHitFlag |= 1;
					}else{
						meSrc->miHitFlag |= 0;
						hurt = src->getFixValue("atk") + src->getvalue("a_atk");				// hurt = mspVals->getint("damage") + src->getvalue("mag") - tar->getvalue("rst");
					}
					/* <�����Ƿ���ҿ����������ᵯ�����ж� */
					// ....
					if(src == tar) exit(999);
					/*hurt -= tar->getFixValue("def");*/
					CCLog(">[BattleField]Physical Damage:%d",hurt);

					EGroup::sharedEGroup()->FactDamage(meSrc, tar_o, hurt, tiDaType);
				}


				break;
			}
		case 1:
			{
				
				CCARRAY_FOREACH(meTar,tar_o){

					CHECK_GROUP;

					tar = ((EChesses*) tar_o)->m_pChara;
					hurt = 0;
					hurt = hurt + adam /*+ src->getvalue("mag")*/; 
					
					//int a;
					//EChesses d;
					bool tbSingle = CalRate((EChesses*) tar_o,hit_rate_base);
					if(tbSingle){
						tiHitFlag |= 1;
						CCLog(">[BattleField]Extra Hurt.");
						hurt *= 1.3;
						meSrc->miHitFlag |= 1;
					}else{
						meSrc->miHitFlag |= 0;
					}

					/*hurt -= tar->getFixValue("rst");*/
					EGroup::sharedEGroup()->FactDamage(meSrc, tar_o, hurt, tiDaType);
				}
				break;
			}
		case 2:
			{
				map<pair<int,int>,EChesses*> fl_list;	// <��ɢ����

				int flam = ascp->getint("flam");	// <˥���뾶
				int radiu = ascp->getint("radiu"); // <��ɢ�뾶
				float fdec = ascp->getfloat("dec"); // <˥������

				CCARRAY_FOREACH(meTar,tar_o){
					CHECK_GROUP;

					hurt = 0;
					if(DisTPoints(meSrc->pos,((EChesses*) tar_o)->pos) < flam){
						hurt = hurt + adam /*+ src->getvalue("mag")*/; 

						bool tbSingle = CalRate((EChesses*) tar_o,hit_rate_base);
						if(tbSingle){
							tiHitFlag |= 1;
							CCLog(">[BattleField]Extra Hurt.");
							hurt *= 1.3;
							meSrc->miHitFlag |= 1;
						}else{
							meSrc->miHitFlag |= 0;
						}
						/*hurt -= ((EChesses*) tar_o)->m_pChara->getFixValue("rst");*/
						EGroup::sharedEGroup()->FactDamage(meSrc, tar_o, hurt, tiDaType);
						//exit(1);

						int mx = ((EChesses*) tar_o)->pos.x;
						int my = ((EChesses*) tar_o)->pos.y;

						int dx = mx - meSrc->pos.x;
						int dy = my - meSrc->pos.y;

						if(dx){ // <������չ
							dx = dx/abs(dx);
							for(int i = 0; i< radiu;){
								++i;
								fl_list[make_pair(mx+dx*i,my)] = ((EChesses*) tar_o);		// <�����ֱ���滻
								CCLog("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
							}
						}

						if(dy){ // <������չ
							dy = dy/abs(dy);
							for(int i = 0; i< radiu;){
								++i;
								fl_list[make_pair(mx,my+dy*i)] = ((EChesses*) tar_o);		// <�����ֱ���滻
								CCLog("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy");
							}
						}

					}else{
						((EChesses*) tar_o)->miDamage = -1;
					}
				}

				CCARRAY_FOREACH(meTar,tar_o){ // <��˥����Χ�Ľ�����������
					CHECK_GROUP;

					if(((EChesses*) tar_o)->miDamage == -1){
						EChesses* teS = fl_list[make_pair(((EChesses*) tar_o)->pos.x,((EChesses*) tar_o)->pos.y)];
						int ds = DisTPoints(teS->pos,((EChesses*) tar_o)->pos);
						((EChesses*) tar_o)->miDamage = teS->miDamage * (1-ds*fdec);
						((EChesses*) tar_o)->miAvgFlag = teS->miAvgFlag;
						CCLog(">[BattleField]FDec...");
					}

					((EChesses*) tar_o)->miDamage -= ((EChesses*) tar_o)->m_pChara->getFixValue("rst");
					CCLog(">[BattleField]Mageck Damage with rst:%d",((EChesses*) tar_o)->miDamage);

					EGroup::sharedEGroup()->FactDamage(meSrc, tar_o, ((EChesses*) tar_o)->miDamage, tiDaType);
				}

				break;
			}
		default:
			CCLog(">[BattleField]Judge Err:Error Type Provided:%d. Hurt will be 0.",tiDaType);
			break;
		}
	}else{
		exit(9999);
	}



	if(tiHitFlag != 0){
		meSrc->miHitFlag = 1;
	}else{
		meSrc->miHitFlag = 0;
	}

	PlaySe(meSrc->m_pChara);	// <���Կ��ܵ���Ч����
}

// <�������ڴ��ͷż���
// <����л���ű�ִ��״̬������������������µĵ���
void BattleField::ActSkill(CCObject* asrc, Script* asp, CCObject* atar)
{
	if(!atar){
		CCLog(">[BattleField] ActSkill() | Null Target Get. Exit.");
		exit(111);
		return;
	}

	EChesses* teTar = (EChesses*) ((Chara*) atar)->mEcOwner;
	EChesses* teSrc = (EChesses*) ((Chara*) asrc)->mEcOwner;
	int ti = asp->getint("id");

	if(mCachedChess){
		if(((EChesses*) mCachedChess)->m_pChara != asrc){
			CCLog(">[BattleField] ActSkill() | Error Scp. Echess And Chara miss match.[?]");
		}
	}

	CCLog(">[BattleField] ActSkill() | Begins.");
	//////////////////////////////////////////////////////////////////////////
	if(!DerLocks(teSrc, teTar, teSrc->m_pChara->mSkillList->getSkill(ti)->msSideEffect.c_str())) return;


	//////////////////////////////////////////////////////////////////////////
	GameManager::sharedLogicCenter()->ml->m_iFuncType = 8;
	BSkill* tbs = BSkillList::sharedBSkillList()->getSkill(ti);

	int tiUseCase = tbs->m_iUseCase;
	GameManager::sharedLogicCenter()->ml->m_iSUseCase = tiUseCase;		
	GameManager::sharedLogicCenter()->ml->bm->miRangeType = tiUseCase;

	GameManager::sharedLogicCenter()->ml->bm->setLink(tbs->m_iLink);
	GameManager::sharedLogicCenter()->ml->BMSkDrawMove(tbs->msAddition);

	CCLog(">[BattleField] ActSkill() | BMSkDrawMove over.");
	GameManager::sharedLogicCenter()->ml->bm->f_setcontroller( teSrc);
	GameManager::sharedLogicCenter()->ml->Dissmiss_Arrows();
	GameManager::sharedLogicCenter()->ml->bm->m_mou_cur = teTar->pos;		// <����λ��
	GameManager::sharedLogicCenter()->ml->bm->f_decide(teTar->pos.x, teTar->pos.y);


	// <�趨���ܱ�־λ || �ɽ����ļ��ܽ���洢��script��
	teSrc->m_pChara->cur_sid = asp->getint("id");
	teSrc->m_pChara->cur_shit = false;

	EffectControler::sharedEffectControler()->md_use_skill(GameManager::sharedLogicCenter()->ml,asp->getint("id"),teSrc->m_pChara);			// <[TODO]�����޸���ڵ㣬������EC�³����е�sp��

	GameManager::sharedLogicCenter()->ml->bm->set_bbattle(5);
	GameManager::sharedLogicCenter()->ml->bm->m_bAnimateOver = false;

	GameManager::sharedLogicCenter()->ml->bm->clean_allcs();

	Trigged();
}

int BattleField::getDisST()
{
	return abs(meSrc->pos.x - ((EChesses*) meTar->objectAtIndex(0))->pos.x) + abs(meSrc->pos.y - ((EChesses*) meTar->objectAtIndex(0))->pos.y);
}

#define MIN_DIS 160
void BattleField::PlaySpxEC(CCObject* atar, Script* asp)
{
	CCLog(">[BattleField] PlaySpxEC()");
#define filename asp->getstring("spx")
	CCSpriteFrameCache *cache = CCSpriteFrameCache::sharedSpriteFrameCache();
	GameManager::sharedLogicCenter()->f_cachetest(CCString::createWithFormat("%s.png",filename)->getCString());
	CCSpriteBatchNode *sheet = CCSpriteBatchNode::create(CCString::createWithFormat("%s.png",filename)->getCString());
	cache->addSpriteFramesWithFile(CCString::createWithFormat("%s.plist",filename)->getCString());

	CCSpriterX* t_animator = CCSpriterX::create(CCString::createWithFormat("%s.SCML",filename)->getCString());

	string aname = asp->getstring("name");
	int tiDirect = asp->getint("direct");

	switch (tiDirect)
	{
	case 1:		// <��mesrc����������
		{
			aname += "_";
			aname += sDirect[meSrc->direc];
			break;
		}
	case 0:		// <���ṩʱ�����㷽��
		{
			break;
		}
	default:
		break;
	}


	t_animator->setPosition(ccp(asp->getint("x") - 45,asp->getint("y")+92));
	t_animator->setAnchorPoint(CCPoint(0.5,0.5));
	t_animator->setRotation(asp->getfloat("angel"));

	float scalex = asp->getfloat("scalex");
	float scaley = asp->getfloat("scaley");

	if(scalex != 0) {
		t_animator->setScale(scalex);
	}
	int zorder = asp->getint("zorder");

	//t_animator->PlayAnim(asp->getstring("animate"),asp->getint("repeat"));
	sheet->addChild(t_animator);
	((EChesses*) atar)->addChild(sheet,asp->getint("zorder"));

	t_animator->runAction(CCSequence::create(CCDelayTime::create(asp->getfloat("delay")), CCHide::create(), CCRemoveSelf::create(), NULL));
	
	t_animator->PlayAnim(aname.c_str(),1);
	t_animator->SetNoLast(false);
#undef filename
}


void BattleField::PlayEffectSp(CCObject* echesscom, const char* asname)
{
	EChessComp* tec = (EChessComp*) echesscom;
	EChesses* te	= (EChesses*) tec->getOwner();

	CCPoint end = te->getPosition();
	CCPoint start = meSrc->getPosition();				

	CCLog(">[BattleField] PlayEffectSp() | Src: (%f, %f) | End: (%f, %f)", meSrc->pos.x, meSrc->pos.y, te->pos.x, te->pos.y);	
	CCLog(">[BattleField] PlayEffectSp() | [Begin: (%f,%f) | End: (%f,%f)]", start.x, start.y, end.x, end.y);

	start	= ccpAdd(start, ccp(20,16));
	end		= ccpAdd(end, ccp(20,16));

	CCSprite* tcs = CCSprite::create(asname);			// <�Ľ�ʱ������sp�и���asnameΪ��׼���в�ѯ
	tcs->setAnchorPoint(CCPointZero);
	tcs->setPosition(start);

	float tangel = std::atan2( start.y - end.y , end.x - start.x) * 180 / 3.14159;
	tcs->setRotation(tangel);

	int dis = abs(start.x - end.x) + abs(start.y - end.y);
	
	float tfdelay = 0.3;
	if(dis<MIN_DIS){
		tfdelay *= (dis/MIN_DIS);
	}
	tfdelay = max(0.1,tfdelay);

	CCLog(">[BattleField] PlayEffectSp() | Dis: %d", dis);
	CCLog(">[BattleField] PlayEffectSp() | Delay: %f", tfdelay);

	meSrc->getParent()->addChild(tcs);
	tec->ELock();
	tcs->runAction(CCSequence::create(CCMoveTo::create(tfdelay,end),CCRemoveSelf::create(),NULL));
	tcs->runAction(CCSequence::create(CCDelayTime::create(0), CCCallFunc::create(tec,callfunc_selector(EChessComp::EUnLock)), NULL));
}

void BattleField::PlayEffectSpFr(CCObject* tmpOwner, Script* sp)
{
	int tiType = sp->getint("operate");
	EChesses* tow = ((EChesses*) tmpOwner);
	string asname = sp->getstring("name");

	switch (tiType)
	{
	case 0:
		{
			CCPoint start = meSrc->getPosition();				// <λ�úͽǶ��д�����
			CCPoint end = tow->getPosition();

			start	= ccpAdd(start, ccp(20,16));
			end		= ccpAdd(end, ccp(20,16));

			CCSprite* tcs = CCSprite::createWithSpriteFrameName(asname.c_str());			// <�Ľ�ʱ������sp�и���asnameΪ��׼���в�ѯ
			tcs->setAnchorPoint(CCPointZero);
			tcs->setPosition(start);

			//////////////////////////////////////////////////////////////////////////
			float tangel = std::atan2( start.y - end.y , end.x - start.x) * 180 / 3.14159;
			int dis = abs(start.x - end.x) + abs(start.y - end.y);

			float tfdelay = sp->getfloat("delay");
			if(dis<MIN_DIS){
				tfdelay *= (dis/MIN_DIS);
			}
			tfdelay = max(0.1,tfdelay);
			tcs->setRotation(tangel);

			meSrc->getParent()->addChild(tcs);
			tcs->runAction(CCSequence::create(CCMoveTo::create(tfdelay,end),CCRemoveSelf::create(),NULL));
			break;
		}
	default:
		break;
	}

}

void BattleField::PlaySpxPo(CCPoint apos, Script* asp)
{
	CCLog(">[BattleField] PlaySpxEC()");
#define filename asp->getstring("spx")
	CCSpriteFrameCache *cache = CCSpriteFrameCache::sharedSpriteFrameCache();
	GameManager::sharedLogicCenter()->f_cachetest(CCString::createWithFormat("%s.png",filename)->getCString());
	CCSpriteBatchNode *sheet = CCSpriteBatchNode::create(CCString::createWithFormat("%s.png",filename)->getCString());
	cache->addSpriteFramesWithFile(CCString::createWithFormat("%s.plist",filename)->getCString());

	CCSpriterX* t_animator = CCSpriterX::create(CCString::createWithFormat("%s.SCML",filename)->getCString());
	int dx = asp->getint("dx");
	int dy = asp->getint("dy");

	string aname = asp->getstring("name");
	int tiDirect = asp->getint("direct");

	switch (tiDirect)
	{
	case 1:		// <��mesrc����������
		{
			aname += "_";
			aname += sDirect[meSrc->direc];
			break;
		}
	case 0:		// <���ṩʱ�����㷽��
		{
			break;
		}
	default:
		break;
	}


	t_animator->setPosition(ccpAdd(apos, ccp(dx, dy)));
	t_animator->setAnchorPoint(CCPoint(0.5,0.5));
	t_animator->setRotation(asp->getfloat("angel"));

	float scalex = asp->getfloat("scalex");
	float scaley = asp->getfloat("scaley");

	if(scalex != 0) {
		t_animator->setScale(scalex);
	}
	int zorder = asp->getint("zorder");

	//t_animator->PlayAnim(asp->getstring("animate"),asp->getint("repeat"));
	sheet->addChild(t_animator);
	
	GameManager::sharedLogicCenter()->ml->bm->addChild(sheet);

	t_animator->runAction(CCSequence::create(CCDelayTime::create(asp->getfloat("delay")), CCHide::create(), CCRemoveSelf::create(), NULL));

	t_animator->PlayAnim(aname.c_str(),asp->getint("repeat"));
	t_animator->SetNoLast(false);
#undef filename
}

void BattleField::PlaySe(CCObject* aeChara)
{
	Chara* tc = ((Chara*) aeChara);
	CCLog(">[BattleField] PlaySe | %d",  tc->miSeType);
	switch (tc->miSeType)
	{
	case 1:	/* <��ͨ���� */
		{
			if(!mbCurMagic){
				tc->PlayToKill();
				tc->PlayAttack();

			}/*else{
				CCLog(">[BattleField] PlaySe | Error xxx | Play Se for na but get a skill.");
				tc->PlaySESkill(miLastSkillID);
			}*/
			
			break;
		}
	case 2: /* <�����ι��� */
		{
			tc->PlayHpStat();
			tc->PlayToKill();

			if(!mbCurMagic){
				if(meOrig->m_pChara == aeChara){	/* <���������ߵĹ��� */
					tc->PlayReAtk();
					tc->PlayAttack();
				}else{	/* <�������ߵķ��� */
					tc->PlayReAtk();
					tc->PlayAttack();
				}
			}/*else{
				tc->PlaySESkill(miLastSkillID);
			}*/

			break;
		}
	case 3:	/* <���� */
		{
			tc->PlayHpStat();
			tc->PlayToKill();

			if(!mbCurMagic){
				tc->PlayReAtk();
				tc->PlayAttack();
			}/*else{
				tc->PlaySESkill(miLastSkillID);
			}*/
			break;
		}
	case 4:
		{
			tc->PlayHpStat();
			tc->PlaySESkill(miLastSkillID);
			break;
		}
	default:
		return;
		break;
	}
	tc->miSeType = 0;
	miLastSkillID = 0;
	tc->PlayVodActor();
}
