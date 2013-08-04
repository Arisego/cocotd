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
	CC_SAFE_DELETE(mSp);
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
	case 1:			// <A* 移动被独立出来，因为没有办法被当做Action来处理
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
// <攻击

void EChessComp::GoAHead()
{
	//[0803]CCLog(">[ECC]Go aHead...");
	if(miELock>0) return;
	if(miScriptSum <= miScriptCount)
	{
		//[0803]CCLog(">[ECC]over:%d,%d",miScriptSum,miScriptCount);
		((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRelease();
		CC_SAFE_DELETE(mSp);
	}else{
		CCLog(">[ECC]step:%d/%d",miScriptSum,miScriptCount);
		DerScript((Script*) mSp->scriptnodes->objectAtIndex(miScriptCount));
	}
	
}

void EChessComp::RunScript( Script* asp )
{
	mSp = asp;
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
				case 0:			// type = 0 | Delay float
					{
						ELock();
						m_pOwner->runAction(CCSequence::create(CCDelayTime::create(tmp->getfloat("delay")),FRELEASE,NULL));
						CCLog("delay:%f",tmp->getfloat("delay"));
						break;
					}
				case 1:			// type = 1 | <进行攻击判定.
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
				case 2:			// type = 2.. <被攻击
					{
						((Entiles*) m_pOwner)->setState(3);
						/* <目前无格挡 */

						break;
					}
				case 4:		// <显示文本  <被攻击
					{
						((Entiles*) m_pOwner)->ShowDamage();
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
