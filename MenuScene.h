#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include "cocos2d.h"
#include "utils\States.h"
#include "XxTitle\MenuLayer.h"
#include "XxTitle\MainLayer.h"
#include "XxTitle\FLLayer.h"

class MenuScene : public cocos2d::CCScene, public StatesManager
{

public:
	~MenuScene();
    virtual bool init();  

	void update(float dt);
  
	virtual void PreQuit() override;
	CREATE_FUNC(MenuScene);

private:
	TitleMMenu* menu;
	MainLayer*	ml;
	FlLayer*	mFlLayer;
	bool		mbControlLock;

	int	miState;

public:
	void	SplashOver();
	void	ChangeState(int ais);

	void	LockControl();		/* <��������|���Ų������Ч�� */
	void	UnLockControl();	/* <��������|���Ų������Ч�� */

	void	GoToGame();			/* <������Ϸ */
};

#endif  // __GAMEPLAY_LAYER_H__