#include "BBuffer.h"
#include "BTrigger.h"
#include "GameManager.h"
#include "SingleTon\BattleFiled.h"
#include "SingleTon\EGroup.h"
#include "SingleTon\SkillJudge.h"

#define ST_VALS		1
#define ST_NATK		2
#define ST_VCHG		3	/* <��ֵ�ı� | ��Ѫ|��ȡ */
#define ST_MATK		4	/* <�ػ�����һ�� */
#define ST_REOVE	-1	/* <�Ƴ�Buffer */
#define ST_RES		-2	/* <�ı�State��ֵ */
#define ST_SKILL	5	/* <ʹ�ü��� | ����meSrc | ����������� */
#define ST_COUNT	6	/* <������ز��� */
#define ST_PURE		7	/* <����Ĵ��� */
#define ST_CMIS		8	/* <ǿ���޸�����ֵ */

#define ST_HALO		9	/* <��ӹ⻷ */
#define ST_DEATH	10	/* <ǿ������ | EGroup->FactDama��Դ����ʹ��| TR_SRDAMA | */
#define ST_NODEA	11  /* <��ɱ|ʹ������λ����һ��HP */
#define ST_MAGL		12  /* <��ʦ������ */

#define ST_ANIMA	13	/* <����ָ���Ķ��� */

BBuffer::~BBuffer()
{
	
	misStates.clear();
	for(map<int,CCObject*>::iterator it = mTrigerCache.begin(); it != mTrigerCache.end(); ++it){
			mpChara->removeTrig(it->first, (BTRigger*) it->second);
	}
	mTrigerCache.clear();
	miscTrigger.clear();
	CCLog(">[BBuffer] ~BBuffer()");
	CC_SAFE_DELETE(mscp);
}

