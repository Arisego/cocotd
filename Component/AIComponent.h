#ifndef __AI_COMP_H__
#define __AI_COMP_H__

#include "cocos2d.h"
#include "packui\CharaS.h"
#include "cocoa\CCGeometry.h"

USING_NS_CC;

class AIComponent : public CCComponent
{
public:
	AIComponent();
	~AIComponent();

	void giveControl();		/* <��ÿ���Ȩ */
	Chara* mpChara;			/* <��ɫ��Ϣָ�� */

	void setTargetPoint(int ax, int ay);	/* <�趨[�ƶ�|����]Ŀ�� */
	void MoveOver();						/* <�ƶ��Ѿ���� */

	void SetNextSkill(int ais);				/* <�����´ι������� */
	int	 getNextSkill();					/* <������ */
	set<int> msSkillDis;					/* <�Ѿ�����ļ����б� */
	
	void ActOver();							/* <���֪ͨ�ӿ� || �ⲿ���� */
	void setSkillTargetPoint( int tiX, int tiY );	/* <���ü���Ŀ��� */
private:
	void releaseControl();	/* <�ͷſ���Ȩ */
	int miAtx, miAty;		/* <�ƶ�Ŀ�� */

	int miNextSkill;		/* <�´ι������� */
	CCPoint miSkillP;

	void UseSkill();		/* <����ʹ�õ���� */
	void ActSkill();		/* <����ʹ�õĳ��� */
public:
	void ActAttact(const char* aScp);		/* <������ͨ���� */

	static int miDebugCount;
	int miState;			/* <״̬���� || ������ */

private:
	void SkillOver();		/* <������ɵĽӿ� */
	bool CheckXuDong();		/* <����Ƿ������������� */

	void GiveOut();			/* <AI����� */
	
	bool mbMoving;			/* <AI����ִ���ƶ�ָ�� */
	
};


#endif