#include "EChessComp.h"
#include "Component/MapLayerComp.h"

#include "utils/Entiles.h"
#include "GameManager.h"
#include "SingleTon/BattleFiled.h"
#include "AIComponent.h"
#include "SingleTon/EGroup.h"
#include "utils/ColorSprite.h"
#include "battle/BBuffer.h"
#include "SoundManager.h"
#include "SingleTon/SkillJudge.h"

class ECCState : public CCObject
{
public:
	Script* mscp;
	int miSum, miNum;
	int miHigGrp;

	CCObject* meSrc;		// <�ű���Դ / �ű�ִ�ж���
	CCPoint m_mou_cur;

	ECCState();
	~ECCState();
};

ECCState::~ECCState()
{
	CCLog(">[ECCState] Deco ~");
	CC_SAFE_RELEASE_NULL(mscp);
	((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRelease();
}

ECCState::ECCState()
{
	meSrc = NULL;
}



#define FRETAIN CCCallFunc::create(this,callfunc_selector(EChessComp::ELock))
#define FRELEASE CCCallFunc::create(this,callfunc_selector(EChessComp::EUnLock))
#define CURSTATE	((ECCState*) mcesCurrent)

EChessComp::EChessComp()
{
	m_strName = "controller";
	miStateFlag = 0;
	miScriptSum = 0;
	mSp			= NULL;
	mbChessDead	= false;
	miELock		= -99;
	if(mbDebugS) CCLog(">[EChessComp] A nEW One.");
	mbDebugS = true;
	mbThinked = false;

	mcaMemories = new CCArray();
	mcesCurrent = NULL;
}

EChessComp::~EChessComp()
{
	CC_SAFE_RELEASE_NULL(mSp);
	CCLog(">[EChessComp] Deco. With %d mem.", mcaMemories->count());
	CC_SAFE_RELEASE_NULL(mcaMemories);
}

bool smbDirect = TRUE;

void EChessComp::move_by_path(std::vector<CCPoint> &vpath )
{
	mPath.clear();
	for(std::vector<CCPoint>::iterator it = vpath.begin(); it != vpath.end(); ++it){
		mPath.push_back(std::make_pair(it->x,it->y));
		if(mbDebugS) CCLog("WTF:%f,%f",it->x,it->y);
	}
	smbDirect = (mPath.size() <= 1);
	miStateFlag = 1;
	setScriptNum(0);
	miELock = 1;
	
	((Entiles*) m_pOwner)->direc =  MS_STOP;
	((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRetain();
	if(mbDebugS) CCLog(">[BattleMap]move_by_path() || Path is ready for component:%d",mPath.size());
}

void EChessComp::update(float delta){
	////[0803]if(mbDebugS) CCLog(">[EChessComp] update() | %d", miStateFlag);
	switch (miStateFlag)
	{
	case 1:			// <A* �ƶ���������������Ϊû�а취������Action������
		{
			if(((Entiles*) m_pOwner)->direc ==  MS_STOP){
				int t_isize = mPath.size();
				if(t_isize > 0){
					((Entiles*) m_pOwner)->setState(1);
					CCLog(">[EChessComp] update() | Push Target.");
					((EChesses*) m_pOwner)->SCMoveto(ccp(mPath.at(t_isize-1).first,mPath.at(t_isize-1).second));
					mPath.pop_back();
				}else{
					if(mbDebugS) CCLog(">[EChessComp] Moving over...");
					if(smbDirect) miStateFlag = 3;
					else miStateFlag = 2;
				}
			}/*else{
				CCLog(">[EChessComp] update() | Moving.");
			}*/
			break;
		}
	case 2:
		{
			if(!((EChesses*) m_pOwner)->m_bMoving){
				miStateFlag = 3;
			}
			break;
		}
	case 3:
		{
			miStateFlag = 0;
			if(mbDebugS) CCLog(">Moving over...<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
			((EChesses*) m_pOwner)->FixPosition();
			((Entiles*) m_pOwner)->direc =  MS_STOP;

			((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRelease();
			EUnLock();
			
			if(HaveAI()) break;
			GameManager::sharedLogicCenter()->ml->bm->LockCamera(m_pOwner);

			GameManager::sharedLogicCenter()->ml->bm->m_touch = new CCTouch();
			GameManager::sharedLogicCenter()->ml->bm->m_touch->setTouchInfo(0, ((EChesses*) m_pOwner)->getPositionX() + 20, ((EChesses*) m_pOwner)->getPositionY() + 100);

			GameManager::sharedLogicCenter()->ml->show_menu(0xdf);
			GameManager::sharedLogicCenter()->ml->mbNeedScroll = true;
			break;
		}

	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// <����

void EChessComp::GoAHead()
{
	//[0803]	
	if(mbDebugS) CCLog(">[EChessComp] GoAHead() | Lock:%d", miELock);
	if(miELock>0) return;

	slatst.clear();
	if(miScriptSum <= miScriptCount)
	{
		if(mbDebugS) CCLog(">[ECC]over:%d,%d",miScriptSum,miScriptCount);	 	
		if(mbDebugS) CCLog(">[ECC]over,owner position:%f,%f",((EChesses*) m_pOwner)->pos.x,((EChesses*) m_pOwner)->pos.y);
		if(mbDebugS) CCLog(">[ECC]over,owner ename:%s,cname:%s",((EChesses*) m_pOwner)->name.c_str(),((EChesses*) m_pOwner)->m_pChara->m_sName.c_str());
		
		//CC_SAFE_RELEASE_NULL(mSp);
		
		PopState();	
		if(mcesCurrent) return;

		AIComponent* taic = ((AIComponent*) ((EChesses*) m_pOwner)->getComponent("ai"));
		CCLog(">[EChessComp] GoAhead | Scp Over. | %s",((EChesses*) m_pOwner)->name.c_str() );
		
		if(miScriptSum > 0){
			((EChesses*) m_pOwner)->m_pChara->Trig(TR_ATKOVER);
			//((EChesses*) m_pOwner)->m_pChara->ScriptOver();
			//if(taic->mpChara) taic->ActOver();
		}else{
			if(HaveAI()) taic->MoveOver();
		}
		
	}else{
		if(mbDebugS) CCLog(">[ECC]step:%d/%d",miScriptSum,miScriptCount);
		DerScript((Script*) mSp->scriptnodes->objectAtIndex(miScriptCount));
	}
	
}

void EChessComp::setSrc(CCObject* teSrc)
{
	CURSTATE->meSrc = teSrc;
}


CCObject* EChessComp::getSrc()
{
	return CURSTATE->meSrc;
}

int EChessComp::getCurGrp()
{
	return ((ECCState*) mcesCurrent)->miHigGrp;
}

void EChessComp::PopState()
{
	((EChesses*) m_pOwner)->miHitGroup = 0;
	CCLog(">[EChessComp] PopState() | Begin %d mem", mcaMemories->count());
	if(miScriptSum == -1 && !mcesCurrent) {
		CCLog(">[EChessComp] PopState() | Trick. Third Attack call ends here.");
		((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRelease();
	}
	if(!mcesCurrent) return;

	// <�����ǰ״̬
	mcaMemories->removeLastObject();
	mcesCurrent = NULL;
	mSp			= NULL;

	CCLog(">[EChessComp] PopState() | State Clear Over.");

	// <���Զ�ȡ״̬
	mcesCurrent = mcaMemories->lastObject();
	if(mcesCurrent){
		mSp				 = CURSTATE->mscp;
		miScriptCount	 = CURSTATE->miNum;
		miScriptSum		 = CURSTATE->miSum;

		miELock = 0;
		m_pOwner->runAction(CCSequence::create(CCDelayTime::create(0),FRELEASE,NULL));
		//EUnLock();
	}
}

void EChessComp::RunScript( Script* asp )
{
	CCLog(">[ECC] RunScript() | Begin.");
	((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRetain();
	if(mSp) {
		CCLog(">[ECC] RunScript() | Run on running node.");
		((ECCState*) mcesCurrent)->miNum = miScriptCount;
	}
	m_pOwner->stopAllActions();
	//CC_SAFE_RELEASE_NULL(mSp);
	
	// <��Ҫ�ĳ�ʼ��
	mSp = asp;
	mSp->retain();
	setScriptNum(mSp->m_snum);
	miELock = 0;

	// <����״ֵ̬
	ECCState* teccs = new ECCState();
	teccs->mscp		 = mSp;
	teccs->miSum	 = mSp->m_snum;
	teccs->miNum	 = 0;
	teccs->m_mou_cur = GameManager::sharedLogicCenter()->ml->bm->m_mou_cur;
	teccs->miHigGrp	 = ((EChesses*) m_pOwner)->miHitGroup;


	mcaMemories->addObject(teccs);
	mcesCurrent		 = teccs;
	teccs->autorelease();

	// <��Ҫ������
	((EChesses*) m_pOwner)->miHitGroup = 0;

	// <<ִ�нű�
	if(mbDebugS) CCLog(">[ECC]Script Ready:%d",miScriptSum);
	m_pOwner->runAction(CCSequence::create(CCDelayTime::create(0),FRELEASE,NULL));
	((EChesses*) m_pOwner)->setState(3);
}

void EChessComp::setScriptNum( int ai )
{
	miScriptSum		= ai;
	miScriptCount	= 0;
}

void EChessComp::DerScript( Script* asp )
{
	int tiSum = asp->m_snum;
	CCArray* acts = asp->scriptnodes;
	miELock = 0;
	Chara* tow = ((EChesses*) m_pOwner)->m_pChara;			// < ��Chess�Ľ�ɫChara.

	if(mbDebugS) CCLog("-----------------script handle----------------------");
	for (int i = 0;i<tiSum;i++)		//multi here?
	{
		if(mbDebugS) CCLog("handle scripte:%d/%d",i,tiSum);
		Script* tmp = (Script*) acts->objectAtIndex(i);//use tag to define node's having state


		int tiType = tmp->getint("type");
		if(mbDebugS) CCLog(">[EChessComp] DerScript() | Script type:%d", tmp->getint("type"));
		//////////////////////////////////////////////////////////////////////////
		// <�����>[EChessComp] type == 8 | c == 2 | change from this to ec-src
		// <�ű�ע�⣺ ��������ǵ�һ��
		// <giҲ���ǽű���ָ�������ж����������ʽ�Ķ�hitgroup��ƥ���õġ�

		int gi = tmp->getint("group");
		CCLog(">[EChessComp] DerScript || gid:%d| %s", gi, ((EChesses*) m_pOwner)->m_pChara->m_sName.c_str());

		if(gi<0){
			//gi = abs(gi) + GameManager::sharedLogicCenter()->ml->bm->m_bSearch->getCount();
			gi = (2<<GameManager::sharedLogicCenter()->ml->bm->m_bSearch->getCount());
			CCLog(">[EChessComp] DerScript() || m_bSearch:%d", GameManager::sharedLogicCenter()->ml->bm->m_bSearch->getCount());
		}

		CCLog(">[EChessComp] DerScript || re:gid:%d", gi);
		int gn = ((ECCState*) mcesCurrent)->miHigGrp;

		if(0 == gn) gn = 1;
		if(0 == gi) gi = 0xff;

		if((gn & gi) == 0){
			if(mbDebugS) CCLog(">[EChessComp] DerScript() || Group filtered, Only Delay works. need[%d] - bug[%d]", gi, gn);
			if(tmp->type == sChange && tmp->getint("type") == 0){
				ELock();
				m_pOwner->runAction(CCSequence::create(CCDelayTime::create(tmp->getfloat("delay")),FRELEASE,NULL));
				if(mbDebugS) CCLog("delay:%f",tmp->getfloat("delay"));
			}
			continue;
		}
		if(mbDebugS) CCLog(">[EChessComp] DerScript() || Prepare for Script Group:%i",gi);

		//////////////////////////////////////////////////////////////////////////
		// <�ű�ɸѡ | ��־

		int tfil = tmp->getint("filter");
		bool filt = false;
		switch (tfil)
		{
		case 1:		// <��־ | �ȼ��� | ��Ȳ�ͨ��
			{
				if(tow->getvalue("tama_3") != tmp->getint("val")) {
					filt = true;
				}
				break;
			}
		case 2:		// <��־���
			{
				int dis = ((EChesses*) CURSTATE->meSrc)->m_pChara->getvalue("tama_3") - tow->getvalue("tama_3");
				if(dis) dis = dis/abs(dis);
				if(dis !=  tmp->getint("val")){
					filt = true;
				}
			}
		default:
			break;
		}

		// <�ű�ɸѡ | BOSS�ȼ�
		tfil = tmp->getint("filter_b");
		switch (tfil)
		{
		case 1:		// <�����ͨ��
			{
				if( tow->getvalue("boss_class") != tmp->getint("bcf")){ 
					filt = true;
				}
			}
		case 2:		// <�������ͨ��
			{
				if( tow->getvalue("boss_class") == tmp->getint("bcf")){ 
					filt = true;
				}
			}
		case 3:	// <�������д��ڸ�ֵ�����
			{
				if( tow->getvalue("boss_class") > tmp->getint("bcf")){ 
					filt = true;
				}
			}
		case 4:	// <��������С�ڵ��ڸ�ֵ�����
			{
				if( tow->getvalue("boss_class") > tmp->getint("bcf")){ 
					filt = true;
				}
			}
		default:
			break;
		}

		// <����ɸѡ
		if(filt){
			if(tiType != 4){
				continue;
			}else{
				((EChesses*) m_pOwner)->miAvgFlag = 1;
			}
			
		}
		//////////////////////////////////////////////////////////////////////////
		// <����
		float tRat = tmp->getfloat("rate");
		if(tRat>CCRANDOM_0_1()) continue;

		//////////////////////////////////////////////////////////////////////////
		// <ִ�нű�
		switch(tmp->type)
		{
		case sShowText:
			{
				if(mbDebugS) CCLog("string:%s",tmp->getstring("content"));
				break;
			}
		case sChange:
			{
				
				if(mbDebugS) CCLog("Change Type int:%i",tiType);
				switch (tiType)
				{
				case -99:	// DEBUG ONLY
					{
						exit(99);
						break;
					}
				case 0:			// type = 0 | <�ӳ٣�ÿ������һ���͹��ˡ�
					{
						ELock();
						m_pOwner->runAction(CCSequence::create(CCDelayTime::create(tmp->getfloat("delay")),FRELEASE,NULL));
						if(mbDebugS) CCLog("delay:%f",tmp->getfloat("delay"));
						break;
					}
				case 1:			// type = 1 | <���й����ж������Ź��������� || ��ͨ������
					{
						BattleField::sharedBattleField()->Judge(tmp);
						break;					
					}
				case -101:		// type = -101 | <��ͨ�����Ķ�������
					{
						((Entiles*) m_pOwner)->setState(3);
						if(((Entiles*) m_pOwner)->miHitFlag){
							((Entiles*) m_pOwner)->playAnimate("huixin",tmp->getint("repeat"));
						}else
						{
							((Entiles*) m_pOwner)->playAnimate("gongji",tmp->getint("repeat"));
						}
						((Entiles*) m_pOwner)->AnimateNoLastO();
						break;
					}
				case -1:		// type = -1 | <�����ж��������Ŷ����� || ������
					{
						BattleField::sharedBattleField()->Judge(tmp);
						((Entiles*) m_pOwner)->setState(3);
						break;
					}
				case -2:
					{
						if(tmp->getint("phisical")){
							if(((Entiles*) m_pOwner)->miHitFlag){
								((Entiles*) m_pOwner)->playAnimate("huixin",tmp->getint("repeat"));
							}else
							{
								((Entiles*) m_pOwner)->playAnimate("gongji",tmp->getint("repeat"));
							}
							((Entiles*) m_pOwner)->AnimateNoLastO();
						}else{
							if(((Entiles*) m_pOwner)->miHitFlag){
								((Entiles*) m_pOwner)->playAnimate("yongcan",tmp->getint("repeat"));
							}else
							{
								((Entiles*) m_pOwner)->playAnimate("ychx",tmp->getint("repeat"));
							}
							((Entiles*) m_pOwner)->AnimateNoLastO();
						}

						break;
					}
				case 2:			// type = 2.. <������ || �ڹ����������ж����ʱ����������ִ�� || �񵲵��ж��������﷢��
					{
						((Entiles*) m_pOwner)->setState(3);
						/* <Ŀǰ�޸� */

						break;
					}
				case 3:
					{
						CCLog(">[EChessComp] Script case 3 | Pos: %f, %f", ((EChesses*) m_pOwner)->pos.x, ((EChesses*) m_pOwner)->pos.y);
						BattleField::sharedBattleField()->PlayEffectSp(this, "Images/yari.png");
						break;
					}
				case -3:
					{
						BattleField::sharedBattleField()->PlayEffectSpFr(m_pOwner, tmp);
						break;
					}
				case 4:		// <��ʾ�ı�  <������
					{

						//((EChesses*) m_pOwner)->miAvgFlag = 1;
						CCLog(">[EChessComp] case:4 | showDamage() | Debug:%s|| filter:%d | group:%d", tmp->getstring("debug"), gi, gn);
						if(mbDebugS) CCLog(">[EChessComp] type == 4 | %s", ((EChesses*) m_pOwner)->m_pChara->m_sName.c_str());
						if(mbDebugS) CCLog(">[EChessComp] DerScript()-ChangeType:4-ShowDamage()");
						
						float tfDelay = tmp->getfloat("delay");
						if(tfDelay == 0){
							tfDelay = 0.25;
						}

						ELock();
						m_pOwner->runAction(CCSequence::create(CCDelayTime::create(tfDelay),FRELEASE,NULL));
						
						((Entiles*) m_pOwner)->mbCanNotShield = (tmp->getint("nshield"));
						((Entiles*) m_pOwner)->mbCanNotAvg = (tmp->getint("force"));
						((Entiles*) m_pOwner)->ShowDamage();
						((Entiles*) m_pOwner)->mbCanNotAvg = false;
						((Entiles*) m_pOwner)->mbCanNotShield = false;

						if(((EChesses*) m_pOwner)->miAvgFlag){
							BattleField::sharedBattleField()->ClearTrigFlag();
							BattleField::sharedBattleField()->mCachedChess = m_pOwner;
							((EChesses*) m_pOwner)->m_pChara->Trig(TR_MISS);
							if(BattleField::sharedBattleField()->mbTrigged){
								return;
							}
							break;
						}else{

							BattleField::sharedBattleField()->RefreshStats();
						}


						//ELock();
						//m_pOwner->runAction(CCSequence::create(CCDelayTime::create(4.9),FRELEASE,NULL));
						if(tmp->getint("bid") == 0){
							break;
						}		// <������ṩbid��ֱ�ӵ���12
						CCLog("-- case:4 | Special Attack with buffer --");
					}
				case 12:	// <Buffer���
					{
						CCLog(">[EChessComp] BufferLock: %d, %d", tmp->getint("bid"), tmp->getint("count"));
						((EChesses*) m_pOwner)->m_pChara->LoadBuffer(tmp->getint("bid"), tmp->getint("count"));

						break;
					}
				case 5:			// type = 5 | <����ָ���Ķ���-Skill.  || ע�⴫��� begin �� end���������ļ��ܵġ�
					{
						//BattleField::sharedBattleField()->Judge();

						float tb = tmp->getfloat("begin");
						float te = tmp->getfloat("end");
						if(te == 0) te = -1;


						((Entiles*) m_pOwner)->setState(3);	
						((Entiles*) m_pOwner)->playAnimate(tmp->getstring("name"),tmp->getint("repeat"), tb, te);
						break;
					}
				case -5:	// type = -5 | <ȡ����������������״̬ || ��������ʱΪ�˷�ֹ��˸���Զ���NoLastģʽ
					{
						((Entiles*) m_pOwner)->AnimateNoLastO();
						break;
					}
				case 6:	// type = 6 | <��ֵ���������ڷ�Hp����ֵ�仯
					{
						Chara* tow = ((EChesses*) m_pOwner)->m_pChara;
						string tname =  tmp->getstring("name");
						int tiv = tmp->getint("value");

						CCLog(">[EChessComp] DerScript | type == 6 | %s, %d", tname.c_str(), tiv);
						tow->addBatValue(tname, tiv);

						BattleField::sharedBattleField()->RefreshStats();

						//if(strcmp(tname.c_str(), "b_hunpo")) {
						//							
						//	if(BattleField::sharedBattleField()->ECIsSrc((EChesses*) getOwner())){
						//		if(tiv < 0){
						//			// <���ݼ��ܺ�Buffer�ļӳɶԻ������Ľ�������

						//			EGroup::sharedEGroup()->ExpHunp(getOwner(), -tiv);
						//		}
						//	}
						//	tow->setvalue(tname,tow->getvalue(tname)+tiv);
						//}else{
						//	tow->setvalue(tname,tow->getvalue(tname)+tiv);
						//}
						
						
						break;
					}
				case 7:	// type = 7 | <��תλ�ã����ڼ����е�λ˲���ƶ������ || ��ǰ��λ��ת || ���ڼ��ܵ�Ŀ����ת
					{
						// <ע��������õ�Ԫ�أ�ͨ������¼����ڱ�����ű�ǰ����Ե�λִ��changeFace();
						int dx = tmp->getint("dx");
						int dy = tmp->getint("dy");
						if(dx == 0 && dy == 0){  // <û���ṩ��תĿ�������½�����ת�� m_con_cur || ��ת��������ĸ�����
							GameManager::sharedGameManager()->sharedLogicCenter()->ml->bm->moveChessConcru((EChesses*) getOwner());
						}else
						{
							GameManager::sharedGameManager()->sharedLogicCenter()->ml->bm->moveChess((EChesses*) getOwner(), ccp(dx,dy), true);
						}
						break;
					}
				case -7:	// type = -7 | <�ƶ���λ���в��䣬��˲��
					{
						int dx = tmp->getint("dx");
						int dy = tmp->getint("dy");

						int cx = ((EChesses*) m_pOwner)->pos.x;
						int cy = ((EChesses*) m_pOwner)->pos.y;

						CCLog(">[EChessComp] Move Chess. f[%d,%d] - t[%d,%d]", dx, dy, cx, cy);

						if(dx == 0 && dy == 0){
							((EChesses*) m_pOwner)->MoveChessSk(CURSTATE->m_mou_cur.x, CURSTATE->m_mou_cur.y);
						}else{
							switch (BattleField::sharedBattleField()->meSrc->direc)
							{
							case MS_LEFT:
								{
									cx -= dx;
									break;
								}
							case MS_RIGHT:
								{
									cx += dx;
									break;
								}
							case MS_UP:
								{
									cy -= dx;
									break;
								}
							case MS_DOWN:
								{
									cy += dx;
									break;
								}
							default:
								break;
							}
							((EChesses*) m_pOwner)->MoveChessSk(cx, cy);
						}

						break;
					}
				case 8:	// type = 8 | <������λ�ĳ��� 
					{
						CCLog(">[EChessComp] type == 8 <|");
						int tiCtype = tmp->getint("ctype");
						switch (tiCtype)
						{
						case 0:
							{
								GameManager::sharedGameManager()->sharedLogicCenter()->ml->bm->ChangeAllFace();	
								break;
							}
						case 1:	//  < Meorig --- Mesrc
							{
								CCLog(">[EChessComp] type == 8 | c == 1 | change from orig to src");
								BattleField::sharedBattleField()->meOrig->ChangeFace(((EChesses*) GameManager::sharedLogicCenter()->ml->bm->m_controller)->pos);
								break;
							}
						case 2:	// <   orig --- owner
							{
								if(mbDebugS) CCLog(">[EChessComp] type == 8 | c == 2 | change from this to ec-src");
								if(CURSTATE->meSrc)
									((EChesses*) m_pOwner)->ChangeFace( ((EChesses*) CURSTATE->meSrc)->pos);
								break;
							}
						default:
							break;
						}
								
						break;
					}
				case 9: // type = 9 | <���� | ���幦�ܴ���~
					{
						CCLog(">[EChessComp] Script Ctn.");
						//////////////////////////////////////////////////////////////////////////
						// <��������
						EChesses* tecsrc = NULL;

						int tisrc = tmp->getint("src");	// <��������Դ
						int tihit = tmp->getint("hit");	// <�Ƿ���Ҫ����

						if(tihit){
							if(((EChesses*) m_pOwner)->miAvgFlag) break;	// <�ɱ�������λ����|����Ƿ�����
							BattleField::sharedBattleField()->meCtnTarget = m_pOwner;
						}

						switch (tisrc)
						{
						case 0:
							tecsrc = BattleField::sharedBattleField()->meSrc;
							break;
						default:
							break;
						}

						CCLog(">[EChessComp] Script Ctn. | Preparing is over.");
						GameManager::sharedGameManager()->sharedLogicCenter()->ml->bm->m_controller = tecsrc;
						BattleField::sharedBattleField()->fGetCont(tmp->getstring("name"));

						break;
					}
				case 10:	// type = 10 | <������������ת
					{
						if(mbDebugS) CCLog(">[EChessComp] type == 10 | %s", ((EChesses*) m_pOwner)->m_pChara->m_sName.c_str());
						if(!GameManager::sharedLogicCenter()->ml->bm->m_bSearch->hasFront()){
							if(mbDebugS) CCLog("--- Jump");
							miScriptCount += tmp->getint("rel");
						}else{
							if(mbDebugS) CCLog("--- Direct");
						}
						break;
					}
				case -10:	// type = -10 | <��������ת
					{
						miScriptCount += tmp->getint("rel");
						break;
					}
				case 11:	// type = 11 | <��ǰ��������
					{
						int tdx, tdy, ttype;
						GameManager::sharedLogicCenter()->ml->bm->m_bSearch->getFront(tdx, tdy);

						if(mbDebugS) CCLog(">[EChessComp] m_bSearch | %d,%d | Cur:%d", tdx, tdy, miScriptCount);
						ttype = tmp->getint("cptype");
						switch (ttype)
						{
						case 1:	// <����������һ����
							{
								((EChesses*) getOwner())->direc = GameManager::sharedLogicCenter()->ml->bm->m_bSearch->getNextDirect(((EChesses*) getOwner())->pos.x,((EChesses*) getOwner())->pos.y);
								break;
							}
						case 2: // <������һ����
							{
								CCLog(">[EChessComp] m_bSearch | Jmp.");
								GameManager::sharedGameManager()->sharedLogicCenter()->ml->bm->moveChess((EChesses*) getOwner(), ccp(tdx,tdy), false);
								GameManager::sharedGameManager()->sharedLogicCenter()->ml->bm->LockCamera(m_pOwner);
								break;
							}
						case 3:	// <��������
							{
								GameManager::sharedLogicCenter()->ml->bm->m_bSearch->stepFront();
								break;
							}
						default:
							if(mbDebugS) CCLog(">[EChessComp] type = 11 | No cptype provide or zero one. [ERROR]");
							break;
						}
						break;
					}
				case 13:	// <�Ƿ񹥻���ռ�� | ��ת | ����������
					{
						if(BattleField::sharedBattleField()->TestPrior(NULL, NULL)){
							if(mbDebugS) CCLog(">[EChessComp] Atk is prio");
							miScriptCount += tmp->getint("yes");
						}else{
							if(mbDebugS) CCLog(">[EChessComp] Atk is not prio");
							miScriptCount += tmp->getint("no");
						}
						break;
					}
				case 14:	// <λ����ز���|Src�� | ��ת
					{
						int dis = BattleField::sharedBattleField()->getDisST();
						switch (tmp->getint("id"))
						{
						case 0:
							{
								if(dis = tmp->getint("dis")) {
									miScriptCount += tmp->getint("jmp");
								}
								break;
							}
						case 1:
							{
								if(dis > tmp->getint("dis")) {
									miScriptCount += tmp->getint("jmp");
								}
								break;
							}
						default:
							break;
						}
						break;
					}
				case -14:	// <λ����ز���|Tar�� | ��ת
					{
						break;
					}
				case 15:	// <�����ƶ���SPX����
					{
						BattleField::sharedBattleField()->PlaySpxEC(m_pOwner, tmp);
						break;
					}
				case -15:   // <ָ������Ķ���
					{
						CCPoint p = GameManager::sharedLogicCenter()->ml->bm->m_getViewc(CURSTATE->m_mou_cur);
						BattleField::sharedBattleField()->PlaySpxPo(p, tmp);
						break;
					}
				case 16:	// <��������
					{
						
						slatst = tmp->getstring("sound");
						CCLog(">[EChessComp] PlaySound(%s)", slatst.c_str());
						m_pOwner->runAction(CCSequence::create(CCDelayTime::create(tmp->getfloat("delay")), CCCallFunc::create(this,callfunc_selector(EChessComp::DelayPlay)), NULL));
						break;
					}
				case 17:	// <��ʧ�ͳ��� | һ������²���ʹ�õ�
					{
						int ctype = tmp->getint("ctype");

						switch (ctype)
						{
						case 0:
							{
								m_pOwner->setVisible(false);
								break;
							}
						case 1:
							{
								m_pOwner->setVisible(true);
								break;
							}
						default:
							break;
						}

						break;
					}
				case 18:	// <���Ŷ��������Զ���ӷ����׺
					{

						//BattleField::sharedBattleField()->Judge();

						float tb = tmp->getfloat("begin");
						float te = tmp->getfloat("end");
						if(te == 0) te = -1;


						((Entiles*) m_pOwner)->setState(3);	
						((Entiles*) m_pOwner)->playAnimateDi(tmp->getstring("name"),tmp->getint("repeat"), tb, te);
						break;

					}
				case 19:	// <�ظ�Ѫ��| ������ | �����ж���Ҳ�����г�HP
					{
						int tt_hp = tmp->getint("chp");
						((EChesses*) m_pOwner)->m_pChara->addBatValue("chp", tt_hp);
						stringstream tss;
						tss << "+";
						tss << tt_hp;

						((EChesses*) m_pOwner)->ShowTestF(tss.str().c_str(), 1);
						
						BattleField::sharedBattleField()->RefreshStats();
						break;
					}
				case 20:	// <����
					{
						((EChesses*) m_pOwner)->m_pChara->sethp(0);
						((EChesses*) m_pOwner)->ShowTestF(tmp->getstring("text"), 1);
						break;
					}
				default:
					exit(0x777);
					break;
				}
				break;
			}
		}
	}
	++miScriptCount;
	//ELock();
	//m_pOwner->runAction(CCSequence::create(CCDelayTime::create(0), CCCallFunc::create(this,callfunc_selector(EChessComp::EUnLock)), NULL));
	GoAHead();
}

void EChessComp::ELock()
{	
	++miELock;
	if(mbDebugS) CCLog("[EChessComp]ELock() | Lock:%d",miELock);
}

void EChessComp::EUnLock()
{
	--miELock;
	if(mbDebugS) CCLog("[EChessComp]EUnLock() | :%d",miELock);
	if(miELock<=0){
		miELock = 0;
		if(mbDebugS) CCLog("-----------Script GoAhead--------------------");
		GoAHead();
	}
}

bool EChessComp::TestRange( CCObject* tar, bool acat)
{
	if(mbDebugS) CCLog("[ECC]TestRange with pos:%d,%f", ((EChesses*) tar)->pos.x, ((EChesses*) tar)->pos.y);
	return SkillJudge::sharedSkillJudge()->CheckNorM(m_pOwner, tar, acat);

}

bool EChessComp::AtkTestRange( CCPoint target )
{
	if(mbDebugS) CCLog("[ECC]TestRange with pos:%d,%f",target.x,target.y);
	vector<int> ars;
	ars.push_back((((EChesses*) m_pOwner)->m_pChara)->miRange);		// <Range����һ�������ж���
	return GameManager::sharedLogicCenter()->ml->bm->f_RangeTest(1,ars,((EChesses*) m_pOwner)->pos,target);
}

/* <Ѱ�ҷ������� */
bool EChessComp::FindFitRe( CCObject* tar,int atime )
{

	if(this == tar) exit(1111);
	if(!(((EChesses*) m_pOwner)->m_pChara)->CanAct()) return false;
	
	/* <��ͨ���� - atime ���� */
	if(miReCount == 0){		// <���������η�����ѯ || ���ε������Ȼ���չ�
		miReState = 2;
		return false;	
	}

	CCLog(">[EChessComp] FindFitRe()|%s | Begin. Act Pass.", ((EChesses*) m_pOwner)->name.c_str());

	if(HaveAI()){
		CCLog(">[EChessComp] FindFitRe()| AI Chara Being ");
	}else{
		CCLog(">[EChessComp] FindFitRe()| Player Chara Bing.");
	}
	/* <��Ҿ��� */
	if( (!mbThinked)){
		if(miReState == 2) return false;
		CCLog(">[EChessComp] FindFitRe() | Try popup Re for pc.");
		BattleField::sharedBattleField()->miState = 2;
		
		mbThinked = true;

		if(HaveAI()){
			//int b_r = CCRANDOM_0_1() * 100;
			//if(b_r < 100){	/* <50%�ļ��ʳ��Խ��м��ܷ��� */
				if(BattleField::sharedBattleField()->PUSkillFilt(m_pOwner, tar, 1)){
					return true;
				}
			//}

		}else{
			if(BattleField::sharedBattleField()->PUSkillFilt(m_pOwner, tar, 1)){
				return true;
			}
		}

		
		CCLog(">[EChessComp] FindFitRe() | Pop up Skill List Failed.");
		
	}

	if((!HaveAI()) && mbThinked){
		CCLog(">[EChessComp] FindFitRe() | Thinked, state:%d", miReState);
		if(miReState == 2) return false;
		mbThinked = false;
	}

	CCLog(">[EChessComp] FindFitRe() | LastPass, state:%d", miReState);
	if(miReState == 2) return false;

	CCLog(">[EChessComp] FindFitRe() | Ready For Normal Attack.");
	miReState = 1;
	--miReCount;
		
	 if(TestRange(tar, true)){
		 CCLog(">[EChessComp] FindFitRe() | Can Act and act.");
		 miReState = 2;
		 return true;
	 }

	 miReState = 2;
	 CCLog(">[EChessComp] FindFitRe() | Not able to react.");
	 return false;

	
}

void EChessComp::CleanRe()
{
	miReCount = 2;
	miReState = 0;
	mbThinked = false;
}

int EChessComp::PreCheckRe( CCObject* tar,int atime )
{
	if(AtkTestRange(((EChesses*) tar)->pos)) {
		return atime;
	}else{
		return 0;
	}
}

void EChessComp::DelayUnLock(float dt )
{
	ELock();
	miScriptSum = -1;
	m_pOwner->runAction(CCSequence::create(CCDelayTime::create(dt),FRELEASE,NULL));
}

int EChessComp::HaveAI()
{
	return !EGroup::sharedEGroup()->CheckGrp(m_pOwner);
}


//////////////////////////////////////////////////////////////////////////

void EChessComp::ChessDead()
{
	mbChessDead = true;
	CheckAlive();
}

void EChessComp::onChessDead()
{
	((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRetain();
	ELock();
	//////////////////////////////////////////////////////////////////////////
	// <����״ֵ̬ - ʹ���߼�ͳһ
	ECCState* teccs = new ECCState();
	teccs->mscp		 = NULL;
	teccs->miSum	 = 0;
	teccs->miNum	 = 0;
	teccs->miHigGrp	 = 0;
	mcaMemories->addObject(teccs);
	mcesCurrent		 = teccs;
	teccs->autorelease();


	//////////////////////////////////////////////////////////////////////////
	float tDelay = 0.8;

	EChesses* tEc = (EChesses*) getOwner();

	tEc->m_pChara->PlaySS("dead");

	tDelay = max(tDelay, tEc->m_pChara->m_sDelay);
	EGroup::sharedEGroup()->ExpDead(tEc);
	tEc->meRS->m_pChara->setHit(-tEc->m_pChara->getvalue("boss_class")-1);

	//////////////////////////////////////////////////////////////////////////
	CCArray *animFrames = CCArray::create();  
	CCSpriteFrameCache *cache = CCSpriteFrameCache::sharedSpriteFrameCache();

	//hitAa000.bmp
	char str[64] = {0};  
	for (int i = 0; i <= 7; ++i) {  
		sprintf(str, "hitAa%03d.bmp", i);  
		CCSpriteFrame *frame = cache->spriteFrameByName(str);  
		if(mbDebugS) CCLog(str);  
		// <���֡������  
		animFrames->addObject(frame);  
	}  

	CCAnimation *animation = CCAnimation::createWithSpriteFrames(animFrames, 0.1);  
	animation->setRestoreOriginalFrame(true);  
	CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "Dead"); 

	//////////////////////////////////////////////////////////////////////////
	CCSpriteFrame *frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("hitAa000.bmp");  

	ColorSprite *mcsFrame = ColorSprite::createWithSpriteFrame(frame);  
	mcsFrame->setPosition(ccp(15, 15));  
	mcsFrame->setAnchorPoint(ccp(0.5,0.5));
	mcsFrame->setScale(0.8);
	m_pOwner->addChild(mcsFrame);

	CCAnimation *anim = CCAnimationCache::sharedAnimationCache()->animationByName("Dead");  
	CCAnimate *animate = CCAnimate::create(anim);  

	if(mbDebugS) CCLog(">[EChessComp] OnChessDead() | show_dead");
	//mParent->update(0);			// <[Debug]
	mcsFrame->setVisible(false);
	mcsFrame->runAction(CCSequence::create(CCDelayTime::create(tDelay - 0.78), CCShow::create(), animate,NULL));

	//////////////////////////////////////////////////////////////////////////
	m_pOwner->runAction(CCSequence::create(CCDelayTime::create(tDelay), CCCallFunc::create(this,callfunc_selector(EChessComp::ChessDead)), FRELEASE, NULL));
	
}

void EChessComp::onAddExp()
{
	((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRetain();
	EChesses* tEc = (EChesses*) getOwner();
	int tiExp = tEc->m_pChara->miEXP;
	if(mbDebugS) CCLog(">[BattleFieldScp] ExpShow(%d)", tiExp);

	int toExp = tEc->m_pChara->getvalue("exp");
	toExp %= 100;
	toExp += tiExp;
	if( toExp > 100){
		if(toExp<200){
			tEc->m_pChara->plusvalue("exp", tiExp);
			tEc->m_pChara->miEXP = 0;
		}else{
			toExp = 100 - toExp + tiExp;
			tEc->m_pChara->plusvalue("exp", toExp);
			tEc->m_pChara->miEXP = tiExp - toExp;
		}
		tEc->m_pChara->plusvalue("lv", 1);
		GameManager::sharedLogicCenter()->ml->showUpgrd(tEc);
	}else{
		tEc->m_pChara->plusvalue("exp", tiExp);
		tEc->m_pChara->miEXP = 0;
		GameManager::sharedLogicCenter()->ml->refresh_view();
		((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRelease();
	}

	

	
}

void EChessComp::CheckAlive()
{
	if(!mbChessDead) return;
	EChesses* tEc = (EChesses*) getOwner();

	if(tEc == BattleField::sharedBattleField()->meConS) BattleField::sharedBattleField()->meConS = NULL;

	GameManager::sharedLogicCenter()->ml->bm->RemoveWhileDead(tEc);

	for (vector<CollideInfo*>::iterator iter = GameManager::sharedLogicCenter()->ml->bm->colse->bc.begin();iter!= GameManager::sharedLogicCenter()->ml->bm->colse->bc.end();iter++)  
	{
		(*iter)->release();
	}
	GameManager::sharedLogicCenter()->ml->bm->colse->bc.clear();
	BattleField::sharedBattleField()->onDeadEvent(tEc);
	
	tEc->removeFromParent();
	/*tEc->release();
	tEc->autorelease();*/
	//delete tEc;
}

void EChessComp::DelayPlay()
{
	CCLOG(">[EChessComp]Delayplay");
	SoundManager::sharedSoundManager()->PlayCVSound(slatst.c_str());
}


