#ifndef __BATTLE_FIELD_H__
#define __BATTLE_FIELD_H__

#include "cocos2d.h"
#include "utils\EChess.h"
#include "utils\Scriptor.h"
#include "TOUI\LogicTree.h"

/*
	<战场管理单例
	-<当标记被置为OVER时会导致ML结束BM，注意添加相关显示。
*/
class BattleField
{
public:
	static BattleField *sharedBattleField();
	static void purgeSharedBattleField();

	~BattleField();
	void Clean();

	bool IsOver();
	//////////////////////////////////////////////////////////////////////////
	// <战场控制
	map<pair<int,int>,EChesses*> mMapC;							// <战场位置登记
	void SetChess(EChesses* ae,int ax, int ay);					// <设置人物 | 初始化人物时调用
	void InitBattleFiled();										// <BattleFiled内部初始化 | 数值
	void InitChessRefreh();										// <刷新人物 | 初始化完成时调用
	void ChessMoved(EChesses* ae, CCPoint astart, CCPoint aend); // <人物移动，调整统帅数值
	void Judge(Script* ascp = NULL);		// <判定 || 组号 | 伤害 | 类型

	bool HasChess(int ax, int ay);								// <返回一个点是否存在人物

	//////////////////////////////////////////////////////////////////////////
	// <战斗控制
	void SetUp(EChesses* aSrc, CCArray* aTar,Script* sp = NULL);		// <战斗设置接口
	void AMOver();														// <一次攻击结束后发起战场检查

	void SetSrc(EChesses* aSrc);
	void SetTars(CCArray* aTar);

	void PlayEffectSp(CCObject* echesscom, const char* asname);								// <生成一个Sprite，并移动到指定的点。
	void PlayEffectSpFr(CCObject* tmpOwner, Script* sp);									// <生成一个Sprite(Frame)

	//////////////////////////////////////////////////////////////////////////
	// <再次攻击
	EChesses* BackChess1;
	EChesses* BackChess2;
	EChesses* BackChess3;

	void CheckBackCh();		// <从EChessTar中读取可以反击的三个单位
	bool TestBackCh(EChesses* atar);		// <检测是否可以反击并进行反击

	bool LogicContinue();
	bool NormalAttackC();	// <普通攻击的检测
	int	 miState;			// <战斗阶段标志位 0 - 等待生成反击组 || 1 - 对反击组进行遍历询问 || 2 - 反击组二次反击 

	//////////////////////////////////////////////////////////////////////////
	// <动态伤害预判
	void setBattle(bool ab);
	bool mbIsInBattle;		// <是否需要进行战斗预判

	void PreJudge(EChesses* atar);

	//////////////////////////////////////////////////////////////////////////
	// <地图脚本
	void ClearBF();				// <结束后打扫战场
	void InitBfSp(const char* pSz);

	Scriptor* mSpLinker;	// <解析器
	void ShowChess(EChesses* atar);

protected:
	virtual bool init();
	void CheckOver();								// <检查是否满足结束条件

	void DepRemoveLead( int centx, int centy, int range, int val );
	void DepLead( int centx, int centy, int range, int val );

	void SetLead(EChesses* te);
	int DerLead( int val,int cx, int cy,int dx, int dy );	// <根据val的值来判定得到的统帅加成
	void DerLead( int val,int cx, int cy );					// <单体扩散
	void UnDerLead( int val,int cx, int cy );				// <移除自身的影响
	
	//////////////////////////////////////////////////////////////////////////
	// <战斗显示
	int miSFlag;	// <标记战斗运行状态 || 0 - Src未未设置 | 1 -Src已经设置 等待可能的开始和判定 | 2 - 战斗经过第一次判定 <<< 以后的判定可以和mistate结合
	
public:
	bool mbIsMagic; // <是否是魔法攻击
	void RefreshStats();	// <更新两边显示
	EChesses* meTarget;		// <当前被用于显示的右边单位

	void ActionFac();		// <动作被实际执行
	bool CalRate(EChesses* tar_o,int hit_rate_base);
	//void CheckBlood();		// <对血量进行检查

	EChesses* meOrig;		// <保存必要的指针
public: 
	EChesses* meSrc;		// <攻击的发起者	| 被攻击对象在Chara内结算
private:
	CCArray* meTar;			// <接收
	Script*	 mspVals;		// <单次战斗的额为数据入口

	CCDictionary* mSpEvList;// <事件列表

	static BattleField *mSharedBattleField;
	bool mbIsOver;
	//int miEnemy;
	void setMeTar(CCArray* atar); // <meTar的封装接口
public:
	//////////////////////////////////////////////////////////////////////////
	// <技能接续
	bool fGetCont( const char* sname);

