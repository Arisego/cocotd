#include "Container.h"

Container::Container(){
	m_bIsEnabled = true;
	mLasto = NULL;
}

void Container::setEnability(bool tar){
	m_bIsEnabled = tar;
	if(!m_bIsEnabled) onDisable();
	else onNormal();
}

void Container::activate()
{
	if (m_bIsEnabled)
	{
		if (m_pListener && m_pfnSelector)
		{
			(m_pListener->*m_pfnSelector)(this);
		}
	}
}

bool Container::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
	if(!isVisible()) return false;
	if(checkTouch(pTouch)){
		onSelect();activate();	
				
	}
	return true;
}

void Container::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent){
	if(!isVisible()) return;
	if(!mLasto) mLasto = new CCTouch();
	mLasto->setTouchInfo(pTouch->getID(),pTouch->getLocationInView().x,pTouch->getLocationInView().y);
}

void Container::update(float fDelta){
	if(mLasto && m_iState != C_STATE_SELECT){
		if(checkTouch(mLasto)){
			onHover();
		}else{
			onNormal();
			CC_SAFE_RELEASE_NULL(mLasto);
		}
	}
}

bool Container::checkTouch(CCTouch *pTouch){				//当需要更精确的tab判定时继承并修改修改即可
	if( getContentSize().width * getContentSize().height > 1.0f )
	{
		CCPoint pt = pTouch->getLocation();
		CCRect rcBoundingBox( 0, 0, getContentSize().width, getContentSize().height );
		rcBoundingBox = CCRectApplyAffineTransform( rcBoundingBox, nodeToWorldTransform() );
		//[PassMessageDebug] CCLOG("Check here:%f,%f in %f,%f,%f,%f",pt.x,pt.y,rcBoundingBox.origin.x,rcBoundingBox.origin.y,rcBoundingBox.size.width,rcBoundingBox.size.height);
		if( !rcBoundingBox.containsPoint( pt ) )
		{
			//[PassMessageDebug] CCLOG(">Check the collide rect:failed;");
			return false;
		}

	}else{
		return false;
	}
	return true;
}

void Container::initwithPre(string c9img,float width,float height){
	string s;
	s = c9img;
	spriteNormal = CCScale9Sprite::create(s.append("_normal.png").c_str()); 
	spriteNormal->setContentSize(CCSize(width,height));
	spriteNormal->setAnchorPoint(ccp(0,0));
	addChild(spriteNormal);

	s = c9img;
	spriteHover = CCScale9Sprite::create(s.append("_hover.png").c_str()); 
	spriteHover->setContentSize(CCSize(width,height));
	spriteHover->setAnchorPoint(ccp(0,0));
	spriteHover->setVisible(false);
	addChild(spriteHover);

	s = c9img;
	spriteDisable = CCScale9Sprite::create(s.append("_disable.png").c_str()); 
	spriteDisable->setContentSize(CCSize(width,height));
	spriteDisable->setAnchorPoint(ccp(0,0));
	spriteDisable->setVisible(false);
	addChild(spriteDisable);

	s = c9img;
	spriteSelected = CCScale9Sprite::create(s.append("_select.png").c_str()); 
	spriteSelected->setContentSize(CCSize(width,height));
	spriteSelected->setAnchorPoint(ccp(0,0));
	spriteSelected->setVisible(false);
	addChild(spriteSelected);

	m_obContentSize.width = width;
	m_obContentSize.height = height;

	mLasto = NULL;
	m_iState = C_STATE_NORMAL;
	scheduleUpdate();
}

