#include "EChessComp.h"

EChessComp::EChessComp()
{
	m_strName = "controller";
	miStateFlag = 0;
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
	//CCLog(">Path is ready for component:%d",mPath.size());
}

void EChessComp::update(float delta){
	//CCLog(">?Who update this.?");
	switch (miStateFlag)
	{
	case 1:
		{

			break;
		}

	default:
		break;
	}
}