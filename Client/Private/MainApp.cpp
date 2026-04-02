#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Cursor.h"
#include <fstream>


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

	m_pGameInstance->Save_Date();

	m_pGameInstance->Load_Date(TEXT("../Bin/DataFiles/Test_Save.json"));

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
	
}

HRESULT CMainApp::Render()
{
	_float4		vClearColor = _float4(0.5f, 0.5f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Clear_Buffers(&vClearColor)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw()))
		return E_FAIL;

	m_pGameInstance->Draw_Font(TEXT("Font_Default"), TEXT("니네들은 싸우지마, 욕하지마!"), _float2(100.f, 0.f));


	if (FAILED(m_pGameInstance->Present()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Fonts()
{
	_ulong			dwByte = { };
	HANDLE			hFile = CreateFile(TEXT("../Bin/DataFiles/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
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

	CloseHandle(hFile);

	/*MakeSpriteFont "넥슨lv1고딕 Bold" /FontSize:16 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 158ex.spritefont */

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/158ex.SpriteFont"))))
		return E_FAIL;

	return S_OK;
}



HRESULT CMainApp::Ready_Prototype_For_Static_Level()
{	
	/* Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Shaderfiles/Shader_VtxTex.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements))))
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

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Cursor"),
		CCursor::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Cursor");
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
	if (FAILED(m_pGameInstance->Add_GameObject(ETOI(LEVEL::STATIC), TEXT("Prototype_GameObject_Cursor"),
		ETOI(LEVEL::STATIC), pLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Gara_Binary()
{
	//// 1. 파일 쓰기 모드로 열기 (확장자는 마음대로!)
	//ofstream fout("House_Model.bin", ios::out | ios::binary);

	//// 2.  1호차 (목차) 세팅 및 저장
	//STATIC_MODEL_HEADER tHeader;
	//tHeader.iNumVertices = vecVertices.size();   // Assimp로 모아둔 정점 개수
	//tHeader.iNumIndices = vecIndices.size();     // Assimp로 모아둔 인덱스 개수
	//tHeader.iNumMaterials = vecMaterials.size(); // 재질 개수

	//// 목차를 통째로 파일에 도장 찍듯 쾅! 씁니다.
	//fout.write((char*)&tHeader, sizeof(STATIC_MODEL_HEADER));

	//// 3.  2호차 (정점 데이터) 저장
	//// vecVertices 배열의 첫 번째 주소부터 끝까지 통째로 파일에 복사!
	//fout.write((char*)vecVertices.data(), sizeof(VTXMODEL) * tHeader.iNumVertices);

	//// 4.  3호차 (인덱스 데이터) 저장
	//fout.write((char*)vecIndices.data(), sizeof(_ulong) * tHeader.iNumIndices);

	//// 5.  4호차 (재질 데이터) 저장
	//fout.write((char*)vecMaterials.data(), sizeof(MATERIAL_DATA) * tHeader.iNumMaterials);

	//// 6. 기차 문 닫기 (저장 끝!)
	//fout.close();
	//
	//	
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
