#pragma once

#include "Base.h"


NS_BEGIN(Engine)

class CDataManager : public CBase
{
private:
	CDataManager();
	virtual ~CDataManager() = default;

public:
	HRESULT Save_Date();
	HRESULT Load_Date(const _tchar* filePath);
	

public:
	static CDataManager* Create();
	virtual void Free() override;
};

NS_END
