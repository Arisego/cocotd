#ifndef __LOGIC_TREE_H__
#define __LOGIC_TREE_H__

/*
	<胜败条件用
*/

#include <vector>
#include "utils\Scriptor.h"
#include <string>

// <树的数据结构
class LogicCell
{
public:
	LogicCell();
	~LogicCell();

	std::string	msText, msName;
	int	miType;	// <节点类型 | 0-叶子节点 | 1-和节点 | 2-或节点 |
	std::vector<LogicCell*> mvChilds;	// <存储子节点
	
	bool isTrue;
	bool checkValue();

};


#endif