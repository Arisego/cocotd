/*
|	UI容器类
|
|
*/
#ifndef __CONTAINER_H__
#define __CONTAINER_H__

#include "byeven/BYCocos.h"
#include "cocos-ext.h"

#include "utils/SpriteTextMenuItem.h"
USING_NS_CC_EXT;
using namespace std;

/*
	<Button容器基类
	<State: Normal, Hover, Selected, Disable
	<BYLayer机制已经更改，可以接受事件
	<[注意！！！] Container不会将事件传递给子控件
	<关于状态：
		<1.只有可见的Container会接受事件
		<2.当m_isEnabled为false时，不接受一切on_XXX函数的改变不调用回调函数
		<3.使用on_XXX进行刷新后，m_isEnabled会被自动设置
	<由于BY机制会无差别的向下传递事件，列表的功能可以通过lastto功能实现
*/
class Container : public BYLayer
{
private:
	CCTouch* mLasto;

protected:
	CCNode* spriteSelected;
	CCNode* spriteHover;
	CCNode* spriteNormal;
	CCNode* spriteDisable;

	bool m_bIsEnabled;

	enum ContainerState
	{
		C_STATE_NORMAL = 0,
		C_STATE_HOVERD,
		C_STATE_SELECT,
		C_STATE_DISABLE
	};

	int m_iState;
	

	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;
	void activate();

public:
	CCLabelBMFont* labelAtlas;

	Container();
	~Container();

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void onExit();

	virtual void update(float fDelta);
	virtual bool checkTouch(CCTouch *pTouch);

	/* <使用一个特定的图片进行初始化，字符串 */
	void initwithPsz(const char* s_MenuItem,const char* s_Title,float width,float height, CCObject* target, SEL_MenuHandler selector);
	/* <内部调用initwithpre,C9Sprite,初始化字符串 */
	void initString(const char* s_Title,float width,float height,string c9img, CCObject* target, SEL_MenuHandler selector);
	/* <使用前缀进行初始化，C9Sprite，不初始化字符串，注意置空 */
	void initwithPre(string, float, float);
	/* <使用外部资源初始化，不初始化字符串 */
	void initwithNode(const char* s_Title,CCNode* aNormal,CCNode* aHover,CCNode* aSelect,CCNode* aDisable,float width,float height);
	void setactivator(CCObject* target, SEL_MenuHandler selector);

	virtual void onHover();
	virtual void onSelect();
	virtual void onNormal();

	void onDisable();
	void SpriteRefresh();

	/* <改变Conatiner的状态，m_bIsEnabled，并切换至normal或disable */
	void setEnability(bool tar);
	/* <改变m_bIsEnabled的值，不改变Container的状态 */
	void settouchstate(bool tar);				

	virtual void setOpacity(GLubyte opacity);
	virtual void setstringnull();
	virtual void setstring(string s);	//Change the string.

	virtual void f_setstring(){};			//Set the string with int.

};

#endif