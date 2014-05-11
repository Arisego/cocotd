#ifndef __B_SEARCH_H__
#define __B_SEARCH_H__

/*
	<BSearch
		< 保留战斗中的状态信息
		< EXP: 矿石-君临
		< 脚本用
*/

#include <vector>
#include "utils\Entiles.h"

class BSearch
{
private:
	std::vector<std::pair<int,int>> m_divPoint;
	int miV;
	int miCpx, miCpy;


public:
	void clear_plist();
	void inval_plist(int ai, int aj);
	void clear();

	void getFront(int &ax, int &ay);
	bool hasFront();
	void stepFront();

	int getCount();

	MoveDirect getNextDirect(int atx, int aty);

};

#endif