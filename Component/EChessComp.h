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

	/* <根据指定的网格坐标序列进行移动 */
	void		move_by_path(std::vector<CCPoint> &vpath);
	virtual void 
				update(float delta);

	/* <运行指定的脚本 */
	void		RunScript(Script* asp);		// <不要对同一个个体添加两种脚本效果，结果是不可预知的。如果要那么做，可以在这个函数里创建新的自销毁Component。
	void		setScriptNum(int ai);
	void		DerScript(Script* asp);

	/* <内部锁 */
	int			miELock;
	void		ELock();
	void		EUnLock();

	/* <寻找合适的RE攻击 */
	bool		FindFitRe(CCObject* tar,int atime);
	void		CleanRe();
	int			PreCheckRe(CCObject* tar,int atime);

	void		DelayUnLock(float dt); // <延迟然后释放持有的锁，缓冲游戏循环。
	int			HaveAI();				// <是否含有AI

	int			miReCount; // <反击次数监视
private:
	int			miScriptSum;
	int			miScriptCount;
	void		GoAHead();
	Script*			mSp;

public:
	/* <范围测定 */
	bool		TestRange(CCObject* tar, bool acat = false);
private:
	map<string,string> 
				t_ssm;
	
	int			miStateFlag;
	std::vector<std::pair<float,float>> 
				mPath;

	void		ChessDead();
	bool		mbChessDead;

public:
	int			miReState;		// <反击标志 [0-还没有 | 1-在反击 | 2-反击完了]

	void		onChessDead();
	void		onAddExp();

	void		CheckAlive();	// <如果被放置了死亡标志，则自杀
	void		DelayPlay();	// <延迟播放声音

	bool		mbDebugS;		// <是否显示调试信息

	bool		mbThinked;	// <是否弹出决策并等待
private:
	CCArray*	mcaMemories;	// <保存ECC中运行的脚本状态
	CCObject*	mcesCurrent;	// <当前状态指针 | ECCState

	void		PopState();		// <尝试检测是否有保存的State
	string		slatst;

public:
	bool		AtkTestRange(CCPoint target);	// <预判时使用，仅做预判用，不修改cs_y。
	int			getCurGrp();					// <获得当前的HitGroup
	void		setSrc(CCObject* teSrc);		// <设置脚本伤害来源
	CCObject*	getSrc();						// <与上面的相对
};


#endif