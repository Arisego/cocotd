#ifndef __LT_LIFE_BLOCK_H__
#define __LT_LIFE_BLOCK_H__

#include "byeven/BYCocos.h"
#include "packui/LifeBar.h"
#include "packui/CharaS.h"
//LifeTop Life Block
class LTLifeBlock : public BYLayerDescendant
{
public:
	~LTLifeBlock();

	virtual bool init();
	void setChara();
	void RefreshValue();

	void onShow();
	void onDismiss();
	Chara* mpChara;
private:
	

	LifeBar* m_lbhp;
	LifeBar* m_lbmp;
	CCSprite* m_avatar;
};


#endif