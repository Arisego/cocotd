//
//  BYTouchDelegate.h
//  TableTest
//
//  Created by jason on 12-12-25.
//
//

#ifndef __TableTest__BYTouchDelegate__
#define __TableTest__BYTouchDelegate__

#include "cocos2d.h"

#define CHILD_MAX 20

USING_NS_CC;

//by message mechanism( by represents beyond )
//node who want to receive by message must derive from BYTouchDelegate.
//if node is layer, it need use macro( BY_MESSAGE_BRIDGE() ) to connect it to by message instead of ccTouchMessage.
class BYTouchDelegate
{
public:
	bool m_bIsEnabled;

    BYTouchDelegate( CCNode* pOwner ) :
    m_pOwner( pOwner ),
    m_bDraging( false )
    {
		m_bIsEnabled = true;

        m_pItemsClaimTouch = CCArray::createWithCapacity( CHILD_MAX );
        assert( m_pItemsClaimTouch );
        m_pItemsClaimTouch->retain();
        
        m_pMenusClaimTouch = CCArray::createWithCapacity( CHILD_MAX );
        assert( m_pMenusClaimTouch );
        m_pMenusClaimTouch->retain();
    }
    virtual ~BYTouchDelegate()
    {
		CCLOG(">BYLayer_DEC");

		//m_pMenusClaimTouch->removeAllObjects();
		//m_pItemsClaimTouch->removeAllObjects();

        CC_SAFE_RELEASE_NULL( m_pItemsClaimTouch );
        CC_SAFE_RELEASE_NULL( m_pMenusClaimTouch );
    }
protected:
    // default implements are used to call script callback if exist
    virtual bool byTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void byTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void byTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void byTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
    //return value:
    //true: pParent is touched by user
    //false: pParent isn't touched by user.
    bool passMessage( CCNode* pParent, CCTouch *pTouch, CCEvent *pEvent );
	bool passMove( CCNode* pParent, CCTouch *pTouch, CCEvent *pEvent );
private:
    CCNode* m_pOwner;
    bool m_bDraging;
    //items claim touch message
    CCArray* m_pItemsClaimTouch;
    CCArray* m_pMenusClaimTouch;
};
#endif /* defined(__TableTest__BYTouchDelegate__) */

