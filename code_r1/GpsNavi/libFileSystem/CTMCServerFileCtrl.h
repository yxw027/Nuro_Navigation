#include "FileMapBaseZ.h"
#ifndef DEFINE_TMCSERVERFILECTRL
	#define DEFINE_TMCSERVERFILECTRL

    class CTMCServerFileCtrl : public CFileMapBaseZ
	{
		public:
			CTMCServerFileCtrl();
			virtual ~CTMCServerFileCtrl();
			nuBOOL ReadTMCServerFile(nuTCHAR *tsFilePath);
			nuINT	iServerTMCFileSize;//紀錄下載下來的檔案大小
			nuBYTE	*pTMCServerData;//用來儲存讀取出來的資料buffer
            nuBOOL ReleaseServerFileStru();
		private:
			nuBOOL bReleaseServerFileOK;
	};


#endif
