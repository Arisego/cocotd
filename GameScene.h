#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

#include "sublayer/TextLayer.h"
#include "sublayer/MapLayer.h"
#include "sublayer/MovieLayer.h"
#include "sublayer/ModelLayer.h"

#include "tinyxml/tinyxml.h"
#include "utils/ScirptDer.h"
//#include "utils/mSpotLight.h"
#include "utils/Controller.h"
#include "utils/States.h"

typedef struct _ImageInfo : public CCObject
{
	CCImage        *image;
	CCImage::EImageFormat imageType;

	~_ImageInfo(){
		image->release();
	}
} ImageInfo;



class GameScene : public cocos2d::CCScene, public StatesManager
{
	
private:
	CCSize visibleSize;
	CCPoint origin;
	bool m_IsTextShown;
	
			
	CCArray* ScriptList;
	int m_IScriptSum;
	ScriptDer* t_sd;
	
	int m_flag;			// Flag for effect lock.
	void FlagRetain();
	void FlagRelease();

	string slatst;
	bool m_bdLoop;
	void DelayPlay();

public:
	ToTextLayer* te;
	MapLayer* ml;
	string m_sBgi;
	bool m_bLoadProtect;	// <对读档时的msp进行保护

	void addBackImg();
    virtual bool init();  
	~GameScene();

  
    // implement the "static node()" method manually
	CREATE_FUNC(GameScene);
	
	void onExit();
	void update(float dt);
	
	
	void f_initover();
	bool f_cachetest(const char* rscMask);		// <移动缓存并记录进map

	void TextChange(float dt);

	int m_StageState;
	CCDictionary* m_pImages;
	
	bool e_TextAuto;

	void handle_key( UINT message,WPARAM wParam, LPARAM lParam );

	bool e_getscript();
	void e_update(int new_state);
	void e_gonext();
	void e_jump(int j);
	void e_act();
	int e_curscript;
	void e_handlecurs(Script* s);
	virtual void PreQuit();

	CCArray* tScriptList;
	int t_curscript;
	int t_ssum;
	void e_tmpscript(CCArray* sl,int sum);			// <处理地图脚本的接口

	CCLayer* bg;
	MovieLayer* tMovie;
	CCSprite* BgImg;

	CCLayerColor* ms_Mask;

	void DerChange(Script* s);

	CCImage* snapshot;
	void PrepareSave();
	void Snap();	/* <屏幕状态截图 */

	mSpotLight* spotLight;

	string scpfile;

};

#endif  // __GAMEPLAY_LAYER_H__