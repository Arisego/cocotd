/*
*	-- SimControl
*	Entile��Ŀ��ƽӿڣ��ṩ�����ĵ�ͼ������
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
	int group_id,group_mask;		//ȷ���жԺ��ѷ����� -- NPC|PC|ENEMY��ʹ��ͬ�����߼�

	virtual void SCTarget(SimControl* tar) = 0;
	virtual void DecidePath(SimControl* tar) = 0;
	virtual b2AABB* f_search() = 0;
	virtual bool SCTouch(b2Fixture* self, b2Fixture* tar,bool bLeave = false) = 0;			//����ֱ�ӷ����Ӵ���Ŀǰ������֧�ֶඨ�������������Ϊ����������ײ�����ѡ�
	virtual bool SCAttacted(SimControl* src) = 0;
	virtual bool SCSearched(SimControl* src) = 0;
	virtual bool SCContact(b2Fixture* self, b2Fixture* tar,bool bLeave = false) = 0;		//�������봫������������ײ
	virtual bool SCContacted(b2Fixture* self,b2Fixture* tar,bool bLeave = false) = 0;		//������Ĵ�������������ײ

	virtual void SCGoto(CCPoint c) = 0;
	virtual void SCMoveto(CCPoint c) = 0;
	virtual CCPoint SCGetPosition() = 0;
};

#endif