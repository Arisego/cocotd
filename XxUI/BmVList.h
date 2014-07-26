#ifndef __BM_VLIST_H__
#define __BM_VLIST_H__

/* <在战斗模式显示的属性表 */

#include "byeven/BYCocos.h"
#include "battle/BChara.h"
#include "packui/TOEquips.h"

class BmVList
	: public BYLayerDescendant
{
protected:
	/* <属性表的基格 */
	CCSprite* mSp_av;	
	/* <属性值字典 string|laberbm */
	CCDictionary* m_cdBmNum;
	/* <装备按钮 */
	vector<TOEquips*> m_vEBtns;	

	/* <角色数据来源 */
	Chara* g_chara;
	//////////////////////////////////////////////////////////////////////////
	// page id 1
	/* name */
	CCLabelTTF* clt;
	/* Button area */
	BYLayerDescendant* eq_mb;
	/* Equip List<Db */
	CCDictionary* m_cdEquips;
	bool m_bPopup;
public:
	~BmVList();
	virtual bool init();
	void	setChara(Chara* ac);

protected:
	void	ShowContents();
	void	InitBmNums();
};

#endif