void Container::initwithPsz(const char* s_MenuItem,const char* s_Title,float width,float height, CCObject* target, SEL_MenuHandler selector){
	spriteDisable = CCSprite::create(s_MenuItem, CCRectMake(0,height*3,width,height));
	spriteDisable->setAnchorPoint(ccp(0,0));
	spriteDisable->setPosition(ccp(0,0));
	spriteDisable->setVisible(false);
	addChild(spriteDisable);

	spriteSelected = CCSprite::create(s_MenuItem, CCRectMake(0,height*2,width,height));
	spriteSelected->setAnchorPoint(ccp(0,0));
	spriteSelected->setPosition(ccp(0,0));
	spriteSelected->setVisible(false);
	addChild(spriteSelected);

	spriteHover = CCSprite::create(s_MenuItem, CCRectMake(0,height*1,width,height));
	spriteHover->setAnchorPoint(ccp(0,0));
	spriteHover->setPosition(ccp(0,0));
	spriteHover->setVisible(false);
	addChild(spriteHover);

	spriteNormal = CCSprite::create(s_MenuItem, CCRectMake(0,height*0,width,height));
	spriteNormal->setAnchorPoint(ccp(0,0));
	spriteNormal->setPosition(ccp(0,0));
	addChild(spriteNormal);

	labelAtlas = CCLabelBMFont::create(s_Title, "fonts/CocoTd.fnt");
	labelAtlas->setPosition(ccp(width/2,height/2));
	addChild(labelAtlas,1);

	m_obContentSize.width = width;
	m_obContentSize.height = height;
	m_pListener = target;
	m_pfnSelector = selector;
	m_bIsEnabled = true;

	mLasto = NULL;
	m_iState = C_STATE_NORMAL;
	scheduleUpdate();
}

void Container::initString(const char* s_Title,float width,float height,string c9img, CCObject* target, SEL_MenuHandler selector){
	//spriteNormal = new CCNode();
	//spriteSelected = new CCNode();
	//spriteHover = new CCNode();
	//addChild(spriteHover);
	//addChild(spriteNormal);
	//addChild(spriteSelected);

	initwithPre(c9img,width,height);

	labelAtlas = CCLabelBMFont::create(s_Title, "fonts/CocoTd.fnt");
	labelAtlas->setPosition(ccp(width/2,height/2));
	addChild(labelAtlas,1);

	m_obContentSize.width = width;
	m_obContentSize.height = height;
	m_pListener = target;
	m_pfnSelector = selector;
	m_bIsEnabled = true;
}

void Container::onDisable(){
	if(C_STATE_DISABLE == m_iState) return;
	m_iState = C_STATE_DISABLE;
	labelAtlas->setColor(ccGRAY);
	SpriteRefresh();
}

void Container::onHover(){			
	if(C_STATE_HOVERD == m_iState || !m_bIsEnabled) return;
	m_iState = C_STATE_HOVERD;
	labelAtlas->setColor(ccRED);
	SpriteRefresh();

}

void Container::onSelect(){
	if(C_STATE_SELECT == m_iState || !m_bIsEnabled) return;
	m_iState = C_STATE_SELECT;
	labelAtlas->setColor(ccORANGE);
	SpriteRefresh();
}

void Container::onNormal(){
	if(C_STATE_NORMAL == m_iState || !m_bIsEnabled) return;
	m_iState = C_STATE_NORMAL;
	labelAtlas->setColor(ccWHITE);
	SpriteRefresh();
}

void Container::SpriteRefresh(){
	m_bIsEnabled = (m_iState != C_STATE_DISABLE);
	spriteHover->setVisible(m_iState == C_STATE_HOVERD);
	spriteNormal->setVisible(m_iState == C_STATE_NORMAL);
	spriteSelected->setVisible(m_iState == C_STATE_SELECT);
	spriteDisable->setVisible(m_iState == C_STATE_DISABLE);
}

void Container::setactivator( CCObject* target, SEL_MenuHandler selector )
{
	m_pListener = target;
	m_pfnSelector = selector;
	m_bIsEnabled = true;
}

void Container::settouchstate( bool tar )
{
	m_bIsEnabled = tar;
}

void Container::setOpacity( GLubyte opacity )
{
	dynamic_cast<CCRGBAProtocol*>(spriteHover)->setOpacity(opacity);
	dynamic_cast<CCRGBAProtocol*>(spriteNormal)->setOpacity(opacity);
	dynamic_cast<CCRGBAProtocol*>(spriteDisable)->setOpacity(opacity);
	dynamic_cast<CCRGBAProtocol*>(spriteSelected)->setOpacity(opacity);
}

void Container::setstringnull()
{
	labelAtlas = CCLabelBMFont::create("", "fonts/CocoTd.fnt");
	labelAtlas->setPosition(CCPointZero);
	labelAtlas->setVisible(false);
	addChild(labelAtlas,1);
}

void Container::setstring(string s){
	labelAtlas->setString(s.c_str());
	labelAtlas->setPosition(ccp(m_obContentSize.width/2,m_obContentSize.height/2));
	labelAtlas->setVisible(true);
}
