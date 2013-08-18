#include "MapLayerComp.h"

#include "sublayer/MapLayer.h"
#include "SingleTon/BattleFiled.h"

MapLayerComp::MapLayerComp()
{
	m_strName = "controller";
	miActCount = 0;
}

void MapLayerComp::ActRetain()
{
	
	if(miActCount<=0){
		miActCount = 0;
		MlLock();
	}
	++miActCount;
	//[0803]CCLog(">[MLC]Retain one lock:%d",miActCount);
}

void MapLayerComp::ActRelease()
{
	--miActCount;
	//[0803]CCLog(">[MLC]Release one lock:%d",miActCount);
	if(miActCount <= 0){
		if(BattleField::sharedBattleField()->LogicContinue()) return;
		MlUnLock();
	}
}

void MapLayerComp::MlLock(){
	//[0803]CCLog(">[MLC]MlLock:%d",miActCount);
	((MapLayer*) m_pOwner)->f_stopcontrol();
}

void MapLayerComp::MlUnLock()
{
	//[0803]CCLog(">[MLC]MlUnLock:%d",miActCount);
	((MapLayer*) m_pOwner)->f_resumecon();
}


