#ifndef __B_HALO_H__
#define __B_HALO_H__

#include "cocos2d.h"
#include "utils\Scriptor.h"
USING_NS_CC;

class HaloStat;

class BHalo : public CCObject
{
public:
	BHalo(Script* asp, CCObject* aec);
	~BHalo();

	CCObject*	m_ecOwner;						// <光环的持有者
private:
	set<CCObject*>	m_caEObs;					// <受到光环影响的单位
	int			miState;						// <状态标志 | 0 - 无单位捕捉 | 1 - 捕捉到单位


	bool		init();							// <初始化
	void		RefreshState();					// <更新状态
	void		RefreshOwner();					// <刷新持有者状态

	Script*		mScp;							// <光环脚本
	int			miDist;							// <光环的范围
	map<CCObject*, HaloStat*> mHalosTates;		// <光环状态缓存

	int			miNums;							// <受影响的单位数 | 不包括自身
	std::map<string, int>	  miAttrs;			// <临时数据装填

public:
	void		checkEC(CCObject* aeTar, CCPoint acpmo, CCPoint acpaf);		// <检查单位是否处在光环中
	void		removEC(CCObject* aeTar);		// <移除这个单位的光环特效 | 先移除再添加的模式

protected:
	void		RefreshAttrs(const char* ast);	// <刷新临时值
	void		RefreshNums();					// <个数和State
	
	HaloStat*	mHaloOS;						// <持有者的状态缓存

	int			miType;							// <光环的扩散类型
	int			miOwType;						// <光环的持有者的类型

	bool		mbTarProtect, mbOwnProtect;		// <变更保护
	int			motoNum;						// <缓存

	void		f_clean();						// <全体清除的实体
public:
	void		removeAllMov();					// <因移动清楚所有
	void		removeClean(CCObject* aeTar);	// <因析构而清除
};


#endif