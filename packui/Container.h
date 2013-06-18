/*
|	UI������
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
	<Button��������
	<State: Normal, Hover, Selected, Disable
	<BYLayer�����Ѿ����ģ����Խ����¼�
	<[ע�⣡����] Container���Ὣ�¼����ݸ��ӿؼ�
	<����״̬��
		<1.ֻ�пɼ���Container������¼�
		<2.��m_isEnabledΪfalseʱ��������һ��on_XXX�����ĸı䲻���ûص�����
		<3.ʹ��on_XXX����ˢ�º�m_isEnabled�ᱻ�Զ�����
	<����BY���ƻ��޲������´����¼����б�Ĺ��ܿ���ͨ��lastto����ʵ��
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

	/* <ʹ��һ���ض���ͼƬ���г�ʼ�����ַ��� */
	void initwithPsz(const char* s_MenuItem,const char* s_Title,float width,float height, CCObject* target, SEL_MenuHandler selector);
	/* <�ڲ�����initwithpre,C9Sprite,��ʼ���ַ��� */
	void initString(const char* s_Title,float width,float height,string c9img, CCObject* target, SEL_MenuHandler selector);
	/* <ʹ��ǰ׺���г�ʼ����C9Sprite������ʼ���ַ�����ע���ÿ� */
	void initwithPre(string, float, float);
	/* <ʹ���ⲿ��Դ��ʼ��������ʼ���ַ��� */
	void initwithNode(const char* s_Title,CCNode* aNormal,CCNode* aHover,CCNode* aSelect,CCNode* aDisable,float width,float height);
	void setactivator(CCObject* target, SEL_MenuHandler selector);

	virtual void onHover();
	virtual void onSelect();
	virtual void onNormal();

	void onDisable();
	void SpriteRefresh();

	/* <�ı�Conatiner��״̬��m_bIsEnabled�����л���normal��disable */
	void setEnability(bool tar);
	/* <�ı�m_bIsEnabled��ֵ�����ı�Container��״̬ */
	void settouchstate(bool tar);				

	virtual void setOpacity(GLubyte opacity);
	virtual void setstringnull();
	virtual void setstring(string s);	//Change the string.

	virtual void f_setstring(){};			//Set the string with int.

};

#endif