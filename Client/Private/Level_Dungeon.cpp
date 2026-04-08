#include "Level_Dungeon.h"
#include "GameInstance.h"
#include "FreeCamera.h"
#include "HotBar.h"
#include "ImGui_Manager.h"

CLevel_Dungeon::CLevel_Dungeon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) 
	: CLevel {pDevice, pContext}
{
}

std::string WStringToString1(const std::wstring& wstr)
{
	if (wstr.empty()) return std::string();

	// 1. 변환에 필요한 공간(바이트 수) 계산
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);

	// 2. 공간 확보 후 실제 변환 진행
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

	return strTo;
}

std::wstring StringToWString1(const std::string& str)
{
	if (str.empty()) return std::wstring();

	// 1. 변환 후 글자 길이가 얼마나 될지 미리 계산 (이번엔 MultiByteToWideChar 사용)
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);

	// 2. 계산된 길이만큼 빈 wstring을 만듭니다.
	std::wstring wstrTo(size_needed, 0);

	// 3. 진짜로 변환해서 집어넣습니다.
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);

	return wstrTo;
}

HRESULT CLevel_Dungeon::Initialize()
{
	ShowCursor(FALSE);

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	/*if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;*/

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Load()))
		return E_FAIL;
	

	return S_OK;
}

void CLevel_Dungeon::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Dungeon::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("마인크래프트 던전"));
#endif 
	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDirection = _float4(-1.0f, -2.0f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CFreeCamera::FREECAMERA_DESC cameraDesc{};

	cameraDesc.fSpeedPerSec = 10.f;
	cameraDesc.fDegreePerSec = 180.f;
	cameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
	cameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	cameraDesc.fFovy = XMConvertToRadians(30.f);
	cameraDesc.fNear = 0.1f;
	cameraDesc.fFar = 500.f;
	cameraDesc.fMouseSensor = 0.05f;
	cameraDesc.Scenetype = CGameObject::SCENETYPE::DUNGEON;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_FreeCamera"),
		ETOI(LEVEL::DUNGEON), strLayerTag, &cameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Player"),
		ETOI(LEVEL::DUNGEON), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Skeleton"),
		ETOI(LEVEL::DUNGEON), strLayerTag)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Dungeon::Add_HotBar(const _wstring& strLayerTag)
{
	//_float  sizeY = 148 * 0.6f;
	//_float  posY = 0.6f;
	//_float  BacksizeY = 148 * 0.5f;
	//_float  BackposY = 0.3f;

	//CHotBar::HOTBAR_DESC DescHeart{};
	//DescHeart.fSizeX = 257* 0.7f;
	//DescHeart.fSizeY = 202* 0.7f;
	//DescHeart.fX = g_iWinSizeX * 0.5f;
	//DescHeart.fY = g_iWinSizeY - DescHeart.fSizeY * 0.73f;
	//DescHeart.iNumTexture = 4;

	//if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_HotBar"),
	//	ETOI(LEVEL::DUNGEON), strLayerTag, &DescHeart)))
	//	return E_FAIL;


	//CHotBar::HOTBAR_DESC DescHeartBack{};
	//DescHeartBack.fSizeX = 310 * 0.7f;
	//DescHeartBack.fSizeY = 270 * 0.7f;
	//DescHeartBack.fX = g_iWinSizeX * 0.5f;
	//DescHeartBack.fY = g_iWinSizeY - DescHeartBack.fSizeY * 0.55f;
	//DescHeartBack.iNumTexture = 5;

	//if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_HotBar"),
	//	ETOI(LEVEL::DUNGEON), strLayerTag, &DescHeartBack)))
	//	return E_FAIL;

	//

	//// Left
	//CHotBar::HOTBAR_DESC DescLeft{};

	//DescLeft.fSizeX = 103 * 0.65f;
	//DescLeft.fSizeY = sizeY;
	//DescLeft.fX = g_iWinSizeX * 0.235f;
	//DescLeft.fY = g_iWinSizeY - DescLeft.fSizeY * posY;
	//DescLeft.iNumTexture = 0;

	//if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_HotBar"),
	//	ETOI(LEVEL::DUNGEON), strLayerTag, &DescLeft)))
	//	return E_FAIL;


	//// Middle
	//CHotBar::HOTBAR_DESC DescMiddle{};

	//DescMiddle.fSizeX = g_iWinSizeX / 1.98f;
	//DescMiddle.fSizeY = sizeY;
	//DescMiddle.fX = g_iWinSizeX * 0.5f;
	//DescMiddle.fY = g_iWinSizeY - DescMiddle.fSizeY * posY;
	//DescMiddle.iNumTexture = 1;

	//if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_HotBar"),
	//	ETOI(LEVEL::DUNGEON), strLayerTag, &DescMiddle)))
	//	return E_FAIL;


	//// Right
	//CHotBar::HOTBAR_DESC DescRight{};

	//DescRight.fSizeX = 103 * 0.65f; // 인데 지금 내 화질크기가 좀 다르잖아 비율생각넣어줘야할듯
	//DescRight.fSizeY = sizeY;
	//DescRight.fX = g_iWinSizeX * 0.765f;
	//DescRight.fY = g_iWinSizeY - DescRight.fSizeY * posY;
	//DescRight.iNumTexture = 2;

	//if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_HotBar"),
	//	ETOI(LEVEL::DUNGEON), strLayerTag, &DescRight)))
	//	return E_FAIL;


	//
	//// BackGround
	//CHotBar::HOTBAR_DESC DescBackLeft{};

	//DescBackLeft.fSizeX = g_iWinSizeX * 0.54f;
	//DescBackLeft.fSizeY = BacksizeY;
	//DescBackLeft.fX = g_iWinSizeX * 0.5f;
	//DescBackLeft.fY = g_iWinSizeY - DescBackLeft.fSizeY * BackposY;
	//DescBackLeft.iNumTexture = 3;

	//if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_HotBar"),
	//	ETOI(LEVEL::DUNGEON), strLayerTag, &DescBackLeft)))
	//	return E_FAIL;


	CHotBar::HOTBAR_DESC DescBackLeft{};

	DescBackLeft.fSizeX = 1427.f;
	DescBackLeft.fSizeY = 219.f;
	DescBackLeft.fX = g_iWinSizeX * 0.5f;
	DescBackLeft.fY = g_iWinSizeY - DescBackLeft.fSizeY *0.48f;
	DescBackLeft.iNumTexture = 0;
	DescBackLeft.Scenetype = CGameObject::SCENETYPE::DUNGEON;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_HotBar"),
		ETOI(LEVEL::DUNGEON), strLayerTag, &DescBackLeft)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_UI(const _wstring& strLayerTag)
{
	Add_HotBar(strLayerTag);

	CGameObject::GAMEOBJECT_DESC desc;
	desc.Scenetype = CGameObject::SCENETYPE::DUNGEON;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Sky"),
		ETOI(LEVEL::DUNGEON), strLayerTag, &desc)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CLevel_Dungeon::Ready_Layer_Load()
{



	//m_pImGui_Manager->Set_Load(true);
	vector<OBJECTINFO> objectInfoList;
	m_pGameInstance->Load_Date(TEXT("../Bin/DataFiles/Test_Save.json"), objectInfoList);

	for (auto& object : objectInfoList)
	{
		CGameObject::GAMEOBJECT_DESC Desc{};
		Desc.m_sPrototype = object.PrototypeName;
		Desc.name = StringToWString1(object.Name);
		Desc.pos = object.Translation;
		Desc.rot = object.Rotation;
		Desc.NumTexture = object.data;
		Desc.Scenetype = CGameObject::SCENETYPE::DUNGEON;

		if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::DUNGEON), StringToWString1(object.PrototypeName),
			ETOI(LEVEL::DUNGEON), TEXT("Load_Layer"), &Desc)))
			return E_FAIL;
	}


	MSG_BOX("불러오기 완료!");
	//m_pImGui_Manager->Set_Load(false);
	return S_OK;
}



CLevel_Dungeon* CLevel_Dungeon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Dungeon* pInstance = new CLevel_Dungeon(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CLevel_Dungeon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Dungeon::Free()
{
	__super::Free();

	//Safe_Release(m_pImGui_Manager);


}
