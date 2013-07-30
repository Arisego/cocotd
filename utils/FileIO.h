#ifndef __FILEIO_H__
#define __FILEIO_H__


/** 
读取zip文件用输入输出，只在windows中使用 << 已使用windows独有api
读取第一层。
*/
#include "tchar.h"

#include "unzip.h"

#include "cocos2d.h"
USING_NS_CC;

class FileIO {

private:
	HZIP hz;
	ZIPENTRY ze;
	int numitems;
	const char* pass;

public:
	FileIO(const char *filename,const char* pw = "1"){
	
		hz = NULL;
		unsigned long filesize;
		pass = pw;

		std::string path;
		do 
		{
			path.clear();
			path= CCFileUtils::sharedFileUtils()->fullPathForFilename(filename);
		} while (path.length() < 1);
		
		char *buffer = (char *)CCFileUtils::sharedFileUtils()->getFileData(path.c_str(), "rb", &filesize);

		hz = OpenZip(buffer, filesize,pass);
		if(!hz) exit(1);

		GetZipItem(hz,-1,&ze);
		numitems = ze.index;
	}

	unsigned char* getFileData(const char* pszFileName, unsigned long* pSize){
		int cur=-1;
		ZIPENTRY ce;

		const WCHAR *pwcsName;
		int nChars = MultiByteToWideChar(CP_ACP, 0, pszFileName, -1, NULL, 0);
		pwcsName = new WCHAR[nChars];
		MultiByteToWideChar(CP_ACP, 0, pszFileName, -1, (LPWSTR)pwcsName, nChars);

		FindZipItem(hz, pwcsName, true, &cur, &ce);
		delete pwcsName;
		if(cur>-1)
		{
			*pSize = ce.unc_size;
			unsigned char *buf = new unsigned char[*pSize];			
			UnzipItem(hz,cur, buf,ce.unc_size);		//如果需要进一步加密,在这里对buffer进行进一步处理，。
			return buf;
		}else{
			return NULL;
		}
	
	}

	~FileIO(){
		CloseZip(hz);	
	}


	

};

#endif

