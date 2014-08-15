/*
*	StateCenter
*	��״̬���д洢�Ͷ�ȡ���࣬����״̬��ά���Ͷ���ȡ��
*	-StateLock  --�������Ŀ���
*	-InitState  --��ͼ�������;��������ƣ�ʹ��tag���棬���ʼ���ű�Эͬ����
*/
#ifndef __STATE_CENTER_H__
#define __STATE_CENTER_H__

#include <map>
#include "utils/Scriptor.h"
#include "packui/CharaS.h"
#include "Macros.h"
#include "json/json.h"

using namespace std;



class StateCenter : public CCObject
{
private:
	static StateCenter* mSharedStateCenter;
	CharaS* g_charas;

	static map<string,int> g_lock;

	string m_tpsz,m_tscr;			//TileMap·���ͽű�·��|��֧�ֶ�ս����ͼ�Ĵ浵��
	CCDictionary* il;				//�洢Map��λ״̬���ֵ� -- itemlist.

	CCDictionary* m_cdItemList;			//��Ʒ�б�
	Json::Value		gLoadedJson;	/* <���� */

public:
	int m_iState,m_oldstate;
	string m_sName, m_sBgi;
	float	mfBgiX,mfBgiY;	/* <����ͼƬ��λ�� */
	int m_iJump, m_iTJump;

	std::map<string,int> TagMap;
	std::map<string,string> PathMap;

	static StateCenter *sharedStateCenter();
	static void purgeSharedStateCenter();
	~StateCenter();

	bool init();
	bool m_bIsLoad;
	Scriptor* g_sp;
	bool g_load_file();

	void g_lock_plus(const char* name, int value);
	void g_lock_set(const char* name, int value);
	void g_lock_change(Script* ts);
	void g_lock_check(Script* ts);
	bool b_lock_check(Script* ts);
	bool m_lock_check(Script* ts);
	int g_lock_query(string name);

	bool f_save_file(const char* psz);
	bool f_load_file(const char* psz);
	void f_load_lock(Script* ts);
	void f_load_map(Script* ts);
	
	Script* m_scTags;

	/* <������Ʒ */
	void f_load_item(Script* ts);
	void f_add_item(Script* ts, bool bSilent = false);				//������ʹ����Ʒû�м���ͳһ�Ľӿ������� = =����Ϊÿ��ֻ�����һ����Ʒ���в������Ծ��ɲ��������д����ˡ�
	string m_sShowMask;
	void f_add_item_show();											//�������ڣ������С����������f_add_itemʹ���Է�ֹ����Ԥ֪��bug��
	void f_insert_item(int aid,int agroup,ItemCellData* aicd);							// <�滻һ����Ʒ��Ŀǰֻ��TOChara�л�װ��ʱʹ��

	void f_get_state();

	void bback(CCObject* c);
	CCDictionary* t_ldb_cid;

	void clear();	//Clear current states.&Prepare for new states.
	CCDictionary* f_get_itemlist(int i);

	void	f_LoadOver();	/* <�����÷ŵĴ浵��ʶ */

};

#endif