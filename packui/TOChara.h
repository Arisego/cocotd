#ifndef __TO_CHARA__
#define __TO_CHARA__

#include "cocos2d.h"
#include "packui/CharaS.h"
#include "packui/TOEquips.h"
#include "utils/Controller.h"
#include "packui/TOPopup.h"

USING_NS_CC;

/*
	TOChara
	<TO_风格的角色信息界面
	<操作键盘接口 ijkl zx wsad

*/

class TOChara : public CCLayer,public CharaTab,public Activator
{
public:
	TOChara(int a_iCharaID,CCObject* target, SEL_MenuHandler selector);
	~TOChara();

	/* <弹出窗口 */
	TOPopup* m_Topop;
	/* <装备被点击,鼠标操作接口 */
	void EquipClick(CCObject* pSender);

	void EquipChange(CCObject* pSender);
	/* Change Equip view */
	bool RefreshEquip(int anewi);
	/* Pop up */
	void EquipPop();

protected:
	/* <属性值字典 string|laberbm */
	CCDictionary* m_cdBmNum;
	/* <装备按钮 */
	vector<TOEquips*> m_vEBtns;	
	/* <属性和装备 */
	CCSprite* mSp_av;
	/* <技能 */
	CCSprite* mSp_sa;			

	/* <刷新显示 */
	void RefreshView(int Page);
	/* <刷新数据 */
	void RefreshData(int Id);
	void f_refresh_cur_data();


	/* <显示属性和装备页 pageid:1 */
	void ShowAV();
	/* <装备列表 */
	void ShowEquips();
	/* <初始化数值显示 pageid:1 */
	void InitBmNums();


	/* <显示技能页 */
	void ShowSa();

	void w_press();
	void s_press();
	void z_press();
	void x_press();

private:
	/* <角色序号 */
	int m_iCharaID;
	/* <页标志 1 为属性 2 为技能 */
	int m_iPage;
	/* <角色数，来自CharaS */
	int m_CharaNums;
	int m_iCurrentEuip;

	/* <角色数据来源 */
	Chara* g_chara;
	//////////////////////////////////////////////////////////////////////////
	// page id 1
	/* name */
	CCLabelTTF* clt;
	/* stand */
	CCSprite*	mSpStand;
	/* Button area */
	BYLayerDescendant* eq_mb;
	/* Equip List<Db */
	CCDictionary* m_cdEquips;
	bool m_bPopup;
};


#endif