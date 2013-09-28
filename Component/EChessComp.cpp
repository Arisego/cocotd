#include "EChessComp.h"
#include "Component/MapLayerComp.h"

#include "utils/Entiles.h"
#include "GameManager.h"
#include "SingleTon/BattleFiled.h"

#define FRETAIN CCCallFunc::create(this,callfunc_selector(EChessComp::ELock))
#define FRELEASE CCCallFunc::create(this,callfunc_selector(EChessComp::EUnLock))

EChessComp::EChessComp()
{
	m_strName = "controller";
	miStateFlag = 0;
	miScriptSum = 0;
	mSp			= NULL;
}

EChessComp::~EChessComp()
{
	CC_SAFE_RELEASE_NULL(mSp);
}

void EChessComp::move_by_path(std::vector<CCPoint> &vpath )
{
	mPath.clear();
	for(std::vector<CCPoint>::iterator it = vpath.begin(); it != vpath.end(); ++it){
		mPath.push_back(std::make_pair(it->x,it->y));
		CCLOG("WTF:%f,%f",it->x,it->y);
	}
	miStateFlag = 1;
	setScriptNum(0);
	miELock = 1;
	((Entiles*) m_pOwner)->setState(1);

	((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRetain();
	//[0803]CCLog(">Path is ready for component:%d",mPath.size());
}

void EChessComp::update(float delta){
	////[0803]CCLog(">?Who update this.?");
	switch (miStateFlag)
	{
	case 1:			// <A* �ƶ���������������Ϊû�а취������Action������
		{
			if(((Entiles*) m_pOwner)->direc ==  MS_STOP){
				int t_isize = mPath.size();
				if(t_isize > 0){
					((Entiles*) m_pOwner)->SCMoveto(ccp(mPath.at(t_isize-1).first,mPath.at(t_isize-1).second));
					mPath.pop_back();
				}else{
					CCLOG(">Moving over...");
					miStateFlag = 0;
					EUnLock();
				}
			}
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
	//[0803]CCLog(">[ECC]Go aHead...");
	if(miELock>0) return;
	if(miScriptSum <= miScriptCount)
	{
		CCLog(">[ECC]over:%d,%d",miScriptSum,miScriptCount);	 	
		CCLOG(">[ECC]over,owner position:%f,%f",((EChesses*) m_pOwner)->pos.x,((EChesses*) m_pOwner)->pos.y);
		CC_SAFE_RELEASE_NULL(mSp);
		((EChesses*) m_pOwner)->miHitGroup = 0;			// <����������Ϊ0���������ʱ�޸����
		((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRelease();
		
	}else{
		CCLog(">[ECC]step:%d/%d",miScriptSum,miScriptCount);
		DerScript((Script*) mSp->scriptnodes->objectAtIndex(miScriptCount));
	}
	
}

void EChessComp::RunScript( Script* asp )
{
	CC_SAFE_RELEASE_NULL(mSp);
	mSp = asp;
	mSp->retain();
	setScriptNum(mSp->m_snum);
	miELock = 0;
	//[0803]CCLog(">[ECC]Script Ready:%d",miScriptSum);
	((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRetain();
	GoAHead();
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
	CCLOG("-----------------script handle----------------------");
	for (int i = 0;i<tiSum;i++)		//multi here?
	{
		CCLog("handle scripte:%d/%d",i,tiSum);
		Script* tmp = (Script*) acts->objectAtIndex(i);//use tag to define node's having state

		// <����Ƿ��ǸĲ�����Ķ�����������ͬ�������ò��š�
		if(((EChesses*) m_pOwner)->miHitGroup != tmp->getint("group")) {
			if(tmp->type == sChange && tmp->getint("type") == 0){
				ELock();
				m_pOwner->runAction(CCSequence::create(CCDelayTime::create(tmp->getfloat("delay")),FRELEASE,NULL));
				CCLog("delay:%f",tmp->getfloat("delay"));
			}
			continue;
		}	

		switch(tmp->type)
		{
		case sShowText:
			{
				CCLog("string:%s",tmp->getstring("content"));
				break;
			}
		case sChange:
			{
				int tiType = tmp->getint("type");
				CCLog("Change Type int:%i",tiType);
				switch (tiType)
				{
				case 0:			// type = 0 | <�ӳ٣�ÿ������һ���͹��ˡ�
					{
						ELock();
						m_pOwner->runAction(CCSequence::create(CCDelayTime::create(tmp->getfloat("delay")),FRELEASE,NULL));
						CCLog("delay:%f",tmp->getfloat("delay"));
						break;
					}
				case 1:			// type = 1 | <���й����ж������Ź��������� || ��ͨ������
					{
						BattleField::sharedBattleField()->Judge();

						((Entiles*) m_pOwner)->setState(3);
						if(((Entiles*) m_pOwner)->miHitFlag){
							((Entiles*) m_pOwner)->playAnimate("huixin",tmp->getint("repeat"));
						}else
						{
							((Entiles*) m_pOwner)->playAnimate("gongji",tmp->getint("repeat"));
						}
						break;					
					}
				case -1:		// type = -1 | <�����ж��������Ŷ����� || ������
					{
						BattleField::sharedBattleField()->Judge();
						((Entiles*) m_pOwner)->setState(3);
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
						BattleField::sharedBattleField()->PlayEffectSp("Images/bullet.png",((Entiles*) m_pOwner)->getPosition());
						break;
					}
				case 4:		// <��ʾ�ı�  <������
					{
						((Entiles*) m_pOwner)->ShowDamage();
						int dm = ((Entiles*) m_pOwner)->miDamage;
						Chara* tow = ((EChesses*) m_pOwner)->m_pChara;
						tow->sethp(tow->gethp()-dm);
						BattleField::sharedBattleField()->RefreshStats();
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
				case 6:	// type = 6 | <��ֵ���������ڷ�Hp����ֵ�仯
					{
						Chara* tow = ((EChesses*) m_pOwner)->m_pChara;
						string tname =  tmp->getstring("name");
						tow->setvalue(tname,tow->getvalue(tname)+tmp->getint("value"));
						BattleField::sharedBattleField()->RefreshStats();
						break;
					}
				case 7:	// type = 7 | <��תλ�ã����ڼ����е�λ˲���ƶ������ || ��ǰ��λ��ת || ���ڼ��ܵ�Ŀ����ת
					{
						// <ע��������õ�Ԫ�أ�ͨ������¼����ڱ�����ű�ǰ����Ե�λִ��changeFace();
						int dx = tmp->getint("dx");
						int dy = tmp->getint("dy");
						if(dx == 0 && dy == 0){  // <û���ṩ��תĿ�������½�����ת�� m_con_cur || ��ת��������ĸ�����
							GameManager::sharedGameManager()->sharedLogicCenter()->ml->bm->moveChessConcru((EChesses*) getOwner());
						}
						break;
					}
				case 8:	// type = 8 | <������λ�ĳ��� | һ���ɱ�������λ����
					{
						 //GameManager::sharedGameManager()->sharedLogicCenter()->ml->bm->ChangeFaceConcur((EChesses*) m_pOwner);	// <��������λת�򱾵�λ
						GameManager::sharedGameManager()->sharedLogicCenter()->ml->bm->ChangeAllFace();		// <�����Ҫ�������ٸ��ݲ���ϸ�֣���ʱȫ������
						break;
					}
				default:
					break;
				}
				break;
			}
		}
	}
	++miScriptCount;
	GoAHead();
}

void EChessComp::ELock()
{	
	++miELock;
	CCLog("Lock:%d",miELock);
}

void EChessComp::EUnLock()
{
	--miELock;
	CCLog("UnLock:%d",miELock);
	if(miELock<=0){
		CCLOG("-----------Script GoAhead--------------------");
		GoAHead();
	}
}

bool EChessComp::TestRange( CCPoint target )
{
	CCLog("[ECC]TestRange with pos:%d,%f",target.x,target.y);
	vector<int> ars;
	ars.push_back(9);			// <����ģʽ����������Ŀǰ��û������

	//GameManager::sharedLogicCenter()->ml->bm->m_controller = (EChesses*) m_pOwner;
	//GameManager::sharedLogicCenter()->ml->bm->set_mouse_range(1,ars);	// <Get&Set Range Paras	
	//GameManager::sharedLogicCenter()->ml->bm->draw_mouse_range(((EChesses*) m_pOwner)->pos);

	return GameManager::sharedLogicCenter()->ml->bm->f_RangeTest(1,ars,((EChesses*) m_pOwner)->pos,target);

}

/* <Ѱ�ҷ������� */
bool EChessComp::FindFitRe( CCObject* tar,int atime )
{
	/* <��ͨ���� - atime ���� */
	if(miReCount == 0) return false;	// <���������η�����ѯ || ���ε������Ȼ���չ�
	--miReCount;

	int t_iType = (((EChesses*) m_pOwner)->m_pChara)->getvalue("attack");		// <[TODO]�ӵ�λ�л�ù�������,�������Դ��Ҫ������ƽ��б����ע��Ĭ��ȡ�õ���0
	t_iType = 1;																// <[TestOnly] ʹ�������ȡ��ֵ

	vector<map<string,string>> vdata;
	vdata.clear();
	DBUtil::initDB("save.db");
	CCString* t_csSql = CCString::createWithFormat("select * from attr_attact where id = %d",t_iType);
	vdata = DBUtil::getDataInfo(t_csSql->getCString(),NULL);
	int m_number = vdata.size();
	DBUtil::closeDB(); 

	 t_ssm = (map<string,string>) vdata.at(0);	
			
	 if(TestRange(((EChesses*) tar)->pos)){
		 Scriptor* scp = new Scriptor();
		 scp->parse_string(t_ssm.at("action_sp"));
		 GameManager::sharedLogicCenter()->ml->bm->f_setcontroller((EChesses*) m_pOwner);
		 GameManager::sharedLogicCenter()->ml->bm->m_caTarget->removeAllObjects();
		 GameManager::sharedLogicCenter()->ml->bm->m_caTarget->addObject(tar);
		 GameManager::sharedLogicCenter()->ml->bm->HandleScriptor(scp);

		 scp->re_init();
		 delete scp;
		 return true;
	 }
	 return false;

	
}

void EChessComp::CleanRe()
{
	miReCount = 2;
}

int EChessComp::PreCheckRe( CCObject* tar,int atime )
{
	if(TestRange(((EChesses*) tar)->pos)){
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

