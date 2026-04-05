#pragma once

#include "Base.h"


NS_BEGIN(Engine)

class CDataManager : public CBase
{
private:
	CDataManager();
	virtual ~CDataManager() = default;

public:
	HRESULT Save_Date(const vector<tagObjectInfo>& objectinfo);
	HRESULT Load_Date(const _tchar* filePath, vector<tagObjectInfo>& objectinfo);
	

public:
	static CDataManager* Create();
	virtual void Free() override;
};

NS_END
