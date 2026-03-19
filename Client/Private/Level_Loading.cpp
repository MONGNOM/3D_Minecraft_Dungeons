#include "Level_Loading.h"
#include "Loader.h"
#include"BackGround.h"
#include "GameInstance.h"

#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Dungeon.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	/* 로딩 화면을 구성해주기위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	/* 저객체들이 로딩화면을 구성하고 유저에게 보여주는 동안 자원을 로드해줄 수 있는 (Cloader)스레드객체를 생성해준다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	if (true == m_pLoader->isFinished() &&
		GetKeyState(VK_RETURN) & 0x8000)
	{
		CLevel* pNextLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL::LOGO: 
			pNextLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::GAMEPLAY:
			pNextLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::DUNGEON:
			pNextLevel = CLevel_Dungeon::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pNextLevel)
		{
			MSG_BOX("Failed to Created : NextLevel");
			return;
		}
			

		if (FAILED(m_pGameInstance->Change_Level(ETOI(m_eNextLevelID), pNextLevel)))
		{
			MSG_BOX("Failed to Changed : NextLevel");
			return;
		}	
		return;
	}	
}

HRESULT CLevel_Loading::Render()
{
#ifdef _DEBUG
	m_pLoader->Print_LoadingText();
#endif

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_UI(const _tchar* pLayerTag)
{
	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
