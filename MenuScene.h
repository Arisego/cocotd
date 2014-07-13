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
private:
	CCSprite* msLogo;
	CCSprite* msLogo2;

	CCSprite* msBg;
	CCSprite* msMenu;
	CCSprite* msWys;	// <Tag:WYS

	CCGLProgram *mglProgLogo;

public:
	void	ELoadFinal();	// <清除和载入真实图片

public:
	~MenuScene();
    virtual bool init();  

	void update(float dt);
  
	virtual void PreQuit() override;
	CREATE_FUNC(MenuScene);

};

#endif  // __GAMEPLAY_LAYER_H__