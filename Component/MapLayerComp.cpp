#include "MapLayerComp.h"

#include "sublayer/MapLayer.h"

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
	CCLog(">[MLC]Retain one lock:%d",miActCount);
}

void MapLayerComp::ActRelease()
{
	--miActCount;
	CCLog(">[MLC]Release one lock:%d",miActCount);
	if(miActCount <= 0){
		MlUnLock();
	}
}

void MapLayerComp::MlLock(){
	CCLog(">[MLC]MlLock:%d",miActCount);
	((MapLayer*) m_pOwner)->f_stopcontrol();
}

void MapLayerComp::MlUnLock()
{
	CCLog(">[MLC]MlUnLock:%d",miActCount);
	((MapLayer*) m_pOwner)->f_resumecon();
}
