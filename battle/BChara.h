#ifndef __B_CHARA_H__
#define __B_CHARA_H__

/*
*	< 改造的Chara类，进行功能统一和模块化，防止设计混乱
*/

#include "utils/Scriptor.h"
#include "packui/ItemCell.h"
#include "battle/BSkill.h"
#include "BTrigger.h"

//static const char* tlevel[] = {"E","D","C","B","A","S"};
static const int siHunPo[] = {2,4,5,6,8,10};
static const int siHuiHun[] = {0,1,2,3,4,6};
static const int siXuDong[] = {1,2,3,4,5,6};
static const int siXuDongP[] = {0,0,1,1,2,2};	// <续动额外加成
static const int siGeDang[] = {0,1,2,3,5,7};

/* Chara */
class Chara : public CCObject, public TriggerCa
{
public:
	CCObject* mEcOwner;			// <EChesses | 持有者 | 目前在 BattleField::setChess中进行赋值初始化

	int m_iCharaID;
	string m_sName, m_sPsz,m_sSpx;

	int m_iElement;
	map<int,int> m_miiEquips;		//for equipments
	/* < TO || 装备损耗 */
	vector<int> m_viiESum;
	vector<int> m_viiELock;

	map<string,int> m_iiAttrs;		//for attributes
	//map<int,int> m_viSkills;
	BSkillList* mSkillList;
#define m_viSkills mSkillList->m_viSkills

	bool m_bIsDead;
	//////////////////////////////////////////////////////////////////////////
	// /* <技能接续 */
	int cur_sid;	//Current skill id.
	bool cur_shit;	//If the skill hits?
	
public:
	map<string,int> mt_stDiffer;	/* <更换技能预判用 */

	/* <存储声音播放配置 */
	CCDictionary* m_ssLib;
	float m_sDelay;

public:
	Chara();
	~Chara();

	/* Set&Get */
	int getvalue(string name);
	void setvalue(string name, int val);
	void addvalue(string name, int val);
	void plusvalue(string name, int val);	// <不做任何检测，替代原有的Addvalue
	void addBatValue(string name, int val);	// <针对b_数值做额外的优化

	int gethp();
	void sethp(int value);
	int getmp();
	void setmp(int value);

	/* Equip Change */
	void aplyDiffer();
	void calDiffer( string item_effect_1, string item_effect_2 );

	/* BattleIn */
	//void RefreshFix();	/* <用于刷新修正指数，在BF判定前调用这个接口 */
	int getLead();		/* <获得统帅的值 */
	int getFixRate(int tiType);	/* <读取概率加成 */
	int getFixValue(string name);	/* <获得指定的修正加成后的值 ||如果不是需要修正的参数请不要调用这个接口！ */

	/* Refresh || Lead */
	void ClearLead();	/* <清理，在BF开始时执行 */
	void AddLead(int val);
	void RemoveLead(int val);
	void RefreshLead();

	/* BatlleIn */
	int GetRePrioer();	/* <获得反击的优先权 */
	void PlaySS(const char* asName, int aiPrio = 0);		/* <播放特定的声音脚本 */

	/* Battle Exchange */
	int getSHP();		/* <魂魄最大值 */
	void initValues();	/* <初始化 */
	void roundUp();		/* <滚动 */
	void battleOver();	/* <战斗结束？ */

	/* Battle Ai */
	int getMoveDis();	/* <获得移动范围 */
private:
	/* <意志，请注意Refresh函数读取的是第几个值 || 命名和表意有什么联系吗？ */
	//int miYizi;	

	/* <统帅加成 */
	map<int,int> mLRecord;
	int miLeadAdd;

	/* Battle Exchange */
	int miRu;

public:
	/* <经验值 */
	int miEXP;		// <经验值累计
	int getZDL();	// <获得战斗力数值

	//int miActs;		// <行动次数

private:
	int miRateFix;						// <累计的概率buffer加成 /100
	map<string,int> m_iiAdditions;		// <累计的数值buffer加成

