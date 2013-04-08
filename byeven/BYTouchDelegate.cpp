//
//  BYTouchDelegate.cpp
//  TableTest
//
//  Created by jason on 12-12-25.
//
//
#include "utils/MouseMenu.h"

#include "BYTouchDelegate.h"
#include "BYUtility.h"
#pragma mark- input touche
bool BYTouchDelegate::byTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	//pass message to all children
	return passMessage( m_pOwner, pTouch, pEvent );
}

void BYTouchDelegate::byTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 )
	passMove( m_pOwner, pTouch, pEvent );
#else
	//special process for menu, we won't pass ccTouchMoved message to menu. Because we think menu doesn't need ccTouchMoved message in ios device where user always want to dray layer instead menu. The fllowing block for menu will only go once.
	int iNumMenus = m_pMenusClaimTouch->count();
	for( int i = 0; i < iNumMenus; ++i )
	{
		( ( CCMenu* )m_pMenusClaimTouch->objectAtIndex( i ) )->ccTouchCancelled( pTouch, pEvent );        
	}

	if( iNumMenus > 0 )
	{
		m_pMenusClaimTouch->removeAllObjects();
	}


	//pass ccTouchMoved message to un-CCMenu item
	for( int i = 0; i < m_pItemsClaimTouch->count(); ++i )
	{
		( ( CCLayer* )m_pItemsClaimTouch->objectAtIndex( i ) )->ccTouchMoved( pTouch, pEvent );
	}
#endif

}

void BYTouchDelegate::byTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	//for menus
	for( int i = 0; i < m_pMenusClaimTouch->count(); ++i )
	{
		( ( CCMenu* )m_pMenusClaimTouch->objectAtIndex( i ) )->ccTouchEnded( pTouch, pEvent );
	}
	m_pMenusClaimTouch->removeAllObjects();

	//for items not menu
	for( int i = 0; i < m_pItemsClaimTouch->count(); ++i )
	{
		( ( CCLayer* )m_pItemsClaimTouch->objectAtIndex( i ) )->ccTouchEnded( pTouch, pEvent );
	}
	m_pItemsClaimTouch->removeAllObjects();
}

void BYTouchDelegate::byTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
	//for menus
	for( int i = 0; i < m_pMenusClaimTouch->count(); ++i )
	{
		( ( CCMenu* )m_pMenusClaimTouch->objectAtIndex( i ) )->ccTouchCancelled( pTouch, pEvent );
	}
	m_pMenusClaimTouch->removeAllObjects();

	//for items not menu
	for( int i = 0; i < m_pItemsClaimTouch->count(); ++i )
	{
		( ( CCLayer* )m_pItemsClaimTouch->objectAtIndex( i ) )->ccTouchCancelled( pTouch, pEvent );
	}
	m_pItemsClaimTouch->removeAllObjects();
}

bool BYTouchDelegate::passMessage( CCNode* pParent, CCTouch *pTouch, CCEvent *pEvent )
{
	if(!m_bIsEnabled || !pParent || !pParent->isVisible() )
	{
		return false;
	}

	//hande message to items
	int iNumChildren = 0;
	CCArray* pChildren = NULL;

	//[Mes] CCLOG("[Mes]>Check the collide rect.");
	//if the item'size > 1, check whether use touches it. Such as TableView.
	//some items doesn't get size. they are medium for maintaining some children. Such as CCTableViewCell.
	if( pParent->getContentSize().width * pParent->getContentSize().height > 1.0f )
	{
		CCPoint pt = pTouch->getLocation();
		CCRect rcBoundingBox( 0, 0, pParent->getContentSize().width, pParent->getContentSize().height );
		//do this only for efficiency, because convertToNodeSpace is heavier than nodeToWorldTransform().
		rcBoundingBox = CCRectApplyAffineTransform( rcBoundingBox, pParent->nodeToWorldTransform() );
		//[Mes] CCLOG("[Mes]Check here:%f,%f in %f,%f,%f,%f",pt.x,pt.y,rcBoundingBox.origin.x,rcBoundingBox.origin.y,rcBoundingBox.size.width,rcBoundingBox.size.height);
		//whether hit the node
		if( !rcBoundingBox.containsPoint( pt ) )
		{
			//[Mes] CCLOG("[Mes]>Check the collide rect:failed;");
			return false;
		}
		
	}

	//CCTouchDelegate* ptd;
	//if( ( ptd = dynamic_cast< CCTouchDelegate* >( pParent ) ) )
	//{
	//	ptd->ccTouchBegan( pTouch, pEvent );
	//}

	//[Mes] CCLOG("[Mes]>Check the collide rect:success;");
	pChildren = pParent->getChildren();

	//no children, but user touch this item, so return true.
	if( !pChildren )
	{
		return true;
	}

	iNumChildren = pParent->getChildren()->count();

	//[Mes] CCLOG("[Mes]>Preparing passing to children:--%d",iNumChildren);
	//pass to all children
	for( int iChildIndex = 0; iChildIndex < iNumChildren; ++iChildIndex )
	{
		//[Mes] CCLOG("[Mes]>Preparing passing to children:%d-%d",iChildIndex,iNumChildren);
		//if the item claims the touch message
		bool bClaim = false;

		CCLayer* pLayer = NULL;
		CCNode* pNode = NULL;

		pNode = ( CCNode*  )( pChildren->objectAtIndex( iChildIndex ) );
		assert( pNode );

		//if it's layer, we should invoke its ccTouchBegan()
		//Make sure that you have commented the CCAssertion statement in CCLayer::ccTouchBegan().
			if( ( pLayer = dynamic_cast< CCLayer* >( pNode ) ) )
			{
				bClaim = pLayer->ccTouchBegan( pTouch, pEvent );
			}

			//pass message for its child if it doesn't derive BYTouchDelegate. Because child deriving BYTouchDelegate has passed message via ccTouchBegan().
			if( !dynamic_cast< BYTouchDelegate* >( pNode ) )
			{
				//items who doesn't derive from BYTouchDelegate can't pass touch message to its children,
				//so we have to help them to pass touch message.
				passMessage( pNode, pTouch, pEvent );
			}

			//if this item is interested in this message, add it to array for other messages
			if( bClaim )
			{
				//we don't use condition of &typeid( *pNode ) == &typeid( CCMenu ) since user may derive CCMenu.
				if ( dynamic_cast< CCMenu* >( pNode ) )
				{
					m_pMenusClaimTouch->addObject( pNode );
				}
				else
				{
					m_pItemsClaimTouch->addObject( pNode );
				}
			}
	}
	//[Mes] CCLOG("[Mes]>Check&Pass is over");
	return true;
}


