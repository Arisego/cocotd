/*
|	用于包装movieplayer
|
|
*/
#ifndef __MOVIE_LAYER_H__
#define __MOVIE_LAYER_H__

#include "cocos2d.h"


#include "utils/MoviePlayer.h"
#include "utils/States.h"
#include "byeven/BYCocos.h"

USING_NS_CC;
using namespace std;


class MovieLayer : public BYLayerModal, public StateMachine
{
public:
	static MoviePlayer* player;

	void Pause();
	void Close();
	void Resume();
	void cleanupplayer();

	virtual void byTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

	string psz;

	void set_media(string s);
	void play_media();
};

#endif	// __MOVIE_LAYER_H__