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
	const PLAYER_DATA&  Get_PlayerData() const;
	void Set_PlayerData(const PLAYER_DATA& data);
	PLAYER_DATA m_sPlayerdata;

	

public:
	static CDataManager* Create();
	virtual void Free() override;
};

NS_END
