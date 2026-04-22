#include "Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "BackGround.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()	
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_F1) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
			return;
	}


	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::DUNGEON))))
			return;
	}

}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("현재 로고레벨 구동 중"));
#endif

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//CBackGround::BACKGROUND_DESC		BackGroundDesc{};

	//BackGroundDesc.iFlag = 0;
	//BackGroundDesc.fSpeedPerSec = 1.f;
	//BackGroundDesc.fRotationPerSec = 1.f;
	
	CBackGround::BACKGROUND_DESC BackGroundDesc{};

	BackGroundDesc.currentLevel = LEVEL::LOGO;
	BackGroundDesc.NumTexture = 0;
	BackGroundDesc.Scenetype = CGameObject::SCENETYPE::LOGO;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::LOGO), TEXT("Prototype_GameObject_Loading"),
		ETOI(LEVEL::LOGO), strLayerTag, &BackGroundDesc)))
		return E_FAIL;


	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CLevel_Logo::Free()
{
	__super::Free();

}
