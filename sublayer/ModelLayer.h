/*
|	∂•∂Àƒ£Ã¨œ‘ æ
|
|
*/
#ifndef __MODEL_LAYER_H__
#define __MODEL_LAYER_H__

#include "cocos2d.h"

#include "utils/States.h"
#include "byeven/BYCocos.h"

#include "packui/Tab.h"

USING_NS_CC;
using namespace std;

class ModelLayer : public BYLayerAncestor, public StateMachine
{
private:
	string sContent;
	int miType, miFlag, miTab;
	Tab* mtTab;
	float m_fHeight,m_fWidth;
	std::vector<Container*> m_vBtns;
	BYLayerDescendant *mb;
	bool m_bLockSave;
	

public:
	static bool m_qlock;
	static bool m_clock;
	
	void Pause();
	void Close();
	void Resume();

	void preQuit();
	void c_quit_yes(CCObject* sender);
	void c_quit_no(CCObject* sender);

	void preConfig(int type, int flag, int tab = 0);
	void noConfig(float dt);
	void Show_Content();
	void Refresh_Button();
	void Add_Button(const char* name,int tag);
	void buttonback(CCObject* sender);

	ModelLayer();
	~ModelLayer();
	
};

#endif	// __MODEL_LAYER_H__