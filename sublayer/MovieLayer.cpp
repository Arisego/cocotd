#include "MovieLayer.h"

#include "GameManager.h"
#include "GameScene.h"
#include "utils/ALSingle.h"

MoviePlayer* MovieLayer::player = NULL;

void MovieLayer::cleanupplayer(){
	if(!player) return;
	player->stop();
	player->removeFromParent();
	player->release();
	
	player = NULL;
	psz.clear();
	ALSingle::sharedALSingle()->SetMov(NULL);
}

void MovieLayer::Pause(){
	if(!b_StateRunning) return;
	if(player){
		//player->pause();
		b_StateRunning = false;
	}
	setTouchEnabled(false);
}

void MovieLayer::Resume(){
	if(psz.length()<1) return;
	if(!player){
		player = new MoviePlayer();
		player->setAnchorPoint(ccp(0,0));
		addChild(player);
		//player->autorelease();
		ALSingle::sharedALSingle()->SetMov(player);
	}
	setVisible(true);
	play_media();
	setTouchEnabled(true);
	b_StateRunning = true;
}

void MovieLayer::Close(){
	if(!player) return;
	cleanupplayer();
	setVisible(false);
	setTouchEnabled(false);

	b_StateRunning = false;
}

void MovieLayer::byTouchEnded(CCTouch *pTouch, CCEvent *pEvent){
	BYLayerModal::byTouchEnded(pTouch,pEvent);
	GameManager::sharedLogicCenter()->resumestate();
	CCLOG("MovieLayer touched!");
	return;
}

void MovieLayer::set_media(string s){
	psz = s;

}

void MovieLayer::play_media()
{
	player->play(psz.c_str());
	player->bStoped = true;
}

MovieLayer::~MovieLayer()
{
	cleanupplayer();
	ALSingle::purgeSharedALSingle();
}
