//
//  BYMacros.h
//  TableTest
//
//  Created by jason on 12-12-26.
//
//

#ifndef TableTest_BYMacros_h
#define TableTest_BYMacros_h


#define BY_ASSERT_POINTER( x ) assert( ( x ) != NULL )

#define BY_DEFAULT_CHARSET "Arial"

#define BY_DOUBLE_CLICK_DELTA 0.4f

//anchor point
#define BY_ANCHOR_POINT_CENTER ccp( 0.5f, 0.5f )
#define BY_ANCHOR_POINT_LEFTBOTTOM ccp( 0.0f, 0.0f )
#define BY_ANCHOR_POINT_RIGHTBOTTOM ccp( 1.0f, 0.0f )
#define BY_ANCHOR_POINT_RIGHTTOP ccp( 1.0f, 1.0f )
#define BY_ANCHOR_POINT_LEFTTOP ccp( 0.0f, 1.0f )

//size
#define BY_IPAD_SIZE_WIDTH 1024
#define BY_IPAD_SIZE_HEIGHT 768

#define BY_IPAD_SIZE_WIDTH_HALF 512
#define BY_IPAD_SIZE_HEIGHT_HALF 384

#define BY_WIN_SIZE_WIDTH ( CCDirector::sharedDirector()->getWinSize().width )
#define BY_WIN_SIZE_HEIGHT ( CCDirector::sharedDirector()->getWinSize().height )

#define BY_WIN_SIZE_WIDTH_HALF ( CCDirector::sharedDirector()->getWinSize().width * 0.5f )
#define BY_WIN_SIZE_HEIGHT_HALF ( CCDirector::sharedDirector()->getWinSize().height * 0.5f )

#define BY_WIN_CENTER_POINT ccp( BY_WIN_SIZE_WIDTH_HALF, BY_WIN_SIZE_HEIGHT_HALF )

#define BY_NODE_WIDTH( n ) ( ( n )->getContentSize().width )
#define BY_NODE_HEIGHT( n ) ( ( n )->getContentSize().height )

#define BY_NODE_WIDTH_HALF( n ) ( ( n )->getContentSize().width * 0.5f )
#define BY_NODE_HEIGHT_HALF( n ) ( ( n )->getContentSize().height * 0.5f )

//menu items

#define BY_MENU_ITEM_COMMON_SPACING 20

//functions
#define BY_GET_DIRECTOR() cocos2d::CCDirector::sharedDirector()
#define BY_GET_BROTHER( brotherClassName, brotherTag ) ( ( brotherClassName* )getParent()->getChildByTag( ( brotherTag ) ) )
#define BY_GET_CHILD( childClassName, childTag ) ( ( childClassName* )getChildByTag( ( childTag ) ) )

#define BY_MESSAGE_CONSTRUCTOR( ownerClassName ) \
ownerClassName() : BYTouchDelegate( this ){}


#define BY_MESSAGE_BRIDGE()                                             \
virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)             \
{                                                                       \
return byTouchBegan( pTouch, pEvent );                                  \
}                                                                       \
virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)             \
{                                                                       \
byTouchMoved( pTouch, pEvent );                                         \
}                                                                       \
virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)             \
{                                                                       \
byTouchEnded( pTouch, pEvent );                                         \
}                                                                       \
virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)         \
{                                                                       \
byTouchCancelled( pTouch, pEvent );                                     \
}


#define BY_INIT_DEFAULT( parentClassName )              \
virtual bool init()                                     \
{                                                       \
if( !parentClassName::init() )                          \
{                                                       \
return false;                                           \
}                                                       \
                                                        \
return true;                                            \
}

#define BY_TOUCH_REGISTER_DEFAULT( iPriority )          \
virtual void registerWithTouchDispatcher( void  )       \
{                                                       \
CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate( this, iPriority, true );                            \
}

#define BY_TOUCH_REGISTER_EMPTY()                       \
virtual void registerWithTouchDispatcher( void  ){}

#endif
