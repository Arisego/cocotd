#include "SingleTon/BattleFiled.h"
#include "packui/CharaS.h"
#include "GameManager.h"

static const int iBossHuiXin[] = {0,3,6,10,15};		// None C - S | 0 -4


//////////////////////////////////////////////////////////////////////////

int DisTPoints(CCPoint src,CCPoint dst);

//////////////////////////////////////////////////////////////////////////

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
		meTar = new CCArray();
		Clean();
		mSpLinker = new Scriptor();

		return true;
	} while (false);
	return false;
}

void BattleField::Clean()
{
	mbIsOver = false;

	mspVals = NULL;
	meSrc = NULL;
	meTar->removeAllObjects();

	//mMapC.clear();

	//BackChess1 = NULL;
	//BackChess2 = NULL;
	//BackChess3 = NULL;

	mbIsInBattle = false;
	//miState = 0;
	//miEnemy = 0;
}

void BattleField::CheckOver()
{
	if(meTar->count()==0) return;
	do 
	{
		//CC_BREAK_IF(meTar->count() == 0);


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
	CCLog(">[BF]Battle Field Receive the battle:%d",aTar->count());

	if(sp=NULL) 
		CCLog(">[BF]Battle with an empty values....");
	else
	{
		CCLog(">[BF]Battle Field Prepare: Skill");
	}
}

void BattleField::SetSrc( EChesses* aSrc )
{
	if(!aSrc) {
		miSFlag = 0;
		return;
	}
	Clean();
	meSrc = aSrc;
	meOrig = aSrc;
	miState = 0;
	/* [SS] <Ŀ�걻ѡ��Ϊ������λ  */
	meOrig->m_pChara->PlaySS();
	GameManager::sharedLogicCenter()->ml->m_lsb->SetContent(meSrc);
	miSFlag = 1;
}


void BattleField::SetTars( CCArray* aTar )
{
	if(!aTar) {
		GameManager::sharedLogicCenter()->ml->m_rsb->setVisible(false);
		GameManager::sharedLogicCenter()->ml->Dissmiss_Arrows();
		GameManager::sharedLogicCenter()->ml->m_lsb->SetNullAct();
		return;
	}
	setMeTar(aTar);
	if(!mbIsInBattle) {
		return;
	}
	meTarget = NULL;
	CheckBackCh();
	PreJudge((EChesses*) aTar->objectAtIndex(0));
	
	miState = 0;
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
}

void BattleField::InitChessRefreh()
{
	EChesses* te;
	EChesses* ste;
	int tiLead;
	// <��ɢͳ˧���� 
	for(map<pair<int,int>,EChesses*>::iterator it = mMapC.begin(); it != mMapC.end(); ++it){
		te = it->second;
		
		CCLog(">[BF]Initing the chess lead values...");

		for(map<pair<int,int>,EChesses*>::iterator sit = mMapC.begin(); sit != mMapC.end(); ++sit){
			ste = sit->second;
			tiLead = ste->m_pChara->getLead();
			te->m_pChara->AddLead(DerLead(tiLead,it->first.first,it->first.second,sit->first.first,sit->first.second));
		}

	}

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
	mMapC.erase(make_pair(astart.x,astart.y));
	mMapC.insert(make_pair(make_pair(aend.x,aend.y),ae));

	// <�Ƴ����滻���е�ͳ˧���
	UnDerLead(tiLead,astart.x,astart.y);
	DerLead(tiLead,aend.x,aend.y);

	// <�Ƴ��������õ���ͳ˧�ӳ�
	ae->m_pChara->ClearLead();

	EChesses* ste;
	CCLog(">[BF]Initing the chess lead values...");

	// <����µ�ͳ˧�ӳ�
	for(map<pair<int,int>,EChesses*>::iterator sit = mMapC.begin(); sit != mMapC.end(); ++sit){
		ste = sit->second;
		tiLead = ste->m_pChara->getLead();
		ae->m_pChara->AddLead(DerLead(tiLead,aend.x,aend.y,sit->first.first,sit->first.second));
	}
	
}

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


/*
	|< ����·��  �û�������/�����Զ����� --> BattleMap::HandleScriptor --> EChessComp::RunScript
	|< �׶�-BF0001�����еڹ����ж����˺����㡣��Component���е��á�
		<Ҫ����״̬����LogicContinue
		<��ʽѡ��׶�
		<-[0b000001]�ͷż���֮ǰ���������ж�[����˹-���ơ���] 
		<-[0b100000]������Ч֮ǰ����[����-ɷն������ά˹-���С���]
*/
void BattleField::Judge(){
	//////////////////////////////////////////////////////////////////////////
	// <����

	int tiHitFlag = 0;
	Chara* src = meSrc->m_pChara;
	CCObject* tar_o = NULL;
	Chara* tar = NULL;
	int hurt = 0;
	bool tbSingle;
	int hit_rate_base = src->getFixValue("hit")*5 + src->getFixValue("luk");

	//////////////////////////////////////////////////////////////////////////
	if(miSFlag == 1){
		miSFlag = 2;
		mbIsMagic = (mspVals!=NULL);
		CCLog(">[BF]Cached its a magic attack.Remove all !mspVal check for magick.");
	}

	//////////////////////////////////////////////////////////////////////////
	// <�°汾���˺�����

	// <����Typeֵ��type==0��Ϊ���������˺� || tiFlag Ϊ�������
	int tiFlag	 = 0;
	int tiDaType = 0;
	if(mspVals){
		tiDaType = mspVals->getint("type");
	}
	
	if(meTar->count() > 0){
		// <��֧
		switch(tiDaType)
		{
		case 0:	// <�����˺�����
			{
				CCARRAY_FOREACH(meTar,tar_o){
					tar = ((EChesses*) tar_o)->m_pChara;
					hurt = 0;
					bool tbSingle = CalRate((EChesses*) tar_o,hit_rate_base);
					if(tbSingle){
						tiHitFlag |= 1;
						CCLog(">[BF]Extra Hurt.");
						hurt = src->getFixValue("atk") * 2.5 + src->getFixValue("a_atk");
						meSrc->miHitFlag |= 1;
					}else{
						meSrc->miHitFlag |= 0;
						hurt = src->getFixValue("atk") + src->getFixValue("a_atk");				// hurt = mspVals->getint("damage") + src->getvalue("mag") - tar->getvalue("rst");
					}
					/* <�����Ƿ���ҿ����������ᵯ�����ж� */
					// ....
					hurt -= tar->getFixValue("def");
					CCLog(">[BF]Physical Damage:%d",hurt);
				}

				((EChesses*) tar_o)->miDamage = hurt;
				break;
			}
		case 1:
			{
				CCARRAY_FOREACH(meTar,tar_o){
					tar = ((EChesses*) tar_o)->m_pChara;
					hurt = hurt + mspVals->getint("damage") + src->getvalue("mag"); 
					//int a;
					EChesses d;
					bool tbSingle = CalRate((EChesses*) tar_o,hit_rate_base);
					if(tbSingle){
						tiHitFlag |= 1;
						CCLog(">[BF]Extra Hurt.");
						hurt *= 1.3;
						meSrc->miHitFlag |= 1;
					}else{
						meSrc->miHitFlag |= 0;
					}

					hurt -= tar->getFixValue("rst");
					((EChesses*) tar_o)->miDamage = hurt;
				}
				break;
			}
		case 2:
			{
				map<pair<int,int>,EChesses*> fl_list;	// <��ɢ����

				int flam = mspVals->getint("flam");	// <˥���뾶
				int radiu = mspVals->getint("radiu"); // <��ɢ�뾶
				float fdec = mspVals->getfloat("dec"); // <˥������

				CCARRAY_FOREACH(meTar,tar_o){
					hurt = 0;
					if(DisTPoints(meSrc->pos,((EChesses*) tar_o)->pos) < flam){
						hurt = hurt + mspVals->getint("damage") + src->getvalue("mag"); 

						bool tbSingle = CalRate((EChesses*) tar_o,hit_rate_base);
						if(tbSingle){
							tiHitFlag |= 1;
							CCLog(">[BF]Extra Hurt.");
							hurt *= 1.3;
							meSrc->miHitFlag |= 1;
						}else{
							meSrc->miHitFlag |= 0;
						}
						((EChesses*) tar_o)->miDamage = hurt;

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

				CCARRAY_FOREACH(meTar,tar_o){ // <��˥����Χ��Ľ�����������
					if(((EChesses*) tar_o)->miDamage == -1){
						EChesses* teS = fl_list[make_pair(((EChesses*) tar_o)->pos.x,((EChesses*) tar_o)->pos.y)];
						int ds = DisTPoints(teS->pos,((EChesses*) tar_o)->pos);
						((EChesses*) tar_o)->miDamage = teS->miDamage * (1-ds*fdec);
						((EChesses*) tar_o)->miAvgFlag = teS->miAvgFlag;
						CCLog(">[BF]FDec...");
					}

					((EChesses*) tar_o)->miDamage -= ((EChesses*) tar_o)->m_pChara->getFixValue("rst");
					CCLog(">[BF]Mageck Damage with rst:%d",((EChesses*) tar_o)->miDamage);
				}

				break;
			}
		default:
			CCLog(">[BF]Judge Err:Error Type Provided:%d. Hurt will be 0.",tiDaType);
			break;
		}
	}


	
	if(tiHitFlag != 0){
		meSrc->miHitFlag = 1;
	}else{
		meSrc->miHitFlag = 0;
	}
	
}

void BattleField::PlayEffectSp( const char* asname, CCPoint end )
{
	CCPoint start = meSrc->getPosition();				// <λ�úͽǶ��д�����

	CCSprite* tcs = CCSprite::create(asname);			// <�Ľ�ʱ������sp�и���asnameΪ��׼���в�ѯ
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
	case 0:		// <��һ�ι��ؽ���
	case 1:		// <������ݶ�ȡ
		{
			//if(!mspVals){		// <û�в����ű�ʱ��Ϊ��ͨ�����Դ� <<<< ���й��������Է���
				ret = NormalAttackC();				
			//}
			break;
		}
	case 3:		// <�������𷽿ɷ��������
		{
			if(!mbIsMagic){		
				// <�����ι���ֻ�п�������ͨ����
				GameManager::sharedLogicCenter()->ml->bm->m_caTarget->removeAllObjects();
				GameManager::sharedLogicCenter()->ml->bm->m_caTarget->addObject(meSrc);
				GameManager::sharedLogicCenter()->ml->bm->m_controller = meOrig;
				GameManager::sharedLogicCenter()->ml->RePack();
				
				miState = 1;			// <�ص���ʼ������ѯ״̬
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
		// <������ѡ��
		if(meTar->count()==0) 
			return false;
		CheckBackCh();

		// <����ѡ�񽫽���EChessComp����
		/*
			<miState == 2 ��ζ�ŵз�Ҫ�������� [BackChess1û���ÿջ��ٴ�ִ�м��]
			
			<miState == 3 �ҷ��ɷ���
			<miState == 1 ֱ�ӽ�����һ�ּ��
		*/
		if(BackChess1){
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

/* <ʱ��Σ����㷴����λ-ֻ��һ��ʼ����һ�� */
void BattleField::CheckBackCh()
{
	if(miState >= 1) return;
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
	CCLog(">[BF]Get ReBack Chess Over with prior:%d,%d,%d",m3,m2,m1);
}

/* 
	|< �׶�-BF0002��ʵ�ʷ�������ִ�У����ٶȵĲ�ֵ���м��
*/
bool BattleField::TestBackCh(EChesses* atar)
{
	do 
	{
		int delta_spd = (atar->m_pChara->getvalue("spd") - meOrig->m_pChara->getvalue("spd"));
		if( delta_spd >= 5){			// <�����������ٶȽϿ�
			miState = 2;
			CC_BREAK_IF(((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,2));		// <FindFitRe������Լ���״̬��miState�����޸�
		}else if(delta_spd <= -5){					// <������������ٶȽϿ�
			if(!mbIsMagic){
				miState = 3;
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
	if(!atar) return;
	meTarget = NULL;
	Chara* src = meOrig->m_pChara;
	Chara* tar = atar->m_pChara;

	//////////////////////////////////////////////////////////////////////////
	// <���������˺�Ԥ�� || ���ܵ��ж���ʽ�ڵ���������[mspVal]��
	int hit_rate = src->getFixValue("hit")*5 + src->getFixValue("luk") - tar->getFixValue("avg") * 5 - tar->getFixValue("luk") + 70 + src->getFixRate()*100;
	int hx_rate = (src->getFixValue("base_hit") + src->getFixValue("luk")/5 - iBossHuiXin[tar->getvalue("boss_class")])/100 + src->getFixRate();
	int damage =src->getFixValue("atk") + src->getFixValue("a_atk");	

	GameManager::sharedLogicCenter()->ml->m_lsb->RefreshAct(hit_rate, hx_rate, damage);

	do 
	{
		//CCLog(">[BF]PreJudge before check target...");
		if(atar == BackChess1){
			break;
		}
		if(atar == BackChess2){
			break;
		}
		if(atar == BackChess3){
			break;
		}
		//CCLog(">[BF]Check but no fit...");
		GameManager::sharedLogicCenter()->ml->m_rsb->setVisible(false);
		GameManager::sharedLogicCenter()->ml->Dissmiss_Arrows();
		return;
	} while (0);
	//CCLog(">[BF]Check fits...");
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
	case(4):			//type == 4 | using skill
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
		miState = 2;
		GameManager::sharedLogicCenter()->ml->Show_Arrows(1*aiu,((EChessComp*) atar->getComponent("controller"))->PreCheckRe(meOrig,2));
		//((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,2);		// <FindFitRe������Լ���״̬��miState�����޸�
	}else if(delta_spd <= -5){		// <������������ٶȽϿ�
		miState = 3;
		GameManager::sharedLogicCenter()->ml->Show_Arrows(2*aiu,((EChessComp*) atar->getComponent("controller"))->PreCheckRe(meOrig,1));
		//((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,1);
	}else{							// <�ٶ���ƽ�ⷶΧ�ڣ�����������ι���
		GameManager::sharedLogicCenter()->ml->Show_Arrows(1*aiu,((EChessComp*) atar->getComponent("controller"))->PreCheckRe(meOrig,1));
		//((EChessComp*) atar->getComponent("controller"))->FindFitRe(meOrig,1);
	}
	GameManager::sharedLogicCenter()->ml->m_rsb->SetContent(meTarget);
	
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
}

void BattleField::ClearBF()
{
	mSpLinker->re_init();
}

BattleField::~BattleField()
{
	mSpLinker->re_init();
	CC_SAFE_DELETE(mSpLinker);
}

void BattleField::ShowChess( EChesses* atar )
{
	if(!atar){
		GameManager::sharedLogicCenter()->ml->m_lsb->setVisible(false);
		return;
	}
	GameManager::sharedLogicCenter()->ml->m_lsb->SetContent(atar);
}

void BattleField::RefreshStats()
{
	if(meSrc) GameManager::sharedLogicCenter()->ml->m_lsb->RefreshAll();
	if(meTar->count()>0) GameManager::sharedLogicCenter()->ml->m_rsb->RefreshAll();
	
}

void BattleField::ActionFac()
{
	Chara* t_owner = meSrc->m_pChara;

	t_owner->setvalue("b_xudong",t_owner->getvalue("b_xudong")-1);
	RefreshStats();
	GameManager::sharedLogicCenter()->ml->Dissmiss_Arrows();
}

//////////////////////////////////////////////////////////////////////////
// <Judge�˺����㸨������


int DisTPoints(CCPoint src,CCPoint dst){
	CCLOG(">[BF]CDisTPoints:%d",abs(src.x - dst.x) + abs(src.y - dst.y));
	return (abs(src.x - dst.x) + abs(src.y - dst.y));
}

// < ���ʼ��㺯�������кͻ���
bool BattleField::CalRate(EChesses* tar_o,int hit_rate_base)
{
	do 
	{
		Chara* tar = tar_o->m_pChara;
		Chara* src = meSrc->m_pChara;

		float hit_rate = (( hit_rate_base - tar->getFixValue("avg") * 5 - tar->getFixValue("luk"))/100) + 0.7 + src->getFixRate();
		CCLog(">[BF]MingZhong-%f",hit_rate);
		bool tbSingle = (CCRANDOM_0_1()<hit_rate);
		if(!tbSingle) {
			((EChesses*) tar_o)->miAvgFlag = 1;
			CCLog(">[BF]Miss....");
			break;
		}

		((EChesses*) tar_o)->miAvgFlag = 0;
		CCLog(">[BF]Hit");

		/* <��������� */
		hit_rate = (src->getFixValue("base_hit") + src->getFixValue("luk")/5 - iBossHuiXin[tar->getvalue("boss_class")])/100 + src->getFixRate();
		CCLog(">[BF]HuiXing-%f",hit_rate);
		return (CCRANDOM_0_1()<hit_rate);
	} while (0);
	return false;
}

//////////////////////////////////////////////////////////////////////////
// <�Ƿ���ڵ�λ
bool BattleField::HasChess( int ax, int ay )
{
	return mMapC.count(make_pair(ax,ay)) > 0;
}

void BattleField::setMeTar( CCArray* atar )
{
	meTar->removeAllObjects();

	CCObject* to;
	CCARRAY_FOREACH(atar,to){
		meTar->addObject(to);
	}
}
