/*
*	StateCenter
*	对状态进行存储和读取的类，负责状态的维护和读档取档
*	-StateLock  --剧情锁的控制
*	-InitState  --地图和立绘的途中载入控制，使用tag储存，与初始化脚本协同工作
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

	string m_tpsz,m_tscr;			//TileMap路径和脚本路径|不支持对战斗地图的存档。
	CCDictionary* il;				//存储Map单位状态的字典 -- itemlist.

	CCDictionary* m_cdItemList;			//物品列表
	Json::Value		gLoadedJson;	/* <缓存 */

public:
	int m_iState,m_oldstate;
	string m_sName, m_sBgi;
	float	mfBgiX,mfBgiY;	/* <背景图片的位置 */
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

	/* <载入物品 */
	void f_load_item(Script* ts);
	void f_add_item(Script* ts, bool bSilent = false);				//丢弃和使用物品没有加入统一的接口里面来 = =，因为每次只会针对一个物品进行操作所以就由操作者自行处理了。
	string m_sShowMask;
	void f_add_item_show();											//弹出窗口，测试中。必须紧接着f_add_item使用以防止不可预知的bug。
	void f_insert_item(int aid,int agroup,ItemCellData* aicd);							// <替换一个物品，目前只有TOChara切换装备时使用

	void f_get_state();

	void bback(CCObject* c);
	CCDictionary* t_ldb_cid;

	void clear();	//Clear current states.&Prepare for new states.
	CCDictionary* f_get_itemlist(int i);

	void	f_LoadOver();	/* <清理被置放的存档标识 */

};

#endif