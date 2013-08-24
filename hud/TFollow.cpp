#include "hud/TFollow.h"

TFollow* TFollow::create( CCNode *pFollowedNode, const CCRect& rect/* = CCRectZero*/ )
{
	TFollow *pRet = new TFollow();
	if (pRet && pRet->initWithTarget(pFollowedNode, rect))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

void TFollow::step( float dt )
{
	CC_UNUSED_PARAM(dt);
	CCPoint tmpposition;
	if(m_bBoundarySet)
	{
		// whole map fits inside a single screen, no need to modify the position - unless map boundaries are increased
		if(m_bBoundaryFullyCovered)
			return;

		CCPoint tempPos = ccpSub( m_obHalfScreenSize, m_pobFollowedNode->getPosition());

		tmpposition = ccp(clampf(tempPos.x, m_fLeftBoundary, m_fRightBoundary), 
			clampf(tempPos.y, m_fBottomBoundary, m_fTopBoundary));
		//m_pTarget->setPosition();
	}
	else
	{
		tmpposition = ccpSub(m_obHalfScreenSize, m_pobFollowedNode->getPosition());
	}
	CCPoint moveVect;

	CCPoint oldPos = m_pTarget->getPosition();
	double dist = ccpDistance(tmpposition, oldPos);
	if (dist > 1){
		moveVect = ccpMult(ccpSub(tmpposition,oldPos),0.05); //0.05 is the smooth constant.
		oldPos = ccpAdd(oldPos, moveVect);
		m_pTarget->setPosition(oldPos);
	}
}
