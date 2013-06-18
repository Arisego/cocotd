#ifndef __TO_EQUIPS_H__
#define __TO_EQUIPS_H__

#include "packui/Container.h"
#include "packui/TOTabOrder.h"
#include "CharaS.h"
/*
	TOEquips
	<装备栏单体
*/
class TOEquips : public Container
{
private:
	CCLabelTTF* mLT_Name;
	CCLabelBMFont* mLB_NJFront;
	CCLabelBMFont* mLB_NJBack;

public:
	virtual void onNormal();
	virtual void onHover();
	virtual void onSelect();
	virtual void onDisable();

	//////////////////////////////////////////////////////////////////////////
	Equip* eq;

	TOEquips();
	~TOEquips();

	void setcontent(Equip* aeq);

};

#endif