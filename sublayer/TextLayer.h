#ifndef __TEXT_LAYER_H__
#define __TEXT_LAYER_H__

#include "cocos2d.h"


#include "utils/Scriptor.h"
#include "utils/MouseMenu.h"
#include "utils/States.h"
#include "byeven/BYCocos.h"
#include "sublayer/TextLayerBtn.h"
#include "XxUI/HSBotton.h"

USING_NS_CC;
using namespace std;

#define MENUTAG 3000

class ToTextLayer : public BYLayerModal,public StateMachine,public Cake
{
private:
	void update(float dt);
	float m_timer;
	CCLabelTTF* m_Label;
	CCLabelTTF* m_Name;
	int cur,sum;
	CCSize s;
	float mSingeWidth,mLineCount;
	float m_fTAuto,m_fTText;

	vector<HSButton*> mvBtns;
	void tlbback(CCObject* sender);
	//bool mtblockclick;

public:
	~ToTextLayer();
	ToTextLayer();  
	virtual void Pause();
	virtual void Resume();
	virtual void Close();

	bool m_bSnap;
	void beforesnap();
	void aftersnap();

	float m_height,m_textwidth,m_avatarwidth;
	char* font;
	int fontsize, m_iTextcount;
	string lines;
	string fulline;
	bool m_bIsReadover,m_bIsShownOver;

	virtual void onEnter();

	void ShowText(const char* line, const char* name);
	void FlushText(const char* line,bool dst = false);
	void StreamText(float dt);

	void FormText();
	vector<int> vi_text;
	int m_iLine;	//The num of line.
	float m_fBlockY;
	CCClippingNode* cpn_textcn;
	CCSpriteBatchNode*	cns_blocks;

	CCSprite* m_sBox;
	//MouseMenu* menu;

	void FadeText(CCObject* sender);
	bool m_bIsNoFade;

	int e_layerstate;
	void GetTimePara();

	virtual void right_click() override;
	CREATE_FUNC(ToTextLayer);

	//////////////////////////////////////////////////////////////////////////
	
	MouseMenu* sel_menu;
	std::map<string,int> TagMap;
	std::map<string,string> PathMap;
	void ELoad();


	//meta
	bool DerSelMenu(Script* ts);
	bool DerLoadImg(Script* ts);
	CCAction* DerAction(Script* ts, int indent = 0);
	void DerFinal(Script* ts);
	void DerLock(Script* ts);
	//meta

	void DerSilent(Script* ts);

	static CCDictionary* lockstate; 
	bool m_bHasFinal;
	map<string,int> m_remove;			//store those must be cleaned 
	map<string,int> m_stop;

	void menucallback(CCObject* sender);

	//////////////////////////////////////////////////////////////////////////
	// 2013-7-21
	void GS_Lock();
	void GS_unLock();

	//////////////////////////////////////////////////////////////////////////
	// 2013-7-23
	void StepNext();	//Changing to Public.
	bool mLockNext;	//Lock for change color.

protected:
	bool m_bIsAuto;
	bool m_bIsSkip;
	void StopStream();
	void AutoNext();
	void RegistAA();	//Register Auto Action
	void StartAuto(CCObject* sender);
	void StartSkip(CCObject* sender);



	bool click(CCTouch *touch, CCEvent * pEvent);
	
	virtual bool byTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
		if(isVisible()) return BYLayerModal::byTouchBegan(pTouch,pEvent);
		setVisible(true);
		bool tb = BYLayerModal::byTouchBegan(pTouch,pEvent);
		setVisible(false);
		return tb;
	}

	bool m_bTouchProt;
	virtual void ccTouchEnded(CCTouch *touch, CCEvent * pEvent){
		m_bTouchProt = true;
		if(m_bIsNoFade) BYLayerModal::ccTouchEnded(touch,pEvent);
		if(m_bTouchProt) click(touch,pEvent);
		
	}
	//virtual void ccTouchEnded(CCTouch *touch, CCEvent * pEvent);
	//virtual void ccTouchCancelled(CCTouch *touch, CCEvent * pEvent);
	virtual void ccTouchMoved(CCTouch *touch, CCEvent * pEvent){
		if(m_bIsNoFade) BYLayerModal::ccTouchMoved(touch,pEvent);
	}

	int miSelMenuJp;
public:
	void SelMenuBC();
};

#endif	// __TEXT_LAYER_H__