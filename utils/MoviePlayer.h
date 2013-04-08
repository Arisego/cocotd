#ifndef __MOVIEPLAYER_H__
#define __MOVIEPLAYER_H__

/****************************************************************************
http://www.cnblogs.com/evan-cai/

Author: Evan-Cai
Date: 2013-01-25
****************************************************************************/

#include <vlc\vlc.h>
#include "sprite_nodes\CCSprite.h"

NS_CC_BEGIN

class MoviePlayer : public CCSprite
{
public:
    ~MoviePlayer();

    static MoviePlayer * instance(void);
	bool	bStoped;
	int		iVoluem;

    bool init(void);
    void play(const char *path);
    void stop(void);
    void pause(void);
	void resume(void);
    void draw(void);
	void update(float fDelta);
	void setvolume(int sv);

    MoviePlayer();

private:

    libvlc_instance_t *vlc;
    libvlc_media_player_t *vlc_player;

    unsigned int width;
    unsigned int height;

    static MoviePlayer * _instance; 
};

NS_CC_END

#endif