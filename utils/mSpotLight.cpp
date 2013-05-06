#include "mSpotLight.h"

#define USING_CCRENDERTEX 

mSpotLight::mSpotLight()  
{  
	m_iLaDep = 0;
	m_spotLightRadius = 0;  
	m_renderColor.r = 0;  
	m_renderColor.g = 0;  
	m_renderColor.b = 0.5;  
	m_renderColor.a = 0.5;  

	m_Board = NULL;
	m_AkaruCircle = NULL;
	m_StencilCircle = NULL;

	m_bAvalible = true;
}  

void mSpotLight::setRenderColor(const ccColor4F& var){ 
	if(ccc4FEqual(m_renderColor,var)) return;
				
	m_renderColor = var;	
	f_refresh_circles();


	
}

bool mSpotLight::f_init(CCNode* cn_borad,int i_ld){
	do 
	{
		//new CCPoint[1];
		m_iLaDep = i_ld;
		m_Board = cn_borad;

		CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();

		
		csb_st = CCSpriteBatchNode::create("Images/circle.png");

		cp_board = CCClippingNode::create();
		cp_board->setContentSize(visibleSize);
		cp_board->setPosition(CCPointZero);
		cp_board->setAnchorPoint(CCPointZero);
		cp_board->setStencil(csb_st);
		cp_board->setInverted(true);

		//////////////////////////////////////////////////////////////////////////

		CCLayerColor* clc = CCLayerColor::create(ccc4BFromccc4F(m_renderColor),visibleSize.width,visibleSize.height);
		clc->setAnchorPoint(CCPointZero);
		clc->setPosition(CCPointZero);
		cp_board->addChild(clc);			//content
		cn_borad->addChild(cp_board,m_iLaDep+1);

		CC_BREAK_IF(!f_refresh_circles());



		//m_StencilCircle = CCSprite::create("Images/circle.png");

		//m_StencilCircle->setPosition(ccp(200,200));
		//m_StencilCircle->setScale(4);
		//csb_st->addChild(m_StencilCircle);



		return true;
	} while (0);

	return false;

}
 
void mSpotLight::f_clear_circles()  
{  
	cp_board->removeFromParent();
	m_AkaruCircle->removeFromParent();
}    

void mSpotLight::setRenderRect(float nx, float ny){
	if(!m_AkaruCircle) return;

	x = nx;
	y = ny;
	
	CCPoint p = cp_board->convertToNodeSpaceAR(ccp(x,y));
	m_AkaruCircle->setPosition(p);
	m_StencilCircle->setPosition(p);
}

void mSpotLight::generate_circle()
{

#ifdef USING_CCRENDERTEX
	
	if(m_AkaruCircle){
		m_AkaruCircle->removeFromParent();
		m_AkaruCircle	=	NULL;
		//CC_SAFE_RELEASE_NULL(m_AkaruCircle);
	}
	ccBlendFunc cbf = {GL_ZERO, GL_ONE_MINUS_SRC_ALPHA};

	CCSprite* t_sp = CCSprite::create("Images/circle.png");
	t_sp->setPosition(CCPointZero);
	t_sp->setAnchorPoint(CCPointZero);
	t_sp->setBlendFunc(cbf);
	
	CCSize vs = t_sp->getContentSize();
	CCRenderTexture* t_crt = CCRenderTexture::create(vs.width, vs.height);
	t_crt->beginWithClear(m_renderColor.r, m_renderColor.g, m_renderColor.b, m_renderColor.a);
	
	t_sp->visit();
	
	t_crt->end();

	m_AkaruCircle = CCSprite::createWithTexture(t_crt->getSprite()->getTexture());
	//m_AkaruCircle->autorelease();
	m_AkaruCircle->setScale(4);
	m_Board->addChild(m_AkaruCircle,m_iLaDep);

#else
	//////////////////////////////////////////////////////////////////////////
	if(!m_AkaruCircle){
		ccBlendFunc cbf = {GL_DST_COLOR, GL_ZERO};

		cns_blocks = CCSpriteBatchNode::create("Images/circle.png");
		cns_blocks->setBlendFunc(cbf);
		cp_board->addChild(cns_blocks);

		////////////////////////////////////////////////////////////////////////////
		m_AkaruCircle = CCSprite::create("Images/circle.png");
		m_AkaruCircle->setBlendFunc(cbf);
		m_AkaruCircle->setPosition(ccp(200,200));
		m_AkaruCircle->setScale(4);
		m_Board->addChild(m_AkaruCircle,m_iLaDep);
	}

#endif

	if(!m_StencilCircle)
	{
		m_StencilCircle = CCSprite::create("Images/circle.png");
		//m_StencilCircle->autorelease();
		m_StencilCircle->setPosition(ccp(200,200));
		m_StencilCircle->setScale(4);
		csb_st->addChild(m_StencilCircle);
	}
}

bool mSpotLight::f_refresh_circles()
{
	if(!m_Board) return false;		//Never Refresh Circle when there is no parent node.

	if(ccc4FEqual(m_renderColor,ccc4f(0,0,0,0))){
		CCLOG(">No need for circle");
		if(m_AkaruCircle){
			m_AkaruCircle->removeFromParent();
			m_AkaruCircle = NULL;
			//CC_SAFE_RELEASE_NULL(m_AkaruCircle);
		}
		if(m_StencilCircle){
			csb_st->removeAllChildren();
			m_StencilCircle = NULL;
		}

		cp_board->setInverted(false);
		return false;
	}

	generate_circle();
	return true;

}

void mSpotLight::f_state_circle( bool st )
{
	if(m_bAvalible == st) return;

	m_bAvalible = st;
	cp_board->setVisible(m_bAvalible);
	m_AkaruCircle->setVisible(m_bAvalible);
}

void mSpotLight::nouse()
{
	new CCPoint[1];
}