	CCDictionary* mcdBuffers;					// <Buffer列表
	virtual void ClearTriggers();				// < 删除过时的buffer
	std::set<int> mbDels;						// <待删除的Buffer的ID

public:
	void LoadBuffer(int aid, int cout = 0, CCObject* acTar = NULL);					// <加载Buffer
	void RemoveBuffer(int aid);					// <移除指定的buffer

	void RemoveMAttrs(map<string,int> &attrs);	// <移除
	void AddonMAttrs(map<string,int> &attrs);	// <添加
	void AddFiRate(int ai);						// <改变概率加成，由于是int，所以移除直接传入负数
	void initSkill();							// <技能相关初始化

	void checkVals();							// <检查数值是否有超过最大值的情况 ： 魂魄和HP
	int BufferState(int abid);					// <获得指定buffer的状态 -1 为 无 | 0-n 为状态值
	void implyVal(string name, int val);		// <修改数值不做检测

	int miRange;								// <攻击范围 | 一些预判函数会使用到 | 注意，实际技能播放时是无视这个值得 

	bool checkXuDong();							// <检查是否已经不能续动
	bool CanAct();								// <检查是否可以动

	//////////////////////////////////////////////////////////////////////////
	// <音效相关
	void	PlayXuDong();							// <播放续动音效

	void	PlayHpStat();							// <播放HP状态音效 |4
	void	PlayToKill();							// <播放终结音效   |3
	void	PlayReAtk();							// <播放反击音效   |2
	void	PlayAttack();							// <播放攻击音效   |1

	void	PlaySESkill(int asid);					// <播放技能音效   |3

	void	PlayReFilt(const char* asname);			// <反击音效的分歧点 | 反击BOSS用

	void	setHit(int ai);							// <判定单位设定
	bool	mbToKill;								// <预判可杀死
	int		miToDamage;								// <造成的伤害|输出的
	int		miBeDamage;								// <被造成的伤害
	string	msLastVoice;							// <上一个声音

	float	mfLastHpRate;							// <上一次判定的HP百分比
	float	mfLastHpJudge;							// <上一次的触发上界
	string	msHSMask;								// <HP状态的掩码 | 否决用
	int		miSSCount;								// <记录SS被覆盖的次数
	int		miHSMask;								// <标记脚本状态的掩码值

	int		miRateMax;								// <最大的概率值|100

	CCObject* mcTar;								// <上次伤害计算的目标单位
	std::map<string, int>	  miVodAttrs;			// <临时数据装填|音效相关|单次行动
	void	RefreshVodAttrs(const char* ast);						// <刷新值

	void	PlaySSScript(Script* ascp);				// <播放脚本设定
	void	PlayVodActor();							// <最终播放的调用接口
	Script*	mVodScp;								// <脚本保存
	int		miVodPrior, miTmpVodPrior;								// <当前脚本的优先级

	int miSHitBClass;							// <按boss阶级覆盖 | 负数为死亡

	void	TurnOver();						// <一次行动完成
private:
	bool checkSScp(Script* ascp);				// <声音脚本的扩展检测

public:
	void initWithSsm(map<string,string> &assm);	// <使用数据库进行初始化
	string msBHead;		// <战斗头像

	void LoadHalo(Script* asp);					// <添加光环
	void CheckHalo(CCObject* aec, CCPoint acpmo, CCPoint acpaf);				// <检查和刷新光环状态
	void RemoHalo();							// <移除光环对其他单位的影响 | 并刷新 | 调用时pos已经切换

	CCArray*	mcaFHalos;						// <被登记的光环
	void RemoveRefHalos();						// <释放所有登记

private:
	CCArray* mcaHalos;

public:
	string msAttakAttrs;						// <攻击属性数组
	int		miSeType;							// <播放的声音类型| 由BattleField转移到这里
};

#endif