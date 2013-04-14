#ifndef __SPOT_LIGHT_H__  
#define __SPOT_LIGHT_H__  
  
#include "cocos2d.h"

using namespace cocos2d;
using namespace std;

typedef struct _SpotInfo : public CCObject
{
	int colormask,blendmask;
} SpotInfo;


class mSpotLight: public CCSprite  
{  
protected:
	float m_spotLightRadius;
	ccColor4F m_renderColor;
	CCRenderTexture* m_renderTexture; 

public: 
	mSpotLight();  
	~mSpotLight();  

	virtual CCRenderTexture* getRenderTexture(void) const { 
		return m_renderTexture; 
	}

	virtual void setRenderTexture(CCRenderTexture* var)   
	{ 
		if (m_renderTexture != var) 
		{ 
			CC_SAFE_RETAIN(var); 
			CC_SAFE_RELEASE(m_renderTexture); 
			m_renderTexture = var; 
		} 
	}  

	virtual float getSpotLightRadius(void) const {
		return m_spotLightRadius; 
	}

	virtual void setSpotLightRadius(float var){ 
		m_spotLightRadius = var;
	}

	virtual ccColor4F getRenderColor(void) const { 
		return m_renderColor; 
	}

	virtual void setRenderColor(const ccColor4F& var){ 
		m_renderColor = var; 
	}

	bool s_bInitialized;

	static mSpotLight* spotLightWithRenderTexture(CCRenderTexture* texture, float radius, ccColor4F color); 
	virtual void setRenderRect(float nx, float ny);
	//virtual void draw();

private:
	bool initWithRenderTexture(CCRenderTexture* texture, float radius, ccColor4F color); 
	float x,y;
	map<string,SpotInfo*> sls;				//添加多个光点备用


};  
  
#endif// __SPOT_LIGHT_H__ 