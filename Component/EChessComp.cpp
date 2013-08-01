#include "EChessComp.h"
#include "Component/MapLayerComp.h"

#include "utils/Entiles.h"
#include "GameManager.h"

EChessComp::EChessComp()
{
	m_strName = "controller";
	miStateFlag = 0;
	miScriptSum = 0;
	mSp			= NULL;
}

EChessComp::~EChessComp()
{
}

void EChessComp::move_by_path(std::vector<CCPoint> &vpath )
{
	mPath.clear();
	for(std::vector<CCPoint>::iterator it = vpath.begin(); it != vpath.end(); ++it){
		mPath.push_back(std::make_pair(it->x,it->y));
		CCLOG("WTF:%f,%f",it->x,it->y);
	}
	miStateFlag = 1;
	miScriptSum = 1;

	((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->MlLock();
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
					GoAHead();
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
	--miScriptSum;
	if(miScriptSum<=0)
	{
		((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->MlUnLock();
		if(mSp) delete mSp;
	}
}

void EChessComp::RunScript( Script* asp )
{
	mSp = asp;
	miScriptSum = mSp->m_snum;

	CCLog(">[ECC]Script Ready:%d",miScriptSum);
}