bool BYTouchDelegate::passMove( CCNode* pParent, CCTouch *pTouch, CCEvent *pEvent )
{
	if(!m_bIsEnabled || !pParent || !pParent->isVisible() )
	{
		return false;
	}

	//hande message to items
	int iNumChildren = 0;
	CCArray* pChildren = NULL;
	
	//[MesMove] CCLOG("[MesMOVE]>Check the collide rect.");
	//if the item'size > 1, check whether use touches it. Such as TableView.
	//some items doesn't get size. they are medium for maintaining some children. Such as CCTableViewCell.
	if( pParent->getContentSize().width * pParent->getContentSize().height > 1.0f )
	{
		
		CCPoint pt = pTouch->getLocation();
		CCRect rcBoundingBox( 0, 0, pParent->getContentSize().width, pParent->getContentSize().height );
		//do this only for efficiency, because convertToNodeSpace is heavier than nodeToWorldTransform().
		rcBoundingBox = CCRectApplyAffineTransform( rcBoundingBox, pParent->nodeToWorldTransform() );
		//[MesMove] CCLOG("[MesMOVE]Check here:%f,%f in %f,%f,%f,%f",pt.x,pt.y,rcBoundingBox.origin.x,rcBoundingBox.origin.y,rcBoundingBox.size.width,rcBoundingBox.size.height);
		//whether hit the node
		if( !rcBoundingBox.containsPoint( pt ) )
		{
			//[MesMove] CCLOG("[MesMOVE]>Check the collide rect:failed;");
			return false;
		}
	}
	//[MesMove] CCLOG("[MesMOVE]>Check the collide rect:success;");
	pChildren = pParent->getChildren();

	//no children, but user touch this item, so return true.
	if( !pChildren )
	{
		return true;
	}
	iNumChildren = pChildren->count();
	//[MesMove] CCLOG("[MesMOVE]>Preparing passing to children:--%d",iNumChildren);
	//sort all children in ascending order.
	pParent->sortAllChildren();

	CCObject* pObject = NULL;
	//traverse in descending order.
	//we only send message to the first child handling message. 
	CCARRAY_FOREACH_REVERSE( pChildren, pObject )
	{
		//if the item claims the touch message
		//bool bClaim = false;
		////[Mes] CCLOG("[Mes]><><><><>%d",iChildIndex);

		MouseMenu* pLayer = NULL;
		CCNode* pNode = NULL;

		pNode = ( CCNode*  )pObject;
		assert( pNode );
		
		//if it's layer, we should invoke its ccTouchBegan()
		//Make sure that you have commented the CCAssertion statement in CCLayer::ccTouchBegan().
		//TODO:根据设计添加相应的事件流
		if( ( pLayer = dynamic_cast< MouseMenu* >( pNode ) ) )
		{
			//bClaim = 
			pLayer->ccTouchMoved( pTouch, pEvent );
			//continue;
			//return true;			//直接返回提高效率
		}

		CCLayer* pLay = NULL;
		if( ( pLay = dynamic_cast< CCLayer* >( pNode ) ) ){
			pLay->ccTouchMoved(pTouch,pEvent);
		}

		BYTouchDelegate* ptd = NULL;
		//pass message for its child if it doesn't derive BYTouchDelegate. Because child deriving BYTouchDelegate has passed message via ccTouchBegan().
		if( !dynamic_cast< BYTouchDelegate* >( pNode ) )
		{
			//items who doesn't derive from BYTouchDelegate can't pass touch message to its children,
			//so we have to help them to pass touch message.
			passMove( pNode, pTouch, pEvent );
		}


	}

	return true;
}
