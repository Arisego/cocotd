#include <cocos2d.h>  
using namespace cocos2d;  
  
#include "SpotLight.h"  
  
const int SPOT_LIGHT_VERTICES_COUNT = 45;// 必须大于5, 圆心和圆周上的顶点数  
const ccColor4B SPOT_LIGHT_CENTER_COLOR = ccc4(0, 0, 0, 0);  
const ccColor4B SPOT_LIGHT_EDGE_COLOR = ccc4(0xFF, 0xFF, 0xFF, 0xFF);  
const ccColor4F SPOT_CENTER = ccc4f(0,0,0,0);
  
  
CCSpotLight::CCSpotLight()  
{  
    m_renderTexture = NULL;  
    m_spotLightRadius = 0;  
    m_renderColor.r = 0;  
    m_renderColor.g = 0;  
    m_renderColor.b = 0;  
    m_renderColor.a = 0;  
}  
  
  
CCSpotLight::~CCSpotLight()  
{  
    CC_SAFE_RELEASE(m_renderTexture);  
}    
  
CCSpotLight* CCSpotLight::spotLightWithRenderTexture(CCRenderTexture* texture, float radius, ccColor4F color)  
{  
    CCSpotLight* spotLight = NULL;  
  
    spotLight = new CCSpotLight();  
    if (spotLight && spotLight->initWithRenderTexture(texture, radius, color) && spotLight->initWithFile("Images/circle.png"))  
    {  
        spotLight->autorelease();  
        return spotLight;  
    }  
  
    CC_SAFE_DELETE(spotLight);  
    return spotLight;  
}  
  
  
bool CCSpotLight::initWithRenderTexture(CCRenderTexture* texture, float radius, ccColor4F color)  
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
  
        bRet = true;  
    }while(0);  
  
    return bRet;  
}  
  
  
void CCSpotLight::setRenderRect(float nx, float ny){
	x = nx;
	y = ny;
}

void CCSpotLight::draw()  
{  
    //CCSprite::draw();  
  
    int segs = SPOT_LIGHT_VERTICES_COUNT;  
    GLfloat *vertices = new GLfloat[2*segs];//malloc( sizeof(GLfloat)*2*(segs));  
    GLfloat *coordinates = new GLfloat[2*segs];////malloc( sizeof(GLfloat)*2*(segs));  
    ccColor4B *colors = new ccColor4B[segs];//malloc( sizeof(ccColor4B)*(segs));  
	CCPoint *cps = new CCPoint[segs];
      
    memset(vertices,0, sizeof(GLfloat)*2*(segs));  
    memset(coordinates,0, sizeof(GLfloat)*2*(segs));  
      
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();  

  
    m_renderTexture->clear(m_renderColor.r, m_renderColor.g, m_renderColor.b, m_renderColor.a);  
    //m_renderTexture->clear(0, 0, 0, 0xFF);  
      
/*    colors[0] = SPOT_LIGHT_CENTER_COLOR;  
    for (int i = 1; i < segs; i++)  
    {  
        colors[i] = SPOT_LIGHT_EDGE_COLOR;  
    } */ 
      
 //   const float coef = 2.0f * (float)M_PI/(segs-2) ;  
 //   CCPoint pos = getPosition();  
 //   CCSize size = this->getContentSize();  
 // 
 //   vertices[0] = winSize.width/2;//pos.x;  
 //   vertices[1] = winSize.height/2;//pos.y;  
	//cps[0] = ccp(winSize.width/2,winSize.height/2);
 //   coordinates[0] = vertices[0]/winSize.width;  
 //   coordinates[1] = (size.height - vertices[1])/winSize.height;  
 //   for(int i=1;i<segs;i++)  
 //   {  
 //       float rads = i*coef;  
 //       float j = m_spotLightRadius * cosf(rads);// + pos.x;  
 //       float k = m_spotLightRadius * sinf(rads);// + pos.y;

	//	j+= winSize.width/2;
	//	k+= winSize.height/2;
 //         
	//	cps[i] = ccp(j,k);

 //       vertices[i*2] = j;  
 //       vertices[i*2+1] = k;  

	//	
 //         
 //       coordinates[i*2] = (j)/winSize.width;  
 //       coordinates[i*2+1] = (size.height-k)/winSize.height;  
 //   }  
 //     
    // Update the render texture  
    //[self.renderTexture begin];  
    m_renderTexture->begin();  
      
	ccGLEnableVertexAttribs( kCCVertexAttribFlag_PosColorTex );


    glBindTexture(GL_TEXTURE_2D, (GLuint)m_renderTexture);  
    glBlendFunc(GL_ZERO, GL_SRC_ALPHA);  
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);  
      
/*    glVertexPointer(2, GL_FLOAT, 0, vertices);  
    glTexCoordPointer(2, GL_FLOAT, 0, coordinates);  
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);  

    glDrawArrays(GL_TRIANGLE_FAN, 0, segs);*/  

	//ccDrawColor4B(SPOT_LIGHT_CENTER_COLOR.r,SPOT_LIGHT_CENTER_COLOR.g,
	//	SPOT_LIGHT_CENTER_COLOR.b,SPOT_LIGHT_CENTER_COLOR.a);
	//ccDrawSolidPoly(cps,segs,SPOT_CENTER);
	
	 glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA); 


	 //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); 
	 //glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);		//在底色上绘制出原本的光

	 
	 //	 CCSprite::draw();
	//#define kQuadSize sizeof(m_sQuad.bl)
	//if (m_pobTexture)
	//{
	//	glBindTexture(GL_TEXTURE_2D, m_pobTexture->getName());
	//}
	//else
	//{
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//}
	//long offset = (long)&m_sQuad;
	
	
	//// vertex
	//int diff = offsetof(ccV3F_C4B_T2F, vertices);
	//glVertexPointer(3, GL_FLOAT, kQuadSize, (void*)(offset + diff));
	//// color
	//diff = offsetof( ccV3F_C4B_T2F, colors);
	//glColorPointer(4, GL_UNSIGNED_BYTE, kQuadSize, (void*)(offset + diff));
	//// tex coords
	//diff = offsetof( ccV3F_C4B_T2F, texCoords);
	//glTexCoordPointer(2, GL_FLOAT, kQuadSize, (void*)(offset + diff));

	//int diff = offsetof( ccV3F_C4B_T2F, vertices);
	//glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));

	//// texCoods
	//diff = offsetof( ccV3F_C4B_T2F, texCoords);
	//glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

	//// color
	//diff = offsetof( ccV3F_C4B_T2F, colors);
	//glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));


	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//m_pobTexture->drawAtPoint(ccp(200,200));
	m_pobTexture->drawInRect(CCRect(x-m_spotLightRadius, y-m_spotLightRadius, 2 * m_spotLightRadius, 2 * m_spotLightRadius));
     
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);  
    glBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);  

    //[self.renderTexture end];  
    m_renderTexture->end();  

	ccGLBlendFunc( m_sBlendFunc.src, m_sBlendFunc.dst );
      
    CC_SAFE_DELETE(vertices);  
    CC_SAFE_DELETE(coordinates);  
    CC_SAFE_DELETE(colors);  
}  