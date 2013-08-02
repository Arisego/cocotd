#include "EChessComp.h"
#include "Component/MapLayerComp.h"

#include "utils/Entiles.h"
#include "GameManager.h"

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
	CCLog(">Path is ready for component:%d",mPath.size());
}

void EChessComp::update(float delta){
	//CCLog(">?Who update this.?");
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
	CCLog(">[ECC]Go aHead...");
	if(miELock>0) return;
	if(miScriptSum <= miScriptCount)
	{
		CCLog(">[ECC]over:%d,%d",miScriptSum,miScriptCount);
		((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRelease();
		CC_SAFE_DELETE(mSp);
	}else{
		CCLog(">[ECC]step:%d,%d",miScriptSum,miScriptCount);
		DerScript((Script*) mSp->scriptnodes->objectAtIndex(miScriptCount));
	}
	
}

void EChessComp::RunScript( Script* asp )
{
	mSp = asp;
	setScriptNum(mSp->m_snum);
	miELock = 0;
	CCLog(">[ECC]Script Ready:%d",miScriptSum);
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
	int tiSum = asp->getint("total");
	CCArray* acts = asp->scriptnodes;
	miELock = 0;
	//CCLOG("script handle.");
	for (int i = 0;i<tiSum;i++)		//multi here?
	{
		//CCLOG("handle scripte:%d",i);
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
				CCLog("int:%i",tiType);
				switch (tiType)
				{
				case 0:			// type = 0 | Delay float
					{
						ELock();
						m_pOwner->runAction(CCSequence::create(CCDelayTime::create(tmp->getfloat("delay")),FRELEASE,NULL));
						CCLog("delay:%f",tmp->getfloat("delay"));
						break;
					}
				case 1:			// type = 1 | Play animation Following should work together with delay.
					{
						((Entiles*) m_pOwner)->setState(3);
						((Entiles*) m_pOwner)->playAnimate(tmp->getstring("anime"),tmp->getint("repeat"));
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
		GoAHead();
	}
}