	//////////////////////////////////////////////////////////////////////////
	// <一轮交互结束
	map<pair<int,int>,EChesses*>::iterator m_EIt;		// <遍历棋子数据的迭代器
	void InitCheck();									// <初始化检查
	bool ActChack();									// <动画播放是需要时间的。
	void UnitDead();									// <有单位死亡了，进行处理。

	void ActOver();										// <一轮交互的结束通知到达点

	//////////////////////////////////////////////////////////////////////////
	// <经验计算
	bool ECIsSrc(EChesses* aTar);
	bool ExpCheck();									// <检查经验值
	void ExpShow();							// <显示经验值

	//////////////////////////////////////////////////////////////////////////
	//

	void InitBF();							// <脚本相关初始化操作 | 将BF纳入脚本操控范围之内

	// <胜败条件
	LogicCell* mlp_Win;
	LogicCell* mlp_Lose;
private:
	Script* Search_script(const char* asScp);
	void	Ex_script(Script* aScp, const char* asScp);

	set<string> mss_Win, mss_Lose;			// <防止map无端的变长
	map<string,LogicCell*> msb_Win, msb_Lose;		// <胜败条件

	void ReadLogicPointScpW(LogicCell* atar, Script* ascp);
	void ReadLogicPointScpL(LogicCell* atar, Script* ascp);

	void onEvent(const char* asEventName);	// <事件处理的核心接口

public:
	void onDeadEvent(EChesses* tar);		// <单位完成死亡判定和清理 | 从BF中解除登记
	
	//////////////////////////////////////////////////////////////////////////
	// <存档读档

	void setSaveState();					// <设置存档状态 | 使得WM能够正常保存
	void setLoadState();					// <设置读档状态 | 在读档前进行清理工作
	void unsetSaveState();					// <解除存档状态 | 防止对游戏循环造成影响
	void Load_Script(Script* ascp);			// <战斗读档接口 | 不由MapLayer负责 | 【注意】战斗中大部分情况下是禁止存档的
	void Load_Over();						// <读档完成接口 | StateCenter读档完成后调用，防止出现wm和bm的冲突

	bool mbNeedSave;						// <是否需要存档 | 不初始化在setSaveState中修改 | 同时承担读档的标志

	int miActCount;							// <单轮攻击中的计数器

public:	
	void ClearTrigFlag();					// <清除触发标记
	bool mbTrigged;							// <触发是否被触发

	void Trigged();										// <触发被触发时调用
	int TestPrior(CCObject* acSrc = NULL, CCObject* acTar = NULL);	// <获得两个单位的速度优先级 | Chara
	int DerLocks(CCObject* acSrc, CCObject* acTar, const char* asLScp);			// <检查技能是否可触发 | 锁
	bool PUSkillFilt(CCObject* acSrc, CCObject* acTar, int aiFlag, const char* asmask = "");				// <从src到tar限定flag的技能列表生成和调用 | 触发|反击|

	//void CleanAllAct();		// <尝试清理所有的脚本造成的影响 | 这不是一个结束清理函数
	Script* mCachedJudge;	// <每次判定被暂停时的缓存

	void ResumeJudge();		// <如果有被停掉的Judge则恢复那次判定
	void JudgeEnti(Script* asp);		// <Judge判定的实体

	//std::string msCacheSk;
	//void PunpOut();				// <制造间隙后弹出菜单用的函数
	void ActSkill(CCObject* src, Script* asp, CCObject* atar = NULL);					// <某个单位发起技能释放
	CCObject* mCachedChess;			// <被缓存的棋子

	int getDisST();					// <获得距离 Src - Tar | 使用Tar列表的第一个单位为标准

	void PlaySpxEC(CCObject* atar, Script* asp);		// <在某个棋子上播放指定的SPX动画

	CCObject* meConS;			// <被锁定的控制器

	bool mbNeLock;				// <是否需要进行MLC解锁操作

	map<int, CCPoint> mCachedSPp;	// <保存技能判定中的作用点
	map<int, int>	mCachedSPpScs;	// <AI用，对应技能的评分

	CCObject* meCtnTarget;			// <接续的目标 | 实际类型 EChess || CCArray？
private:
	bool CheckSkill(CCObject* acSrc, CCObject* acTar, int sid);	
	CCObject* meLastRe;				// <最后一个反击单位
	int CalDamage(Script* asp);		// <伤害计算

public:
	void PlaySpxPo(CCPoint apos, Script* asp);	// <在指定的位置播放动画

	int miSeType;		// <控制音效的跳转

	bool mbCurMagic;	// <当前的脚本是否是魔法
	void PlaySe(CCObject* aeChara);	// <播放音效 | Chara

	int	miLastSkillID;			// <上一个触发的技能ID

	void ClearReASkill();		// <反击技能生效时清除操作
};


#endif