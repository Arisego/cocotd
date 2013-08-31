#include "hud/StatBar.h"

void StatBar::SetContent( EChesses* aec )
{
	if(m_ec == aec){
		setVisible(true);
		return;
	}
	m_ec = aec;

	msb_SoulBall->setSum(m_ec->m_pChara->getSHP());

	mb_BloodOne->revalue(m_ec->m_pChara->getvalue("hp"));

	RefreshAll();
	SetNullAct();
	//setVisible(true);
}

void StatBar::RefreshAll()
{
	Chara* t_owner = m_ec->m_pChara;
	mb_Exp->setval(t_owner->getvalue("exp")%100);
	CCLog(">< %d",t_owner->getvalue("hit")%100);
	msb_SoulBall->setAli(t_owner->getvalue("b_hunpo"));

	ml_Ctn->setString(CCString::createWithFormat("%d/%d",t_owner->getvalue("b_xudong"),t_owner->getvalue("b_maxxudong"))->getCString());
	ml_Par->setString(CCString::createWithFormat("%d/%d",t_owner->getvalue("b_gedang"),t_owner->getvalue("b_maxgedang"))->getCString());

	ml_Hp->setString(CCString::createWithFormat("%d/%d",t_owner->gethp(),t_owner->getvalue("hp"))->getCString());

	mb_BloodOne->setval(t_owner->gethp());

}

void StatBar::RefreshAct( int a1, int a2, int a3 )
{
	ml_Hit->setString(CCString::createWithFormat("%d",a1)->getCString());
	ml_Crt->setString(CCString::createWithFormat("%d",a2)->getCString());
	ml_Dmg->setString(CCString::createWithFormat("%d",a3)->getCString());
}

void StatBar::SetNullAct(){
	ml_Hit->setString("--");
	ml_Crt->setString("--");
	ml_Dmg->setString("--");
}