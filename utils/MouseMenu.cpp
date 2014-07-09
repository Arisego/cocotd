#include "MouseMenu.h"
#include "SoundManager.h"
#include "EventCenter.h"

MouseMenu* MouseMenu::createWithArray(CCArray* pArrayOfItems)
{
	MouseMenu *pRet = new MouseMenu();
	if (pRet && pRet->initWithArray(pArrayOfItems))
	{
		pRet->autorelease();
		pRet->f_init();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}
	
	return pRet;
}

MouseMenu * MouseMenu::menuWithItems(CCMenuItem* item, ...)
{
    va_list args;
    va_start(args,item);
    MouseMenu *pRet = new MouseMenu();//这里记得要注意


	CCArray* pArray = NULL;
	if( item )
    {
        pArray = CCArray::create(item, NULL);
        CCMenuItem *i = va_arg(args, CCMenuItem*);
        while(i)
        {
            pArray->addObject(i);
            i = va_arg(args, CCMenuItem*);
        }
    }


    if (pRet && pRet->initWithArray(pArray))
    {
        pRet->autorelease();
        va_end(args);
		pRet->f_init();
        return pRet;
    }
    va_end(args);
    CC_SAFE_DELETE(pRet);
    return NULL;
}

MouseMenu* MouseMenu::menuWithItem(CCMenuItem* item)
{
    return menuWithItems(item, NULL);
}

void MouseMenu::f_init(){
	lastto = NULL;

	miNums		=	this->getChildrenCount();
	miHovered	=	-1;
	miOver		=	-1;
	miAlignType =	1;

	this->scheduleUpdate();
	Regist();
}

void MouseMenu::onExit()
{
	if (m_eState == kCCMenuStateTrackingTouch && m_pSelectedItem != NULL)
	{
		m_pSelectedItem->unselected();
		m_eState = kCCMenuStateWaiting;
		m_pSelectedItem = NULL;
	}
	CC_SAFE_RELEASE_NULL(lastto);
	UnRegist();
	CCLayer::onExit();
}

void MouseMenu::registerWithTouchDispatcher(){
    CCDirector* pDirector = CCDirector::sharedDirector();
	pDirector->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority, true);
}


bool MouseMenu::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    CC_UNUSED_PARAM(event);
    if (
		m_eState != kCCMenuStateWaiting || 
		! m_bVisible || !isEnabled())
    {
        return false;
    }

    for (CCNode *c = this->m_pParent; c != NULL; c = c->getParent())
    {
        if (c->isVisible() == false)
        {
            return false;
        }
    }

	CCLog(">[MouseMenu] ccTouchBegan()");
	CCMenuItem* tcs = this->itemForTouch(touch);
   
    if (tcs)
    {
		 m_pSelectedItem = tcs;
        m_eState = kCCMenuStateTrackingTouch;
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32 )
        m_pSelectedItem->selected();
#endif
        return true;
    }

	CCLog(">[MouseMenu] ccTouchBegan() | Nothing being touched.");
    return false;
}

void MouseMenu::ccTouchEnded(CCTouch *touch, CCEvent* event)
{

    CC_UNUSED_PARAM(touch);
    CC_UNUSED_PARAM(event);
    CCAssert(m_eState == kCCMenuStateTrackingTouch, "[Menu ccTouchEnded] -- invalid state");

	CCLog(">[MouseMenu] ccTouchEnded()");
	if(miOver == miHovered && miHovered != -1) return;

    if (m_pSelectedItem)
    {
        m_pSelectedItem->unselected();
        m_pSelectedItem->activate();
		m_pSelectedItem = NULL;
		CC_SAFE_RELEASE_NULL(lastto);
		mbEnabled = false;
		SoundManager::sharedSoundManager()->PlayDecideSFX();
    }
    m_eState = kCCMenuStateWaiting;
}

void MouseMenu::ccTouchCancelled(CCTouch *touch, CCEvent* event)
{
    CC_UNUSED_PARAM(touch);
    CC_UNUSED_PARAM(event);
    CCAssert(m_eState == kCCMenuStateTrackingTouch, "[Menu ccTouchCancelled] -- invalid state");
    if (m_pSelectedItem)
    {
        m_pSelectedItem->unselected();
    }
    m_eState = kCCMenuStateWaiting;
}

void MouseMenu::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
    CC_UNUSED_PARAM(event);

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32 )
    CCAssert(m_eState == kCCMenuStateTrackingTouch, "[Menu ccTouchMoved] -- invalid state");
#endif
	if(!lastto) lastto = new CCTouch();
	lastto->setTouchInfo(touch->getID(),touch->getLocationInView().x,touch->getLocationInView().y);

}

