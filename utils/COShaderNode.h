#ifndef __CO_SHADER_NODE_H__
#define __CO_SHADER_NODE_H__

#include "cocos2d.h"

USING_NS_CC;

/*
	<�����Ҫͨ����texture��������m_tex1�Ĵ���ע�͵�
*/
class COShaderNode : public CCNode, public CCTouchDelegate
{
public:
	COShaderNode();
	~COShaderNode();

	bool initWithVertex(const char *vert, const char *frag);
	void loadShaderVertex(const char *vert, const char *frag);
	void listenBackToForeground(CCObject *obj);

	virtual void update(float dt);
	virtual void setPosition(const CCPoint &newPosition);
	virtual void draw();
	virtual void setSize(float a, float b);		//Set the size.

	//void initTexure(const char* cname);

	static COShaderNode* shaderNodeWithVertex(const char *vert, const char *frag);

private:

	float size_x;
	float size_y;


	ccVertex2F m_center;
	ccVertex2F m_resolution;
	float      m_time;
	GLuint     m_uniformCenter, m_uniformResolution, m_uniformTime, m_uniformMouse, m_uniformTex;
	std::string m_vertFileName;
	std::string m_fragFileName;

	//CCTexture2D* m_tex1;

protected:
	//////////////////////////////////////////////////////////////////////////
	// <��괫��
	float x,y,z;		//  <x,y ����ֵ || z �����Ƿ��ڰ���״̬ || vec4���Ĳ��� click ���岻��

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
};

#endif