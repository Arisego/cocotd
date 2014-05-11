#ifndef __ECHESS_COMP_H__
#define __ECHESS_COMP_H__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "utils/Scriptor.h"

USING_NS_CC;
USING_NS_CC_EXT;

class EChessComp : public CCComponent 
{
public:
	EChessComp();
	~EChessComp();

	/* <����ָ���������������н����ƶ� */
	void		move_by_path(std::vector<CCPoint> &vpath);
	virtual void 
				update(float delta);

	/* <����ָ���Ľű� */
	void		RunScript(Script* asp);		// <��Ҫ��ͬһ������������ֽű�Ч��������ǲ���Ԥ֪�ġ����Ҫ��ô������������������ﴴ���µ�������Component��
	void		setScriptNum(int ai);
	void		DerScript(Script* asp);

	/* <�ڲ��� */
	int			miELock;
	void		ELock();
	void		EUnLock();

	/* <Ѱ�Һ��ʵ�RE���� */
	bool		FindFitRe(CCObject* tar,int atime);
	void		CleanRe();
	int			PreCheckRe(CCObject* tar,int atime);

	void		DelayUnLock(float dt); // <�ӳ�Ȼ���ͷų��е�����������Ϸѭ����
	int			HaveAI();				// <�Ƿ���AI

	int			miReCount; // <������������
private:
	int			miScriptSum;
	int			miScriptCount;
	void		GoAHead();
	Script*			mSp;

public:
	/* <��Χ�ⶨ */
	bool		TestRange(CCObject* tar, bool acat = false);
private:
	map<string,string> 
				t_ssm;
	
	int			miStateFlag;
	std::vector<std::pair<float,float>> 
				mPath;

	void		ChessDead();
	bool		mbChessDead;

public:
	int			miReState;		// <������־ [0-��û�� | 1-�ڷ��� | 2-��������]

	void		onChessDead();
	void		onAddExp();

	void		CheckAlive();	// <�����������������־������ɱ
	void		DelayPlay();	// <�ӳٲ�������

	bool		mbDebugS;		// <�Ƿ���ʾ������Ϣ

	bool		mbThinked;	// <�Ƿ񵯳����߲��ȴ�
private:
	CCArray*	mcaMemories;	// <����ECC�����еĽű�״̬
	CCObject*	mcesCurrent;	// <��ǰ״ָ̬�� | ECCState

	void		PopState();		// <���Լ���Ƿ��б����State
	string		slatst;

public:
	bool		AtkTestRange(CCPoint target);	// <Ԥ��ʱʹ�ã�����Ԥ���ã����޸�cs_y��
	int			getCurGrp();					// <��õ�ǰ��HitGroup
	void		setSrc(CCObject* teSrc);		// <���ýű��˺���Դ
	CCObject*	getSrc();						// <����������
};


#endif