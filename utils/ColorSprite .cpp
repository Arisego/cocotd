#include "ColorSprite.h"

static  const GLchar *transparentshader = 
	"\n\
	#ifdef GL_ES                                            \n\
	precision lowp float;                                   \n\
	#endif                                                  \n\
	varying vec4 v_fragmentColor;                           \n\
	varying vec2 v_texCoord;                                \n\
	uniform sampler2D u_texture;                            \n\
	void main()                                             \n\
	{                                                       \n\
	float ratio=0.0;                                    \n\
	vec4 texColor = texture2D(u_texture, v_texCoord);   \n\
	ratio = texColor[0] > texColor[1]?(texColor[0] > texColor[2] ? texColor[0] : texColor[2]) :(texColor[1] > texColor[2]? texColor[1] : texColor[2]);  \n\
	if (ratio != 0.0)                                          \n\
	{                                                         \n\
	texColor[3] = ratio;                                   \n\
	}                                                          \n\
	else                                                       \n\
	{                                                          \n\
	texColor[3] = 0.0;                                     \n\
	}                                                          \n\
	gl_FragColor = v_fragmentColor*texColor;                   \n\
	}";

ColorSprite* ColorSprite::create(const char *pszFileName)  
{

	ColorSprite *pRet = new ColorSprite();  
	if (pRet && pRet->initWithFile(pszFileName))
	{  
		pRet->autorelease();  
		return pRet;  
	}else{  
		delete pRet;  
		pRet = NULL;  
		return NULL;  
	}  
}  

bool ColorSprite::initWithSpriteFrame(CCSpriteFrame *pSpriteFrame)
{
	CCLog(">[ClolorSprite] initWithSpriteFrame()");
	CCAssert(pSpriteFrame != NULL, "");
	bool bRet = initWithTexture(pSpriteFrame->getTexture());
	// <加载顶点着色器和片元着色器  
	m_pShaderProgram = new  CCGLProgram();  
	m_pShaderProgram ->initWithVertexShaderByteArray(ccPositionTextureA8Color_vert, transparentshader);  

	CHECK_GL_ERROR_DEBUG();  

	// <启用顶点着色器的attribute变量，坐标、纹理坐标、颜色  
	m_pShaderProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);  
	m_pShaderProgram->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);  
	m_pShaderProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);  

	CHECK_GL_ERROR_DEBUG();  

	// <自定义着色器链接  
	m_pShaderProgram->link();  

	CHECK_GL_ERROR_DEBUG();  

	// <设置移动、缩放、旋转矩阵  
	m_pShaderProgram->updateUniforms();  

	CHECK_GL_ERROR_DEBUG();  

	setDisplayFrame(pSpriteFrame);

	return bRet;
}

void ColorSprite::draw(void)  
{  
	//CCLog("override draw!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");  
	CC_PROFILER_START_CATEGORY(kCCProfilerCategorySprite, "CCSprite - draw");  

	CCAssert(!m_pobBatchNode, "If CCSprite is being rendered by CCSpriteBatchNode, CCSprite#draw SHOULD NOT be called");  

	CC_NODE_DRAW_SETUP();  

	// <启用attributes变量输入，顶点坐标，纹理坐标，颜色  
	//  
	ccGLEnableVertexAttribs( kCCVertexAttribFlag_PosColorTex );  
	ccGLBlendFunc(m_sBlendFunc.src, m_sBlendFunc.dst);  

	m_pShaderProgram->use();  
	m_pShaderProgram->setUniformsForBuiltins();  

	// <绑定纹理到纹理槽0  
	ccGLBindTexture2D(m_pobTexture->getName());  



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

ColorSprite* ColorSprite::createWithSpriteFrameName(const char* sprName)
{
	CCSpriteFrame *pFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(sprName);

#if COCOS2D_DEBUG > 0
	char msg[256] = {0};
	sprintf(msg, "Invalid spriteFrameName: %s", sprName);
	CCAssert(pFrame != NULL, msg);
#endif

	return createWithSpriteFrame(pFrame);
}

ColorSprite* ColorSprite::createWithSpriteFrame(CCSpriteFrame *pSpriteFrame)
{
	ColorSprite *pobSprite = new ColorSprite();
	if (pSpriteFrame && pobSprite && pobSprite->initWithSpriteFrame(pSpriteFrame))
	{
		pobSprite->autorelease();
		return pobSprite;
	}
	CC_SAFE_DELETE(pobSprite);
	return NULL;
}


