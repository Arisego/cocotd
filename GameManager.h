#ifndef __GAME_MANAGER_H__
#define __GAME_MANAGER_H__

#include "utils/Scriptor.h"
#include "GameScene.h"
#include "utils/FileIO.h"
#include "sublayer/ModelLayer.h"



class GameManager
{
public:
	static GameManager *sharedGameManager();
	static GameScene *sharedLogicCenter();
	static void purgeSharedGameManager();
	static void purgeSharedLogicCenter();
	static bool mSceneChanged;

	void fullPathFromRelativePath(const char *pszRelativePath,string &psz);			// <提供一个线程安全版本的获取路径函数。
	
	static ModelLayer *mdl;
	CCScene* cs;

	
	bool init();

	enum SceneId
	{
		SCENE_MENU,
		SCENE_PLAY,
		SCENE_ZB,
		SCENE_GAMEOVER
	};

	void runSceneWithId(SceneId id);
	void settransition();

	SceneId state;
	string NextPara;
	
	void preQuit();
	void noQuit();

	bool m_bInfo;
	bool m_bQuit;
	void showinfo();
	void dismissinifo();

	void preConfig(int type, int flag, int tab);
	void goConfig();
	void noConfig();

	void prePareSave();
	void finisSave();


protected:
	GameManager();
	~GameManager();

private:
	static GameManager *mSharedGameManager;
	static GameScene *mCurrentStage;
	bool mbActivate;

public:
	bool isActivate();
	void Disable();
	void EnAblie();

	bool fCanDirectQuit(); /* <是否可以直接退出 */

};


#endif

