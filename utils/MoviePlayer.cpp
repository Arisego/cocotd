#include "MoviePlayer.h"
#include "CCDirector.h"
#include "GameManager.h"
#include "packui/ConfigManager.h"

NS_CC_BEGIN

MoviePlayer * MoviePlayer::_instance = 0;

static char * videobuf = 0;
bool bPlaying = false;


static void *lock(void *data, void **p_pixels)
{
    *p_pixels = videobuf;
    return NULL;
}

static void unlock(void *data, void *id, void *const *p_pixels)
{
    assert(id == NULL);
}

static void display(void *data, void *id)
{
    (void) data;
    assert(id == NULL);
}

MoviePlayer::MoviePlayer():
vlc(0), vlc_player(0)
{
    init();
}

MoviePlayer::~MoviePlayer()
{
    CCSprite::~CCSprite();
    free(videobuf);

    libvlc_media_player_stop(vlc_player);
    libvlc_media_player_release(vlc_player);
    libvlc_release(vlc);
}

bool MoviePlayer::init(void)
{
	ConfigManager::sharedConfigManager()->GetConfigV(CM_VIDIEOVOLEM,iVoluem);
    vlc = libvlc_new(0, NULL);
    vlc_player = libvlc_media_player_new(vlc);
	bStoped = false;

    CCSize size = CCDirector::sharedDirector()->getWinSize();
    width = size.width;
    height = size.height;
    videobuf = (char *)malloc((width * height) << 2);
    memset(videobuf, 0, (width * height) << 2);
    libvlc_video_set_callbacks(vlc_player, lock, unlock, display, NULL);
    libvlc_video_set_format(vlc_player, "RGBA", width, height, width << 2);
    
    CCTexture2D *texture = new CCTexture2D();
    texture->initWithData(videobuf, kCCTexture2DPixelFormat_RGBA8888, width, height, size);
    return initWithTexture(texture);
}

void MoviePlayer::play(const char *path)
{	
	if(bStoped) return;
	if(libvlc_Playing == libvlc_media_player_get_state(vlc_player)) {
		resume();
		return;
	}
	if(libvlc_Stopped == libvlc_media_player_get_state(vlc_player)){
		bStoped = true;
		return;
	}
    libvlc_media_t *media = libvlc_media_new_path(vlc, path);
	setvolume(iVoluem);
    libvlc_media_player_set_media(vlc_player, media);
    libvlc_media_release(media);
    libvlc_media_player_play(vlc_player);
	scheduleUpdate();
}

void MoviePlayer::stop(void)
{
    libvlc_media_player_stop(vlc_player);
	libvlc_release(vlc);
	bStoped = true;
}

void MoviePlayer::pause(void)
{
    libvlc_media_player_pause(vlc_player);
}

void MoviePlayer::resume(void)
{
	libvlc_media_player_play(vlc_player);
}

void MoviePlayer::draw(void)
{
    CC_PROFILER_START_CATEGORY(kCCProfilerCategorySprite, "CCSprite - draw");

    CCAssert(!m_pobBatchNode, "If CCSprite is being rendered by CCSpriteBatchNode, CCSprite#draw SHOULD NOT be called");

    CC_NODE_DRAW_SETUP();

    ccGLBlendFunc( m_sBlendFunc.src, m_sBlendFunc.dst );

    if (m_pobTexture != NULL)
    {
        ccGLBindTexture2D( m_pobTexture->getName() );
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,(uint8_t *) videobuf);
    }
    else
    {
        ccGLBindTexture2D(0);
    }

    //
    // Attributes
    //

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_PosColorTex );

#define kQuadSize sizeof(m_sQuad.bl)
    long offset = (long)&m_sQuad;

    // vertex
    int diff = offsetof( ccV3F_C4B_T2F, vertices);
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));

    // texCoods
    diff = offsetof( ccV3F_C4B_T2F, texCoords);
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

    // color
    diff = offsetof( ccV3F_C4B_T2F, colors);
    glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    CHECK_GL_ERROR_DEBUG();

    CC_INCREMENT_GL_DRAWS(1);

    CC_PROFILER_STOP_CATEGORY(kCCProfilerCategorySprite, "CCSprite - draw");
}

MoviePlayer * MoviePlayer::instance()
{
    if(_instance == 0)
        _instance = new MoviePlayer();
    return _instance;
}

void MoviePlayer::update(float fDelta){
	bStoped = (libvlc_Ended == libvlc_media_player_get_state(vlc_player));
	if(bStoped){
		unscheduleUpdate();
		setVisible(false);
		GameManager::sharedLogicCenter()->resumestate();
	}
}

void MoviePlayer::setvolume(int sv){
	iVoluem = sv;
	libvlc_audio_set_volume(vlc_player,sv);
}

NS_CC_END