#ifndef __SPOT_LIGHT_H__  
#define __SPOT_LIGHT_H__  
  
#include "cocos2d.h"

using namespace cocos2d;
using namespace std;

typedef struct _SpotInfo : public CCObject
{
	int colormask,blendmask;
} SpotInfo;


class CCSpotLight: public CCSprite  
{  
public:  
    CCSpotLight();  
    ~CCSpotLight();  
  
	map<string,SpotInfo*> sls;				//添加多个光点备用
	float x,y;

    static CCSpotLight* spotLightWithRenderTexture(CCRenderTexture* texture, float radius, ccColor4F color);  
  
	void setRenderRect(float nx, float ny);
  
    CC_SYNTHESIZE_RETAIN(CCRenderTexture*, m_renderTexture, RenderTexture)  
	CC_SYNTHESIZE(float, m_spotLightRadius, SpotLightRadius)
    CC_SYNTHESIZE_PASS_BY_REF(ccColor4F, m_renderColor, RenderColor)  

	bool s_bInitialized;
private:  
    bool initWithRenderTexture(CCRenderTexture* texture, float radius, ccColor4F color);  
    void draw();  
	//void update(float dt);


};  
  
#endif// __SPOT_LIGHT_H__ 