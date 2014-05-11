#include "LogicTree.h"


LogicCell::~LogicCell()
{
	for(std::vector<LogicCell*>::iterator it = mvChilds.begin(); it != mvChilds.end(); ++it){
		delete(*it);
	}
	mvChilds.clear();
}

LogicCell::LogicCell(){
	miType = 0;
	isTrue = false;
}

bool LogicCell::checkValue(){
	if(isTrue) return true;
	bool ret = true;
	switch(miType){
	case(0):	// LEAF
		{
			CCLog(">[LogicTree] Check | %s | %d", this->msText.c_str(), isTrue);
			return isTrue;		
		}
	case(1):	// AND
		{
			CCLog(">[LogicTree] Check | --AND--");
			for(std::vector<LogicCell*>::iterator it = mvChilds.begin(); it != mvChilds.end(); ++it){
				if((*it)->checkValue()) continue;
				ret = false;
				break;
			}
			break;
		}
	case(2):	// OR
		{
			CCLog(">[LogicTree] Check | --OR--");
			for(std::vector<LogicCell*>::iterator it = mvChilds.begin(); it != mvChilds.end(); ++it){
				if((*it)->checkValue()) {
					ret = false;
					break;
				}
			}
			ret = !ret;
			break;
		}
	default:
		{
			break;
		}
	}	
	return ret;
}

