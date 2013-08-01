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

	/* <����ָ���������������н����ƶ� */
	void move_by_path(std::vector<CCPoint> &vpath);
	virtual void update(float delta);

	/* <����ָ���Ľű� */
	void RunScript(Script* asp);		// <��Ҫ��ͬһ������������ֽű�Ч��������ǲ���Ԥ֪�ġ����Ҫ��ô������������������ﴴ���µ�������Component��

private:
	int miScriptSum;
	void GoAHead();
	Script* mSp;

	int miStateFlag;
	std::vector<std::pair<float,float>> mPath;

};


#endif