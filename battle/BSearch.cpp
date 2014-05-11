#include "BSearch.h"

void BSearch::clear_plist()
{
	m_divPoint.clear();
}

void BSearch::inval_plist(int ai, int aj){
	m_divPoint.push_back(std::make_pair(ai, aj));
}

void BSearch::clear()
{
	clear_plist();
	miV = 0;
}

void BSearch::getFront(int &ax, int &ay)
{
	CCLog(">[BSearch] getFront() | [%d/%d]", miV, m_divPoint.size());
	if(miV >= m_divPoint.size()) return;

	ax = m_divPoint[miV].first;
	ay = m_divPoint[miV].second;
}


void BSearch::stepFront()
{
	++miV;
	CCLog(">[BSearch] stepFront() | %d/%d", miV, m_divPoint.size());
}


bool BSearch::hasFront()
{
	if(miV < m_divPoint.size()){
		CCLog(">[BSearch] hasFront() | %d/%d", miV, m_divPoint.size());
		miCpx = m_divPoint[miV].first;
		miCpy = m_divPoint[miV].second;
		
		return true;
	}
	m_divPoint.clear();
	return false;
}

int BSearch::getCount()
{
	return miV;
}

MoveDirect BSearch::getNextDirect(int atx, int aty)
{
	if(miV >= m_divPoint.size()) return MS_DOWN;
	CCLog(">[BSearch] getNextDirect( %d/%d) || [%d, %d], [%d, %d]",miV, m_divPoint.size(), atx, aty, m_divPoint[miV].first, m_divPoint[miV].second );

	if(aty >m_divPoint[miV].second) {
		CCLog("Up");
		return MS_UP;
	}else if(aty < m_divPoint[miV].second){
		CCLog("Down");
		return MS_DOWN;
	}

	if(atx > m_divPoint[miV].first){
		CCLog("Left");
		return MS_LEFT;
	}else {
		CCLog("Right");
		return MS_RIGHT;
	}

}
