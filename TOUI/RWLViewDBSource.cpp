#include "RWLViewDBSource.h"
#include "SingleTon\BattleFiled.h"

RWLViewDBSource::~RWLViewDBSource(){
	vdata.clear();
	data->release();
}

RWLViewDBSource::RWLViewDBSource(){
	data = NULL;
}

void RWLViewDBSource::ReadLgCell(std::string asHead, LogicCell* algc)
{
	if(0 == algc->miType){
		CCLabelTTF* tlt = CCLabelTTF::create((asHead+algc->msText).c_str(), FNT_UI_LABEL, 24,CCSize(rw,0), kCCTextAlignmentLeft);
		tlt->retain();
		m_plength.push_back(tlt->getContentSize().height);
		CCLog("+++++++%f", tlt->getContentSize().height);
		m_height += tlt->getContentSize().height;
		data->addObject(tlt);		

		asHead += " ";
		return;
	}


	for(std::vector<LogicCell*>::iterator it = algc->mvChilds.begin(); it != algc->mvChilds.end(); ++it){
		if((*it)->miType == 0){
			asHead += " ";
			break;
		}
	}

	for(std::vector<LogicCell*>::iterator it = algc->mvChilds.begin(); it != algc->mvChilds.end(); ++it){
		ReadLgCell(asHead.c_str(), *it);
	}

}

void RWLViewDBSource::init_data(float width){
	int o_iid = 1;

	char * errMsg = NULL;
	std::string sqlstr;
	vdata.clear();
	m_plength.clear();
	m_height = 0;
	CC_SAFE_RELEASE_NULL(data);

	rw = width;

	data = CCArray::create();

	switch (miSBType)
	{
	case(0):
		{
			/*CCLabelTTF* tlt = CCLabelTTF::create("Win:", FNT_UI_LABEL, 26,CCSize(rw,0), kCCTextAlignmentLeft);
			tlt->retain();
			m_plength.push_back(tlt->getContentSize().height);
			CCLog("+++++++%f", tlt->getContentSize().height);
			m_height += tlt->getContentSize().height;
			data->addObject(tlt);	*/
			ReadLgCell(" ", BattleField::sharedBattleField()->mlp_Win);
			break;
		}
	case(1):
		{
/*			CCLabelTTF* tlt = CCLabelTTF::create("Win:", FNT_UI_LABEL, 26,CCSize(rw,0), kCCTextAlignmentLeft);
			tlt->retain();
			m_plength.push_back(tlt->getContentSize().height);
			CCLog("+++++++%f", tlt->getContentSize().height);
			m_height += tlt->getContentSize().height;
			data->addObject(tlt);*/	
			ReadLgCell(" ", BattleField::sharedBattleField()->mlp_Lose);
			break;
		}
	default:
		break;
	}



	m_number = data->count();
	data->retain();
	DBUtil::closeDB(); 

}

unsigned int RWLViewDBSource::numberOfCellsInTableView(CCTableView *table){
	return m_number;
}

CCSize RWLViewDBSource::getwholesize(){
	return CCSize(rw,m_height);
}

float RWLViewDBSource::offsetfromidx(int idx){
	float th = 0.0;
	for(int i=0;i<=idx;i++){
		th += m_plength[i];
	}

	return m_height - th;
}

int RWLViewDBSource::indexfromoffset(float hei){	//input the origin from tableview,do not modify it in tableview again;
	float dxh = m_height - hei;

	int idx = 0;
	int i=0;
	for(;i<m_number;i++){
		dxh -= m_plength[i];
		if(dxh<0) break;
	}

	//		if(dxh<0)					//完整性检查，防止越界  xxx 会破坏tableview本身的保护机制
	return i;


}

cocos2d::CCSize RWLViewDBSource::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
	return CCSize(rw,m_plength[idx]);
}
