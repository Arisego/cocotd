#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include "cocos2d.h"
#include "utils\MouseMenu.h"
#include "byeven\BYCocos.h"
#include "utils\States.h"

#include "SoundManager.h"

class MenuLayerMainMenu : public BYLayerAncestor, public StateMachine
{
protected:
    CCMenuItem*    m_disabledItem;
	MouseMenu* menu;

public:
    MenuLayerMainMenu(void);
    ~MenuLayerMainMenu();

public:
    virtual void registerWithTouchDispatcher();


    void allowTouches(float dt);
    void menuCallback(CCObject* pSender);
    void menuCallbackConfig(CCObject* pSender);
    void menuCallbackDisabled(CCObject* pSender);
    void menuCallbackEnable(CCObject* pSender);
    void menuCallback2(CCObject* pSender);
    void menuCallbackPriorityTest(CCObject* pSender);
    void onQuit(CCObject* pSender);

	virtual void Pause();
	virtual void Resume();
	virtual void Close();

    //CREATE_NODE(MenuLayer1);
};


class MenuScene : public cocos2d::CCScene, public StatesManager
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

	void update(float dt){
		//SoundManager::sharedSoundManager()->update(dt);
	}
  
	virtual void PreQuit(){
		StatesManager::PreQuit();
		
	}

    // implement the "static node()" method manually
	CREATE_FUNC(MenuScene);

};

#endif  // __GAMEPLAY_LAYER_H__