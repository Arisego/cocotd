#include "MainLayer.h"
#include "SoundManager.h"
#include "MenuScene.h"

#define TAG_WYS 6001
#define MV_TLEN 3

void MainLayer::Close()
{
	setVisible(false);
	SoundManager::sharedSoundManager()->StopMusic();
}

void MainLayer::Pause()
{
	setTouchEnabled(false);
}

void MainLayer::Resume()
{
	setVisible(true);
}

MainLayer::MainLayer()
{
}

MainLayer::~MainLayer()
{
	CCLog(">[ManiLayer] Deco~");
}

//////////////////////////////////////////////////////////////////////////
bool MainLayer::init()
{
	bool bRet = false;
	do 
	{
		CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
		CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
		CC_BREAK_IF(! CCLayer::init());

		msBg = CCSprite::create("Images/UI/bg.jpg");
		msBg->setAnchorPoint(ccp(0,1));
		msBg->setPosition(ccp(0,600));
		addChild(msBg,-10);

		msWys = CCSprite::create("Images/UI/wys.png");
		msWys->setAnchorPoint(ccp(0,0));
		msWys->setPosition(ccp(0,0));
		msWys->setOpacity(48);
		msWys->setTag(TAG_WYS);
		addChild(msWys,0);

		msLogo = CCSprite::create("Images/UI/logo.png");
		msLogo->setAnchorPoint(CCPointZero);
		msLogo->setPosition(ccp(44,0));
		addChild(msLogo,8);

		msLogo2 = CCSprite::create("Images/UI/logo.png");
		msLogo2->setAnchorPoint(CCPointZero);
		msLogo2->setPosition(ccp(44,0));
		msLogo2->setOpacity(188);
		addChild(msLogo2,9);

		msBg->runAction(CCMoveBy::create(MV_TLEN,ccp(0,251)));
		msWys->runAction(CCSequence::create(CCDelayTime::create(MV_TLEN/2), CCFadeTo::create(MV_TLEN*0.7,255), 
			CCCallFunc::create(this, callfunc_selector(MainLayer::ELoadFinal))
			, NULL));

		GLchar * fragSource = (GLchar*) CCString::createWithContentsOfFile(CCFileUtils::sharedFileUtils()->fullPathForFilename("Shaders/logo.fsh").c_str())->getCString();
		mglProgLogo = new CCGLProgram();
		mglProgLogo->initWithVertexShaderByteArray(ccPositionTexture_vert, fragSource);
		mglProgLogo->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
		mglProgLogo->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
		mglProgLogo->link();
		mglProgLogo->updateUniforms();
		mglProgLogo->autorelease();
		msLogo->setShaderProgram(mglProgLogo);

		//////////////////////////////////////////////////////////////////////////
		/* <²¥·ÅÒôÀÖ */
		PlayOpMusic();

		CCDirector* pDirector = CCDirector::sharedDirector();
		pDirector->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority+1, true);
		setTouchEnabled(true);
		scheduleUpdate();
		mbLoading = true;
		bRet = true;
	} while (false);
	return bRet;

}


void MainLayer::ELoadFinal()
{
	stopAllActions();
	msBg->stopAllActions();
	msBg->setPosition(ccp(0,851));

	mbLoading = false;
	removeChildByTag(TAG_WYS);
	msWys = CCSprite::create("Images/UI/wys2.png");
	msWys->setAnchorPoint(ccp(0,0));
	msWys->setPosition(ccp(0,0));
	msWys->setTag(TAG_WYS);
	addChild(msWys,0);

	((MenuScene*) getParent())->SplashOver();
}

void MainLayer::PlayOpMusic()
{
	SoundManager::sharedSoundManager()->PlayMusic("sound/bgm/op.ogg");
}

void MainLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	if(mbLoading) ELoadFinal();
}