void BBuffer::Trig(int ai)
{
	CCLog(">[BBuffer] Trig(%d)", ai);
	//miState = 1;	// <��ʱ�� | ���ӻ����޸Ĵ˴�

	Script* tsp = miscTrigger[ai];
	if(!tsp){
		CCLog(">[BBuffer] Trig() | No script match.");
		return;
	}
	int tis = tsp->getint("tar");

	Script* stsp = NULL;
	if(tis == 0){
		int tiye = tsp->getint("val");	// <���
		CCLog(">[BBuffer] Trig() | Check for flags. bid:%d", miId);

		if(tiye & 1){	// <HP���
			float tr = mpChara->gethp()/mpChara->getvalue("hp");
			for(int i = 0; i< tsp->m_snum; ++i){
				stsp = (Script*) tsp->scriptnodes->objectAtIndex(i);
				if(tr < stsp->getfloat("gate")){
					tis = stsp->getint("tar");
					CCLog(">[BBuffer] Trig() | Check and match.");
					break;
				}
			}
			if(!tis) return;
		}

		if(tiye & 2){	// <State���

			if(tis){
				if(stsp->getint("val") != miState && miState) return;	// <����ǲ�ָ����ֵ��������������||�Ѿ�ָ����ֵ������
			}else{
				for(int i = 0; i< tsp->m_snum; ++i){
					stsp = (Script*) tsp->scriptnodes->objectAtIndex(i);
					if(stsp->getint("val") == miState || !miState){		// <���ڲ�ָ����ֵ��State���ͣ������û�������
						tis = stsp->getint("tar");
						break;
					}
				}
			}



		}

		if(!tis) return;

		CCLog(">[BBuffer] Trig() | Check for flags. bid:%d - Pass:%d", miId, tis);
		//((EChesses*) mpChara->mEcOwner)->ShowTestF(stsp->getstring("text"), 3);
	}
	miState = tis;

	tsp = misStates[tis];
	
	for(int i =0; i<tsp->m_snum; ++i)
	{
		stsp = (Script*) tsp->scriptnodes->objectAtIndex(i);
		// <�����Ҫ���Ŷ���������µ�һ��ṹ
		int stis = stsp->getint("type");
		CCLog(">[Buffer] Acting BType:%d", stis);

		switch (stis)
		{
		case ST_REOVE:	// <����buffer
			{
				mpChara->RemoveMAttrs(miAttrs);
				miAttrs.clear();
				mpChara->RemoveBuffer(miId);
			}
		case 0:	// <ǿ�Ƴ���
			{
				mpChara->RemoveMAttrs(miAttrs);
				miAttrs.clear();
				break;
			}
		case ST_VALS:			// <������ֵ���
			{
				//BattleField::sharedBattleField()->Trigged();
				if(CCRANDOM_0_1() < stsp->getfloat("rate")) break;
				mpChara->RemoveMAttrs(miAttrs);
				RefreshAttrs(stsp->getstring("vals"));
				mpChara->AddonMAttrs(miAttrs);
				mpChara->checkVals();
				((EChesses*) mpChara->mEcOwner)->ShowTestF(stsp->getstring("text"), stsp->getint("color"));
				
				break;
			}
		case ST_NATK:			// <׷��һ����ͨ���� | ����
			{
				if(CCRANDOM_0_1() < stsp->getfloat("rate")) break;
				
				CCLog(" ---- More One | ST_NATK|Repack ----");
				//GameManager::sharedLogicCenter()->ml->RePack();
							
				//SkillJudge::sharedSkillJudge()->CheckNorM(mpChara->mEcOwner, ((EChessComp*) ((EChesses*) mpChara->mEcOwner)->getComponent("controller"))->getSrc(), true);
				if(EGroup::sharedEGroup()->AtkPlay(stsp, mpChara->mEcOwner)){
					BattleField::sharedBattleField()->Trigged();
					((EChesses*) mpChara->mEcOwner)->ShowTestF(stsp->getstring("text"), stsp->getint("color"));
				}
				break;
			}
		case ST_MAGL:			// <��ʦ����
			{
				if(CCRANDOM_0_1() < stsp->getfloat("rate")) break;
				if(EGroup::sharedEGroup()->AtkSkill(stsp, mpChara->mEcOwner)){
					BattleField::sharedBattleField()->Trigged();
					((EChesses*) mpChara->mEcOwner)->ShowTestF(stsp->getstring("text"), stsp->getint("color"));
				}
				break;
			}
		case ST_VCHG:
			{
				if(EGroup::sharedEGroup()->ValChange(stsp)){
					((EChesses*) mpChara->mEcOwner)->ShowTestF(stsp->getstring("text"), stsp->getint("color"));
				}
				break;
			}
		case ST_MATK:
			{
				if(CCRANDOM_0_1() < stsp->getfloat("rate")) break;
				CCLog(" ---- More One | ST_MATK ----");
				
				if(EGroup::sharedEGroup()->AtkPlay(stsp, mpChara->mEcOwner)){
					((EChesses*) mpChara->mEcOwner)->ShowTestF(stsp->getstring("text"), stsp->getint("color"));
				}
				
				break;
			}
		case ST_RES:
			{
				miState = stsp->getint("value");
				break;
			}
		case ST_SKILL:
			{
				CCLog(">[BBuffer] ActSkill begin");
				((EChesses*) mpChara->mEcOwner)->ShowTestF(stsp->getstring("text"), stsp->getint("color")); // <Debug
				BattleField::sharedBattleField()->ActSkill(mpChara, stsp, mcTarget);
				CCLog(">[BBuffer] ActSkill over");
				break;
			}
		case ST_COUNT:
			{
				int ti = stsp->getint("operate");
				CCLog(">[BBuffer] Count Down: %d| CD:%d", ti, miCountDown);
				switch (ti)
				{
				case 0:		// <�趨����ֵ | һ�㲻���õ�
					{
						miCountDown = stsp->getint("val");
						break;
					}
				case 1:		// <����һ�� | ��0��...
					{
						--miCountDown;
						if(!miCountDown) Trig(TR_COUTDOWN);		// <���Ͳ㼶����������BUG
						break;
					}

				}
				break;
			}
		case ST_PURE:
			{
				if(CCRANDOM_0_1() < stsp->getfloat("rate")) break;
				((EChesses*) mpChara->mEcOwner)->ShowTestF(stsp->getstring("text"), stsp->getint("color"));
				BattleField::sharedBattleField()->Trigged();
				break;
			}
		case ST_CMIS:
			{
				((EChesses*) mpChara->mEcOwner)->ShowTestF(stsp->getstring("text"), stsp->getint("color"));
				((EChesses*) mpChara->mEcOwner)->miAvgFlag = stsp->getint("value");
				CCLog(">[BBuffer] ST_CMIS | set miAvgFlag to %d.", ((EChesses*) mpChara->mEcOwner)->miAvgFlag);
				break;
			}
		case ST_HALO:
			{
				mpChara->LoadHalo(stsp);
				break;
			}
		case ST_DEATH:
			{
				if(EGroup::sharedEGroup()->MakDeath(stsp)){
					((EChesses*) mpChara->mEcOwner)->ShowTestF(stsp->getstring("text"), stsp->getint("color"));
				}
				break;
			}
		case ST_NODEA:
			{
				Chara* tc = (Chara*) EGroup::sharedEGroup()->meLast;
				tc->m_bIsDead = false;
				tc->sethp(1);
				((EChesses*) mpChara->mEcOwner)->ShowTestF(stsp->getstring("text"), stsp->getint("color"));
				break;
			}
		case ST_ANIMA:
			{
				float tb = stsp->getfloat("begin");
				float te = stsp->getfloat("end");
				if(te == 0) te = -1;


				((EChesses*) mpChara->mEcOwner)->setState(3);	
				((EChesses*) mpChara->mEcOwner)->playAnimate(stsp->getstring("name"),stsp->getint("repeat"), tb, te);
				break;
			}
		default:
			CCLog(">[BBuffer] Trig() | Unkown Type. Script Error.");

			break;
		}
		
	}

}

