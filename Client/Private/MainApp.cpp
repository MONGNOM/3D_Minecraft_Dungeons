#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Cursor.h"
#include <fstream>
#include "BossMark.h"
#include "BackGround.h"
#include "Inventory.h"
#include "Icon.h"

CMainApp::CMainApp()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);

	// D3D11_SAMPLER_DESC

	//D3D11_BLEND_DESC
	//D3D11_DEPTH_STENCIL_DESC
	//D3D11_RASTERIZER_DESC
	//m_pContext->RSSetState();
	//m_pContext->OMSetBlendState();
	// m_pContext->OMSetDepthStencilState();

}

HRESULT CMainApp::Initialize()
{
	/* 내 게임을 구동하기위한 기초 초기화작업을 수행한다. */

	/* 엔진을 이용하기위해 엔진프로젝트를 준비시킨다. */
	ENGINE_DESC		EngineDesc{};
	EngineDesc.hInstance = g_hInst;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iNumLevels = ETOI(LEVEL::END);
	EngineDesc.iViewportWidth = g_iWinSizeX;
	EngineDesc.iViewportHeight = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;


	if (FAILED(Ready_Fonts()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_For_Static_Level()))
		return E_FAIL;


	/* 내 게임의 시작을 위해 시작이되는 레벨 할당과 동작을 시킨다. */
	if(FAILED(Ready_StartLevel(LEVEL::LOGO)))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	//_float4		vClearColor = _float4(0.5f, 0.5f, 1.f, 1.f);
	_float4		vClearColor = _float4(0.0f, 0.0f, 0.f, 1.f);
	if (FAILED(m_pGameInstance->Clear_Buffers(&vClearColor)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw()))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Present()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Fonts()
{
	/*_ulong			dwByte = { };

	HANDLE			hFile = CreateFile(TEXT("../Bin/DataFiles/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
 	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_float3			vPoints[3] = {};

	vPoints[0] = _float3(0.f, 0.f, 10.f);
	vPoints[1] = _float3(10.f, 0.f, 0.f);
	vPoints[2] = _float3(0.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.f, 0.f, 10.f);
	vPoints[1] = _float3(10.f, 0.f, 10.f);
	vPoints[2] = _float3(10.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.f, 0.f, 20.f);
	vPoints[1] = _float3(10.f, 0.f, 10.f);
	vPoints[2] = _float3(0.f, 0.f, 10.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(10.f, 0.f, 10.f);
	vPoints[1] = _float3(20.f, 0.f, 0.f);
	vPoints[2] = _float3(10.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);*/

	/*MakeSpriteFont "넥슨lv1고딕 Bold" /FontSize:16 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 158ex.spritefont */
	
	//MakeSpriteFont "나눔고딕" / FontSize:32 / FontStyle : Bold / FastPack / CharacterRegion : 0x0020 - 0x00FF / CharacterRegion : 0x3131 - 0x3163 / CharacterRegion : 0xAC00 - 0xD800 / DefaultCharacter : 0xAC00 MC_Test_Bold.spritefont
	
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/158ex.SpriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Damage"), TEXT("../Bin/Resources/Fonts/MC_Galmuri11_24.SpriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_BossUI"), TEXT("../Bin/Resources/Fonts/MC_Galmuri11_28.SpriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_BossUI1"), TEXT("../Bin/Resources/Fonts/MC_Galmuri11_32.SpriteFont"))))
		return E_FAIL;

	return S_OK;
}



HRESULT CMainApp::Ready_Prototype_For_Static_Level()
{	
	/* Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxTex.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex_Blend"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxTex_Blend.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex_Hp"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxTex_Hp.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
		return E_FAIL;
	

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Cursor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/mincraft/UI/Cursor/Cursor%d.png"), 4))))
	{
		MSG_BOX("Fail to Add_Prototype : Cursor Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Inventroy"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/mincraft/UI/Inventory/BackGroundInventory.png"), 1))))
	{
		MSG_BOX("Fail to Add_Prototype : Inventroy Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_InventroyIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/mincraft/UI/Inventory/Icon/Icon%d.png"), 19))))
	{
		MSG_BOX("Fail to Add_Prototype : Inventroy Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Cursor"),
		CCursor::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Cursor");
		return E_FAIL;
	}
	

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Inventroy"),
		CInventory::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Inventory");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_InventoryIcon"),
		CIcon::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : InventoryIcon");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::AABB))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_Component_Collider_AABB");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::SPHERE))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_Component_Collider_SPHERE");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::OBB))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_Component_Collider_OBB");
		return E_FAIL;
	}



	return S_OK;
}

HRESULT CMainApp::Ready_StartLevel(LEVEL eStartLevelID)
{
	if (LEVEL::LOADING == eStartLevelID)
		return E_FAIL;


	if (FAILED(m_pGameInstance->Change_Level(ETOI(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Ready_Layer_UI(const _tchar* pLayerTag)
{
	//cursor
	if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Cursor"),
		ETOI(LEVEL::STATIC), pLayerTag)))
		return E_FAIL;

	// 인벤토리
	CInventory::INVENTORY_DESC invenDesc{};
	invenDesc.fX = g_iWinSizeX * 0.5f;
	invenDesc.fY = g_iWinSizeY * 0.5f;
	invenDesc.fSizeX = g_iWinSizeX;
	invenDesc.fSizeY = g_iWinSizeY;
	invenDesc.NumTexture = 0;

	if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Inventroy"),
		ETOI(LEVEL::STATIC), pLayerTag, &invenDesc)))
		return E_FAIL;

	return S_OK;
}



CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
	
}
