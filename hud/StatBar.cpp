#include "hud/StatBar.h"



StatBar::StatBar()
{
	m_ec = NULL;
}


void StatBar::SetContent( EChesses* aec )
{
	//CCLog(">[StatBar] SetContent.");
	if(!aec){
		m_ec = aec;
		setVisible(false);
		return;
	}
	//CCLog(">[StatBar] SetContent. | None Null Pass.");

	if(m_ec == aec){
		setVisible(true);
		return;
	}
	//CCLog(">[StatBar] SetContent. | New Stat Calculate.");

	m_ec = aec;
	CCLog(">[StatBar] setContent | Name: %s", m_ec->m_pChara->m_sName.c_str());

	msb_SoulBall->setSum(m_ec->m_pChara->getSHP());
	mb_BloodOne->revalue(m_ec->m_pChara->getvalue("hp"));
	setHead(m_ec->m_pChara->msBHead.c_str());

	RefreshAll();
	SetNullAct();
	setVisible(true);
}

void StatBar::RefreshAll()
{
	CCLog(">[StatBar] ReereshAll() ");
	if(!m_ec) return;
	Chara* t_owner = m_ec->m_pChara;

	CCLog(">[StatBar] -- RefreshAll() --");
	CCLog(">[StatBar] Name: %s", t_owner->m_sName.c_str());
	mb_Exp->setval(t_owner->getvalue("exp")%100);
	CCLog(">[RefreshAll] hit: %d",t_owner->getvalue("hit")%100);
	
	msb_SoulBall->setSum(t_owner->getvalue("b_maxhunpo"));
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

void StatBar::setHead(const char* asH)
{
	CCLog(">[StatBar] setHead(%s) ", asH);
}

CCObject* StatBar::getContent()
{
	return m_ec;
}
