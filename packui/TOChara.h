#ifndef __TO_CHARA__
#define __TO_CHARA__

#include "cocos2d.h"
#include "packui/CharaS.h"

USING_NS_CC;

/*
	TOChara
	<TO_风格的角色信息界面
	<操作键盘接口 ijkl zx wsad

*/

class TOChara : public CCLayer
{
public:
	TOChara(int a_iCharaID,CCObject* target, SEL_MenuHandler selector);
	~TOChara();

protected:
	CCDictionary* m_cdBmNum;
	/* <属性和装备 */
	CCSprite* mSp_av;
	/* <技能 */
	CCSprite* mSp_sa;			

	/* <刷新显示 */
	void RefreshView(int Page);
	/* <刷新数据 */
	void RefreshData(int Id);
	/* <显示属性和装备页 pageid:1 */
	void ShowAV();
	/* <初始化数值显示 pageid:1 */
	void InitBmNums();

private:
	//////////////////////////////////////////////////////////////////////////
	//	<通用的回调结构

	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;
	void activate(CCObject* pSender);
	//////////////////////////////////////////////////////////////////////////
	/* <角色序号 */
	int m_iCharaID;
	/* <页标志 1 为属性 2 为技能 */
	int m_iPage;
	/* <角色数，来自CharaS */
	int m_CharaNums;

	/* <角色数据来源 */
	Chara* g_chara;
	//////////////////////////////////////////////////////////////////////////
	// page id 1
	/* name */
	CCLabelTTF* clt;
	/* stand */
	CCSprite*	mSpStand;
};


#endif