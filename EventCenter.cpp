#include "EventCenter.h"


#include "GameManager.h"


EventCenter* EventCenter::mSharedEventCenter = NULL;
Controller* EventCenter::mController = NULL;		//关心事件 w s a d z || 地图基础控制
Scroller* EventCenter::mScroller = NULL;			//关心事件 鼠标滚轮	 || 
Selector* EventCenter::mSelector = NULL;			//关心事件 q e c	 ||	切换选择器

Cake* EventCenter::m_bm_cake = NULL;				//右键

CharaTab* EventCenter::mCharaTab = NULL;			// <角色面板


EventCenter::~EventCenter(){
	CCEGLView* eglView = CCEGLView::sharedOpenGLView();
	eglView->setAccelerometerKeyHook(NULL);

}

EventCenter* EventCenter::sharedEventCenter(){
	if (mSharedEventCenter == NULL)
	{
		mSharedEventCenter = new EventCenter();
		if (!mSharedEventCenter || !mSharedEventCenter->init())
		{
			CC_SAFE_DELETE(mSharedEventCenter);
		}

	}

	return mSharedEventCenter;

}

void EventCenter::ehandler( UINT message,WPARAM wParam, LPARAM lParam ){
	switch(message){
	case WM_RBUTTONDOWN:
		handlerightm(wParam,lParam);
		break;
	case WM_CLOSE:
		GameManager::sharedGameManager()->preQuit();
		break;
	case WM_KEYDOWN:
		handlekeydown(wParam,lParam);
		break;
	case WM_KEYUP:
		handlekeyup(wParam,lParam);
		break;
	case WM_MOUSEWHEEL:
		if(mScroller) mScroller->scroll_in(wParam,lParam);
		break;
	default:
		break;
	}
}

void EventCenter::handlekeydown(WPARAM wParam, LPARAM lParam){
	
	if( !(lParam & 0x40000000) ){
		switch(wParam){
		case 73:
			if(mCharaTab) {mCharaTab->i_press();return;}
			break;
		case 75:
			if(mCharaTab) {mCharaTab->k_press();return;}
			break;
		case 74:
			if(mCharaTab) {mCharaTab->j_press();return;}
			break;
		case 76:
			if(mCharaTab) {mCharaTab->l_press();return;}
			break;
		case 87:
			if(mCharaTab){mCharaTab->w_press();return;}
			if(!mController) return;
			mController->w_press();
			break;						 
		case 83:
			if(mCharaTab) {mCharaTab->s_press();return;}
			if(!mController) return;
			mController->s_press();
			break;
		case 88:
			if(mCharaTab) {mCharaTab->x_press();return;}
			break;
		case 90:
			if(mCharaTab) {mCharaTab->z_press();return;}
			break;
		case 65:
			if(mCharaTab) {mCharaTab->a_press();return;}
			if(!mController) return;
			mController->a_press();
			break;
		case 68:
			if(mCharaTab) {mCharaTab->d_press();return;}
			if(!mController) return;
			mController->d_press();
			break;
		case 81:
			if(!mSelector) return;
			mSelector->q_press();
			break;
		case 69:
			if(!mSelector) return;
			mSelector->e_press();
			break;
		case 66:
			if(!mController) return;
			mController->b_click();
			break;
		case 67:
			if(!mSelector) return;
			mSelector->c_press();
			break;
		case 27:
			GameManager::sharedGameManager()->goConfig();	
			break;
		default:
			break;
		}
	}
}

void EventCenter::handlekeyup(WPARAM wParam, LPARAM lParam){
	if(!mController) return;
	switch(wParam){
	case 87:
		mController->w_press(true);
		break;						 
	case 83:
		mController->s_press(true);
		break;
	case 65:
		mController->a_press(true);
		break;
	case 68:
		mController->d_press(true);
		break;
	case 90:
		mController->z_press(true);
		break;
	default:
		break;
	}
}

void EventCenter::handlerightm(WPARAM wParam, LPARAM lParam){
	if(m_bm_cake)
		m_bm_cake->right_click();
}

bool EventCenter::init(){
	do 
	{
		CCEGLView* eglView = CCEGLView::sharedOpenGLView();
		eglView->setAccelerometerKeyHook(ehandler);

		m_vScrolls.clear();
		

		return true;
	} while (0);
	return false;
}

void EventCenter::setController(Controller* c){
	mController = c;
}

void EventCenter::setScroller(Scroller* s /* = NULL */){
	if(s){
		m_vScrolls.push_back(mScroller);
		mScroller = s;
	}else{
		int vs = m_vScrolls.size();
		if(vs>1) {
			mScroller = m_vScrolls.at(m_vScrolls.size()-1);
			m_vScrolls.pop_back();
		}
	}

	
}

void EventCenter::setSelector( Selector* s /*= NULL*/ )
{
	mSelector = s;
}

void EventCenter::setBmCake( Cake* s /*= NULL*/ )
{
	m_bm_cake = s;
}

void EventCenter::purgeSharedEventCenter()
{
	CC_SAFE_DELETE(mSharedEventCenter);
}

void EventCenter::SetCharaTab( CharaTab* s /*= NULL*/ )
{
	mCharaTab = s;
}
