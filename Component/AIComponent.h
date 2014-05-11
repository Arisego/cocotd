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

	void giveControl();		/* <获得控制权 */
	Chara* mpChara;			/* <角色信息指针 */

	void setTargetPoint(int ax, int ay);	/* <设定[移动|技能]目标 */
	void MoveOver();						/* <移动已经完成 */

	void SetNextSkill(int ais);				/* <设置下次攻击技能 */
	int	 getNextSkill();					/* <返回下 */
	set<int> msSkillDis;					/* <已经否决的技能列表 */
	
	void ActOver();							/* <完成通知接口 || 外部调用 */
	void setSkillTargetPoint( int tiX, int tiY );	/* <设置技能目标点 */
private:
	void releaseControl();	/* <释放控制权 */
	int miAtx, miAty;		/* <移动目标 */

	int miNextSkill;		/* <下次攻击技能 */
	CCPoint miSkillP;

	void UseSkill();		/* <技能使用的入口 */
	void ActSkill();		/* <技能使用的出口 */
public:
	void ActAttact(const char* aScp);		/* <进行普通攻击 */

	static int miDebugCount;
	int miState;			/* <状态变量 || 监视用 */

private:
	void SkillOver();		/* <技能完成的接口 */
	bool CheckXuDong();		/* <检查是否满足续动条件 */

	void GiveOut();			/* <AI活动结束 */
	
	bool mbMoving;			/* <AI正在执行移动指令 */
	
};


#endif