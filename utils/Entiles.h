#ifndef __ENTILES_H__
#define __ENTILES_H__

#include "cocos2d.h"

#include "utils/SimControl.h"
#include "Macros.h"

USING_NS_CC;

#define PTM_RATIO 32 
#define S_WID 0.5
#define S_HEI 0.25
 
typedef enum{
	MS_LEFT,
	MS_RIGHT,
	MS_UP,
	MS_DOWN,
	MS_STOP
} MoveDirect;

//所有地图实体的基类
//保证实体类的单纯性，实体类不负责进行HUD显示，所有HUD相关事宜由ML和TM处理。
class Entiles : public CCNode, public SimControl
{
public:
	b2Body *m_body;
	string name,group,psz;
	CCSprite *m_sprite;

	bool b_CanMove;
	MoveDirect direc;
	MoveDirect stand;
	bool b_Dirty,b_IsControler,b_Re;
	string m_sTarget;
	int state;
	/*
	| b_dirty 位置发生了变更，需要重绘
	| b_iscontroler 当前entiles获得了控制
	| b_re 是否是可移动的物体，需要被重新计算zorder
	*/
	Entiles();
	~Entiles();

	virtual void initFiles(const char *pszFileName, const CCRect& rect);			//Do not use this.It now calls initwithfile() and is not compatible with CCSpriteFrameCache. In fact you do not need an additional rect with SpriteFrameCache.
	virtual void initFiles(const char *pszFileName);

	virtual void lin();
	virtual b2AABB* f_search();

	virtual void SCTarget(SimControl* tar);
	virtual void DecidePath(SimControl* tar);
	virtual bool SCTouch(b2Fixture* self, b2Fixture* tar,bool bLeave = false);			//物体直接发生接触，目前将不会支持多定制器的情况，因为是用来做碰撞检测而已。
	virtual bool SCAttacted(SimControl* src);
	virtual bool SCSearched(SimControl* src);
	virtual bool SCContact(b2Fixture* self, b2Fixture* tar,bool bLeave = false);		//有物体与传感器发生了碰撞
	virtual bool SCContacted(b2Fixture* self,b2Fixture* tar,bool bLeave = false);						//与物体的传感器发生了碰撞

	virtual void SCGoto(CCPoint c);
	virtual void SCMoveto(CCPoint c);
	virtual CCPoint SCGetPosition();

	virtual void ELoad();
	string GetFullName();
	//////////////////////////////////////////////////////////////////////////



};

#endif