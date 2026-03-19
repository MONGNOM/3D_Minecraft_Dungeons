#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 로딩을 위한 스레드를 생성한다. */
/* 지정된 레벨을 위한 자원을 로딩한다. (스레드가 한다) */

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Loading();
	_bool isFinished() {
		return m_isFinished;
	}
#ifdef _DEBUG
public:
	HRESULT Print_LoadingText();
#endif

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	HANDLE						m_hThread = {};
	CRITICAL_SECTION			m_CriticalSection = {};
	LEVEL						m_eNextLevelID = { LEVEL::END };

	_tchar						m_szLoadingText[MAX_PATH] = {};
	_bool						m_isFinished = { false };
	CGameInstance*				m_pGameInstance = { nullptr };

private:
	HRESULT Loading_For_LogoLevel();
	HRESULT Loading_For_GamePlayLevel();
	HRESULT Loading_For_Dungeon();

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;

};

NS_END