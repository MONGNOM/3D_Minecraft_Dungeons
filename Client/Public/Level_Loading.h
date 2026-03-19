#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* 1. 로딩 장면을 구성해주기위한 객체를 생성해주고 업데이트와 렌더의 반복적이 ㄴ수행. */
/* 2. 로딩 다음에 등장할 레벨을 위한 자원을 로드해준다.(CLoader) == 로더를 생성해준다. */

NS_BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVEL eNextLevelID);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LEVEL			m_eNextLevelID = { LEVEL::END };
	class CLoader* m_pLoader = { nullptr };

private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

NS_END