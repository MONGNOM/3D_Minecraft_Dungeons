#include "Level_Loading.h"
#include "Loader.h"
#include"BackGround.h"
#include "GameInstance.h"

#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Dungeon.h"
#include "Level_Boss.h"
#include "Level_BossPath.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;
	switch (m_eNextLevelID)
	{
	case LEVEL::LOGO:
		m_iNumLoading = 0;
		break;
	case LEVEL::GAMEPLAY:
		m_iNumLoading = 0;
		break;
	case LEVEL::DUNGEON:
		m_iNumLoading = 1;
		break;
	case LEVEL::BOSS:
		m_iNumLoading = 3;
		break;
	case LEVEL::BOSSPATH:
		m_iNumLoading = 2;
		break;
	}


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
		case LEVEL::BOSS:
			pNextLevel = CLevel_Boss::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::BOSSPATH:
			pNextLevel = CLevel_BossPath::Create(m_pDevice, m_pContext);
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

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::LOADING), TEXT("Prototype_Component_Texture_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/mincraft/Title/TitleImage_%d.png"), 4))))
	{
		MSG_BOX("Fail to Add_Prototype : Title Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::LOADING), TEXT("Prototype_GameObject_Loading"),
		CBackGround::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Loading");
		return E_FAIL;
	}


	CBackGround::BACKGROUND_DESC BackGroundDesc{};

	BackGroundDesc.currentLevel = LEVEL::LOADING;
	BackGroundDesc.NumTexture = m_iNumLoading;
	BackGroundDesc.Scenetype = CGameObject::SCENETYPE::LOADING;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::LOADING), TEXT("Prototype_GameObject_Loading"),
		ETOI(LEVEL::LOADING), pLayerTag, &BackGroundDesc)))
		return E_FAIL;

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
