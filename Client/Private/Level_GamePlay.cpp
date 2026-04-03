#include "Level_GamePlay.h"
#include "GameInstance.h"

#include "Level_Loading.h"
#include "FreeCamera.h"
#include "ImGui_Manager.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()	
{
	ShowCursor(true);

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_GaraGui()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	

	//m_pGameInstance->Load_Date(TEXT("../Bin/DataFiles/Test_Save.json"));
	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	/*if (GetKeyState(VK_SPACE) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::DUNGEON))))
			return;
	}*/
	
	CImGui_Manager::GetInstance()->Update_Engine();

}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("현재 게임플레이 구동 중"));
#endif

	CImGui_Manager::GetInstance()->Render();

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CFreeCamera::FREECAMERA_DESC		CameraDesc{};

	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fDegreePerSec = 180.f;
	CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.f) ;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fMouseSensor = 0.05f;
	CameraDesc.name = TEXT("Camera");
	CameraDesc.m_sPrototype = "Prototype_GameObject_FreeCamera";

	if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_FreeCamera"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag, &CameraDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_GaraGui()
{
	if (FAILED(CImGui_Manager::GetInstance()->Initialize_Manager(m_pDevice, m_pContext)))
		return E_FAIL;

	m_pImGui_Manager = CImGui_Manager::GetInstance();

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC desc{};
	desc.name = TEXT("Terrain");
	desc.pos = _float3(0,0,0);
	desc.m_sPrototype = "Prototype_GameObject_Terrain";
	
	if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag, &desc)))
		return E_FAIL;


	/*if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sky"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC desc{};
	desc.m_sPrototype = "Prototype_GameObject_Skeleton";
	desc.name = TEXT("Skeleton");

	if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Skeleton"),
		ETOI(LEVEL::GAMEPLAY), strLayerTag, &desc)))
		return E_FAIL;

	
	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CLevel_GamePlay::Free()
{
	__super::Free();

	Safe_Release(m_pImGui_Manager);

}
