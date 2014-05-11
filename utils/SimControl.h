/*
*	-- SimControl
*	Entile类的控制接口，提供基本的地图操作。
*
*/

#ifndef	__SIM_CONTROL_H__
#define	__SIM_CONTROL_H__

#include "Box2D/Box2D.h"
#include "cocos2d.h"



USING_NS_CC;

class SimControl 
{
private:
	vector<int> m_viPath;
	
	int m_iHp;
	bool m_bHp;

public:
	int group_id,group_mask;		//确定敌对和友方势力 -- NPC|PC|ENEMY将使用同样的逻辑

	virtual void SCTarget(SimControl* tar) = 0;
	virtual void DecidePath(SimControl* tar) = 0;
	virtual b2AABB* f_search() = 0;
	virtual bool SCTouch(b2Fixture* self, b2Fixture* tar,bool bLeave = false) = 0;			//物体直接发生接触，目前将不会支持多定制器的情况，因为是用来做碰撞检测而已。
	virtual bool SCAttacted(SimControl* src) = 0;
	virtual bool SCSearched(SimControl* src) = 0;
	virtual bool SCContact(b2Fixture* self, b2Fixture* tar,bool bLeave = false) = 0;		//有物体与传感器发生了碰撞
	virtual bool SCContacted(b2Fixture* self,b2Fixture* tar,bool bLeave = false) = 0;		//与物体的传感器发生了碰撞

	virtual void SCGoto(CCPoint c) = 0;
	virtual void SCMoveto(CCPoint c) = 0;
	virtual CCPoint SCGetPosition() = 0;
};

#endif