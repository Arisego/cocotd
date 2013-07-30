#include "MapLayerComp.h"

#include "sublayer/MapLayer.h"

MapLayerComp::MapLayerComp()
{
	m_strName = "controller";
}

void MapLayerComp::ActRetain()
{
	++miActCount;
}

void MapLayerComp::ActRelease()
{
	--miActCount;
	if(miActCount <= 0){
		MlUnLock();
	}
}

void MapLayerComp::MlLock(){
	((MapLayer*) m_pOwner)->f_stopcontrol();
}

void MapLayerComp::MlUnLock()
{
	((MapLayer*) m_pOwner)->f_resumecon();
}
