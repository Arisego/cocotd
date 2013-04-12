#include "utils/mSpotLight.h"

mSpotLight::mSpotLight()  
{  
	m_renderTexture = NULL;  
	m_spotLightRadius = 0;  
	m_renderColor.r = 0;  
	m_renderColor.g = 0;  
	m_renderColor.b = 0;  
	m_renderColor.a = 0;  
}  


mSpotLight::~mSpotLight()  
{  
	CC_SAFE_RELEASE(m_renderTexture);  
}    

mSpotLight* mSpotLight::spotLightWithRenderTexture(CCRenderTexture* texture, float radius, ccColor4F color)  
{  
	mSpotLight* spotLight = NULL;  

	spotLight = new mSpotLight();  
	if (spotLight && spotLight->initWithRenderTexture(texture, radius, color) && spotLight->initWithFile("Images/circle.png"))  
	{  
		spotLight->autorelease();  
		return spotLight;  
	}  

	CC_SAFE_DELETE(spotLight);  
	return spotLight;  
} 

bool mSpotLight::initWithRenderTexture(CCRenderTexture* texture, float radius, ccColor4F color)  
{  
	bool bRet = false;  

	do  
	{  
		bRet = CCSprite::init();  
		CC_BREAK_IF(!bRet);  

		sls.clear();
		setRenderTexture(texture);  
		setSpotLightRadius(radius);  
		setRenderColor(color);  
		new CCPoint[1];
		bRet = true;  
	}while(0);  

	return bRet;  
}  


void mSpotLight::setRenderRect(float nx, float ny){
	x = nx;
	y = ny;
}

void mSpotLight::draw()
{
	m_renderTexture->clear(m_renderColor.r, m_renderColor.g, m_renderColor.b, m_renderColor.a);  
	m_renderTexture->begin();  

	ccGLEnableVertexAttribs( kCCVertexAttribFlag_PosColorTex );


	glBindTexture(GL_TEXTURE_2D, (GLuint)m_renderTexture);  
	glBlendFunc(GL_ZERO, GL_SRC_ALPHA);  
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);  

	glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA); 
	m_pobTexture->drawInRect(CCRect(x-m_spotLightRadius, y-m_spotLightRadius, 2 * m_spotLightRadius, 2 * m_spotLightRadius));

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);  
	glBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);   
	m_renderTexture->end();  

	ccGLBlendFunc( m_sBlendFunc.src, m_sBlendFunc.dst );
}