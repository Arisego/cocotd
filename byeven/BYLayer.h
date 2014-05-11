//
//  BYLayer.h
//  TableTest
//
//  Created by jason on 12-12-21.
//
//

#ifndef __TableTest__BYLayer__
#define __TableTest__BYLayer__

#include "cocos2d.h"
#include "BYTouchDelegate.h"
#include "BYMacros.h"

USING_NS_CC;

//Users shouldn derive from BYLayerAncestor, BYLayerDescendant, BYLayerModal instead BYLayer.
//BYLayer can't be touched.
//BYLayer pass message to it's all descendant of CCLayer
class BYLayer : public CCLayer, public BYTouchDelegate
{
    //static
public:
    CREATE_FUNC( BYLayer );
	
    
protected:
    BY_INIT_DEFAULT( CCLayer );
    
    BY_MESSAGE_CONSTRUCTOR( BYLayer );
    BY_MESSAGE_BRIDGE();
    
    virtual void onEnter()
    {
        setChildrenTouchDisable( this );
        CCLayer::onEnter();
    }
    
    void setChildrenTouchDisable( CCNode* pParent )
    {
        if( !pParent )
        {
            return;
        }
        
        CCArray* pChildren = pParent->getChildren();
        if( !pChildren )
        {
            return;
        }
    
        int iNumChildren = pChildren->count();
        for( int i = 0; i < iNumChildren; ++i )
        {
            CCLayer* pLayer = NULL;
            CCNode* pChild = ( CCNode* )pChildren->objectAtIndex( i );
            if ( ( pLayer = dynamic_cast< CCLayer* >( pChild ) ) )
            {
                pLayer->setTouchEnabled( false );
            }
            
            setChildrenTouchDisable( pChild );
        }
    }
};

//BYLayerAncestor can be touched.
//all secene should have only one BYLayerAncestor for bottom layer.
//all the other layer should be BYLayerDescendant.
class BYLayerAncestor : public BYLayer
{
protected:
    bool virtual init()
    {
        if( !BYLayer::init() )
        {
            return false;
        }
        
        setTouchEnabled( true );
        return true;
    }
    BY_TOUCH_REGISTER_DEFAULT( 0 );
    //static
public:
    CREATE_FUNC( BYLayerAncestor );
    //data
};

class BYLayerDescendant : public BYLayer
{
protected:
    BY_INIT_DEFAULT( BYLayer );
    //static
public:
    CREATE_FUNC( BYLayerDescendant );
    //data

};

//BYLayerModal stopes touch messages from being passed to other layers which is not it's children.
//And will pass touch messages to it's children normally.
class BYLayerModal : public BYLayerAncestor
{
protected:
    BY_INIT_DEFAULT( BYLayerAncestor );
    BY_TOUCH_REGISTER_DEFAULT( kCCMenuHandlerPriority );
    //static
public:
    CREATE_FUNC( BYLayerModal );
    
    //data
};


#endif /* defined(__TableTest__BYLayer__) */
