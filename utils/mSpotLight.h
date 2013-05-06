#ifndef __SPOT_LIGHT_H__  
#define __SPOT_LIGHT_H__  
  
#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;
using namespace cocos2d;
using namespace std;

typedef struct _SpotInfo : public CCObject
{
	int colormask,blendmask;
} SpotInfo;


class mSpotLight
{  
protected:
	int m_iLaDep;

	float m_spotLightRadius;
	ccColor4F m_renderColor;

	CCClippingNode* cp_board;
	CCSpriteBatchNode* cns_blocks;

public: 
	mSpotLight();  
	void f_clear_circles();  

	bool m_bAvalible;					//This does not control the on/off, it just represent that cp_board's visibility.
	void f_state_circle(bool st);

	virtual float getSpotLightRadius(void) const {
		return m_spotLightRadius; 
	}

	virtual void setSpotLightRadius(float var){ 
		m_spotLightRadius = var;
	}

	virtual ccColor4F getRenderColor(void) const { 
		return m_renderColor; 
	}

	virtual void setRenderColor(const ccColor4F& var);

	bool s_bInitialized;

	virtual void setRenderRect(float nx, float ny);
	//virtual void draw();
	virtual bool f_init(CCNode* cn_borad, int i_ld = 0);
	bool f_refresh_circles();
private:
	CCSprite* m_StencilCircle;				//Always Generate once.
	CCSprite* m_AkaruCircle;
	CCSpriteBatchNode* csb_st;

	CCNode* m_Board;

	float x,y;
	map<string,SpotInfo*> sls;				//添加多个光点备用

	void generate_circle();					//Avalible only after f_init(). And empty if not using render texture.
	void nouse();

};  
  
#endif// __SPOT_LIGHT_H__ 