void MouseMenu::update(float dt){
	//CCLog(">[MouseMenu] update()");
	if(!lastto) return;
	if(!mbEnabled) return;

	miOver = -1;
	CCMenuItem *currentItem = this->itemForTouch(lastto);		
	if(NULL != currentItem){
		if (currentItem != m_pSelectedItem ) 
		{
			SoundManager::sharedSoundManager()->PlayHoverSFX();
			if (m_pSelectedItem)
			{
				m_pSelectedItem->unselected();
			}
			m_pSelectedItem = currentItem;
			if (m_pSelectedItem)
			{
				m_pSelectedItem->selected();
				miOver = m_pSelectedItem->getTag() -100;
				miHovered = miOver;
			}
		}
	}else{
		//if (m_pSelectedItem)
		//{
		//	//SoundManager::sharedSoundManager()->PlayLaserSFX();
		//	m_pSelectedItem->unselected();
		//	m_pSelectedItem = NULL;
		//	CC_SAFE_RELEASE_NULL(lastto);
		//}
	}
}

void MouseMenu::ReSume()
{
	mbEnabled = true;
}

MouseMenu::MouseMenu():
	mbEnabled(true),
	miHovered(-1),
	miOver(-1),
	mbRegisted(false)
{

}

void MouseMenu::s_press(bool up /*= false*/)
{
	if(!mbEnabled) return;
	if(!up) return;
	if(miAlignType != 1) return;

	++miHovered;
	CCMenuItem* tci = (CCMenuItem*) this->getChildByTag(miHovered+100);
	CCLog(">[MouseMenu] s_press() | %d", miHovered);
	if(!tci){
		--miHovered;
		return;
	}

	SoundManager::sharedSoundManager()->PlayHoverSFX();
	if(m_pSelectedItem) m_pSelectedItem->unselected();
	m_pSelectedItem = tci;
	m_pSelectedItem->selected();
}

void MouseMenu::w_press(bool up /*= false*/)
{
	if(!mbEnabled) return;
	if(!up) return;
	if(miAlignType != 1) return;

	if(miHovered == -1) miHovered = miNums;
	--miHovered;
	CCMenuItem* tci = (CCMenuItem*) this->getChildByTag(miHovered+100);
	CCLog(">[MouseMenu] w_press() | %d", miHovered);
	if(!tci){
		++miHovered;
		return;
	}

	SoundManager::sharedSoundManager()->PlayHoverSFX();
	if(m_pSelectedItem) m_pSelectedItem->unselected();
	m_pSelectedItem = tci;
	m_pSelectedItem->selected();
}

void MouseMenu::a_press(bool up /*= false*/)
{
	if(!mbEnabled) return;
	if(!up) return;
	if(miAlignType != 2) return;

	if(miHovered == -1) miHovered = miNums;
	--miHovered;
	CCMenuItem* tci = (CCMenuItem*) this->getChildByTag(miHovered+100);
	CCLog(">[MouseMenu] a_press() | %d", miHovered);
	if(!tci){
		++miHovered;
		return;
	}

	SoundManager::sharedSoundManager()->PlayHoverSFX();
	if(m_pSelectedItem) m_pSelectedItem->unselected();
	m_pSelectedItem = tci;
	m_pSelectedItem->selected();
}

void MouseMenu::d_press(bool up /*= false*/)
{
	if(!mbEnabled) return;
	if(!up) return;
	if(miAlignType != 2) return;

	++miHovered;
	CCMenuItem* tci = (CCMenuItem*) this->getChildByTag(miHovered+100);
	CCLog(">[MouseMenu] d_press() | %d", miHovered);
	if(!tci){
		--miHovered;
		return;
	}

	SoundManager::sharedSoundManager()->PlayHoverSFX();
	if(m_pSelectedItem) m_pSelectedItem->unselected();
	m_pSelectedItem = tci;
	m_pSelectedItem->selected();
}

void MouseMenu::z_press(bool up /*= false*/)
{
	if(!mbEnabled) return;
	if (m_pSelectedItem)
	{
		m_pSelectedItem->unselected();
		m_pSelectedItem->activate();
		m_pSelectedItem = NULL;
		CC_SAFE_RELEASE_NULL(lastto);
		mbEnabled = false;
		SoundManager::sharedSoundManager()->PlayDecideSFX();
	}
	m_eState = kCCMenuStateWaiting;
}

void MouseMenu::f_setaligntype(int ai)
{
	miAlignType = ai;
}

void MouseMenu::UnRegist()
{
	if(mbRegisted){
		mbRegisted = false;
		EventCenter::sharedEventCenter()->setController(NULL);		
	}
}

void MouseMenu::Regist()
{
	if(mbRegisted) return;
	EventCenter::sharedEventCenter()->setController(this);
	mbRegisted = true;
}
