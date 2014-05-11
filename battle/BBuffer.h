#ifndef __B_BUFFER_H__
#define __B_BUFFER_H__

/*
	< BBuffer | 所有buffer类特效
*/

#define TR_OPEN		0	/* <Buffer加载时触发 */
#define TR_ROUND	1	/* <回合切换开始时触发 */
#define ATK_FIREST	2	/* <第一次攻击 是 普通攻击 时触发【攻击后】 */
#define TR_DAMAGE	3	/* <对别的单位造成伤害(Judge) */
#define TR_ATKED	4	/* <被攻击，反击前 */
#define TR_HP		5	/* <HP发生变动 | 不是死亡专用 */
#define TR_ATKOVER	6	/* <EchessComp 一组动作播放完成 */
#define TR_MISS		7	/* <发生闪避时 */
#define TR_COUTDOWN	8	/* <倒数到零发生|仅在BUFFER内部使用 */
#define TR_BEHURT	9	/* <被发起伤害判定 | 唤醒 */
#define TR_BEGIN	10	/* <开始一轮战斗时触发 【SetBat】 */

#define TR_GEDANG	11	/* <格挡判定前 */
#define TR_BEDAMA	12	/* <伤害显示前 |显示 单位| 强制闪避 */
#define TR_SRDAMA	13	/* <伤害判定前 |Egroup-FactDamage  |攻击 单位| 致死 */
//#define TR_BSDAMA	14  /* <伤害判定前 |Entile-ShowDamage  |攻击 单位 */
#define TR_DEATH	15  /* <单位被杀死 |Entile-ShowDamage  |攻击 单位 */

#include "cocos2d.h"
#include "BChara.h"
#include <map>
#include "utils\Scriptor.h"
USING_NS_CC;

class BBuffer : public CCObject
{
public:
	Chara* mpChara;
	~BBuffer();

	BBuffer(const char* scp, Chara* apc);

	void Trig(int ai);
private:
	Scriptor* mscp;

	std::map<int, Script*> misStates;
	std::map<string, int> miAttrs;

	std::map<int, Script*> miscTrigger;

	void RegTrigger(int ai);			// <注册触发器
	std::map<int, CCObject*>	mTrigerCache;	// <触发器缓存

	void RefreshAttrs(const char* ast);	// <更新数值对表

	int miState;
	
public:
	int miCountDown;					// <用于计数累计

	int miId;							// <BufferID
	int getState();						// <获得状态值

	CCObject* mcTarget;					// <缓存的目标
	
};

#endif