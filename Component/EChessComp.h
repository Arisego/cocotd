#ifndef __ECHESS_COMP_H__
#define __ECHESS_COMP_H__

#include "cocos2d.h"
#include "cocos-ext.h"

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

private:
	int miStateFlag;
	std::vector<std::pair<float,float>> mPath;

};


#endif