BBuffer::BBuffer(const char* scp, Chara* apc)
{
	miState = 0;
	mpChara = apc;
	mscp = new Scriptor();
	mscp->parse_string(scp);

	Script* tsp = (Script*) mscp->m_caScript->objectAtIndex(0);
	CCArray* tca = tsp->scriptnodes;
	CCObject* to;

	{
		CCARRAY_FOREACH(tca, to){
			misStates.insert(std::make_pair( ((Script*) to)->getint("id"), ((Script*) to)));
			CCLog(">[BBuffer] Read States Pair Script.");
		}
	}

	tsp = (Script*) mscp->m_caScript->objectAtIndex(1);
	tca = tsp->scriptnodes;

	{
		CCARRAY_FOREACH(tca, to){
			miscTrigger.insert(std::make_pair( ((Script*) to)->getint("type"), (Script*) to));
			RegTrigger(((Script*) to)->getint("type"));
			CCLog(">[BBuffer] Read Trigger Pair Script.");
		}
	}

	mcTarget = NULL;

	Trig(0);
}

void BBuffer::RegTrigger(int ai)
{
	CCLog(">[RegTrigger] Type: %d", ai);
	// < �������ֵ�������ڣ���������ɾ�� [TODO]
	switch(ai){

	default:
		{
			BTRigger* tbtr = new BTRigger(this);
			mTrigerCache.insert(make_pair(ai, tbtr));
			mpChara->addTrig(ai, tbtr);
			break;
		}
	}
}

void BBuffer::RefreshAttrs(const char* ast)
{
	//miAttrs.clear();

	stringstream teststream;
	teststream<<ast;
	string s;
	int k;
	do{
		s.clear();
		teststream>>s;
		if(s.length() < 1) break;
		teststream>>k;
		miAttrs[s] += k;
		CCLog(">[BBuffer] Read attr :[%s||%d]",s.c_str(),k);
	} while(1);
}

int BBuffer::getState(){
	return miState;
}