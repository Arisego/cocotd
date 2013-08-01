#ifndef __ECHESS_COMP_H__
#define __ECHESS_COMP_H__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "utils/Scriptor.h"

USING_NS_CC;
USING_NS_CC_EXT;

class EChessComp : public CCComponent 
{
public:
	EChessComp();
	~EChessComp();

	/* <根据指定的网格坐标序列进行移动 */
	void move_by_path(std::vector<CCPoint> &vpath);
	virtual void update(float delta);

	/* <运行指定的脚本 */
	void RunScript(Script* asp);		// <不要对同一个个体添加两种脚本效果，结果是不可预知的。如果要那么做，可以在这个函数里创建新的自销毁Component。

private:
	int miScriptSum;
	void GoAHead();
	Script* mSp;

	int miStateFlag;
	std::vector<std::pair<float,float>> mPath;

};


#endif