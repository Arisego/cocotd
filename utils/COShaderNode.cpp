#include "COShaderNode.h"
#include "cocos2d.h"

///---------------------------------------
// 
// COShaderNode
// 
///---------------------------------------


COShaderNode::COShaderNode()
	:m_center(vertex2(0.0f, 0.0f))
	,m_resolution(vertex2(0.0f, 0.0f))
	,m_time(0.0f)
	,m_uniformCenter(0)
	,m_uniformResolution(0)
	,m_uniformTime(0)
	,m_uniformMouse(0)
	,m_uniformTex(0)
	,x(0)
	,y(0)
	,z(0)
{
	CCDirector* pDirector = CCDirector::sharedDirector();
	pDirector->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority, true);
}

COShaderNode::~COShaderNode()
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, EVENT_COME_TO_FOREGROUND);
	CCDirector* pDirector = CCDirector::sharedDirector();
	pDirector->getTouchDispatcher()->removeDelegate(this);
}

COShaderNode* COShaderNode::shaderNodeWithVertex(const char *vert, const char *frag)
{
	COShaderNode *node = new COShaderNode();
	node->initWithVertex(vert, frag);
	node->autorelease();

	return node;
}

bool COShaderNode::initWithVertex(const char *vert, const char *frag)
{
	CCNotificationCenter::sharedNotificationCenter()->addObserver(this,
		callfuncO_selector(COShaderNode::listenBackToForeground),
		EVENT_COME_TO_FOREGROUND,
		NULL);

	loadShaderVertex(vert, frag);

	size_x = 256;
	size_y = 256;

	m_time = 0;
	m_resolution = vertex2(size_x, size_y);

	scheduleUpdate();

	setContentSize(CCSizeMake(size_x, size_y));
	setAnchorPoint(ccp(0.5f, 0.5f));

	m_vertFileName = vert;
	m_fragFileName = frag;

	return true;
}

void COShaderNode::listenBackToForeground(CCObject *obj)
{
	this->setShaderProgram(NULL);
	loadShaderVertex(m_vertFileName.c_str(), m_fragFileName.c_str());
}

void COShaderNode::loadShaderVertex(const char *vert, const char *frag)
{
	CCGLProgram *shader = new CCGLProgram();
	shader->initWithVertexShaderFilename(vert, frag);

	shader->addAttribute("aVertex", kCCVertexAttrib_Position);
	shader->link();

	shader->updateUniforms();

	m_uniformCenter = glGetUniformLocation(shader->getProgram(), "center");
	m_uniformResolution = glGetUniformLocation(shader->getProgram(), "resolution");
	m_uniformTime = glGetUniformLocation(shader->getProgram(), "time");
	m_uniformMouse = glGetUniformLocation(shader->getProgram(), "iMouse");
	m_uniformTex = glGetUniformLocation(shader->getProgram(),"iChannel0");

	this->setShaderProgram(shader);

	shader->release();
}

void COShaderNode::update(float dt)
{
	//CCLog(">[COShaderNode] update()");
	m_time += dt;
}

void COShaderNode::setPosition(const CCPoint &newPosition)
{
	CCNode::setPosition(newPosition);
	CCPoint position = getPosition();
	m_center = vertex2(position.x * CC_CONTENT_SCALE_FACTOR(), position.y * CC_CONTENT_SCALE_FACTOR());
}

void COShaderNode::draw()
{
	CC_NODE_DRAW_SETUP();

	float w = size_x, h = size_y;
	GLfloat vertices[12] = {0,0, w,0, w,h, 0,0, 0,h, w,h};

	//
	// Uniforms
	//
	getShaderProgram()->setUniformLocationWith2f(m_uniformCenter, m_center.x, m_center.y);
	getShaderProgram()->setUniformLocationWith2f(m_uniformResolution, m_resolution.x, m_resolution.y);
	getShaderProgram()->setUniformLocationWith4f(m_uniformMouse, x, y, z, 0);
	//getShaderProgram()->setUniformLocationWith1i(m_uniformTex,m_tex1->getName());
	
	//m_uniformMouse

	// time changes all the time, so it is Ok to call OpenGL directly, and not the "cached" version
	glUniform1f(m_uniformTime, m_time);

	ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );

	glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	CC_INCREMENT_GL_DRAWS(1);
}

void COShaderNode::setSize( float a, float b )
{
	m_resolution.x = a;
	m_resolution.y = b;
	size_x = a;
	size_y = b;

	setContentSize(CCSizeMake(size_x,size_y));


}

//////////////////////////////////////////////////////////////////////////
// <鼠标事件响应，填充到uniform中
bool COShaderNode::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	CCLog(">[COSN]Mouse click down!!");
	z = 1.0;
	return true;
}

void COShaderNode::ccTouchMoved( CCTouch *pTouch, CCEvent *pEvent )
{
	if(z<=0) return;
	CCPoint tcp = pTouch->getLocation();
	x = tcp.x;
	y = tcp.y;
}

void COShaderNode::ccTouchEnded( CCTouch *pTouch, CCEvent *pEvent )
{
	z = 0.0;
}

void COShaderNode::ccTouchCancelled( CCTouch *pTouch, CCEvent *pEvent )
{
	z = 0.0;
}

//////////////////////////////////////////////////////////////////////////
//void COShaderNode::initTexure( const char* cname )
//{
//	CCSprite* cs = CCSprite::create("tex16.png");
//	m_tex1 = cs->getTexture();
//	cs->retain();
//	
//}


