#ifndef __BUFFER_LIST__
#define __BUFFER_LIST__

/*
	<BufferList
	<	�е�Buffer��Ч�ű���Ԥ�ȶ�ȡ
*/

#include <map>

class BufferList
{
public:
	static BufferList *sharedBufferList();
	static void purgeSharedBufferList();

	const char* getScp(int ai);

private:
	static BufferList* mSharedBufferList;
	std::map<int, std::string> msiBufferScps;

	bool init();
	void LoadFromDb();

};

#endif