#include "Loader.h"
#include "GameInstance.h"

#include "BackGround.h"
#include "Terrain.h"
#include "FreeCamera.h"
#include "Monster.h"
#include "Player.h"
#include "HotBar.h"
#include "Body_Skeleton.h"
#include "Skeleton.h"
#include "Bunting.h"
#include "Body_Player.h"
#include "Weapon.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

/* typedef unsigned (__stdcall* _beginthreadex_proc_type)(void*); */

unsigned int APIENTRY ThreadMain(void* pArg)
{


	CLoader* pLoader = static_cast<CLoader*>(pArg);
	if (nullptr == pLoader)
		return 1;

	if (FAILED(pLoader->Loading()))
		return 1;
	
	return 0;
}



HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	/* 실제 자원을 로드해주기위한 스레드를 생성해준다. */	
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr);

	if (0 == m_hThread)
	{
		MSG_BOX("Failed To Created : Thread");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading()
{
	
	HRESULT		hr = {};

	EnterCriticalSection(&m_CriticalSection);

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);


	switch (m_eNextLevelID)
	{
	case LEVEL::LOGO:
		hr = Loading_For_LogoLevel();
		break;

	case LEVEL::GAMEPLAY:
		hr = Loading_For_GamePlayLevel();
		break;

	case LEVEL::DUNGEON:
		hr = Loading_For_Dungeon();
		break;

	default:
		hr = E_FAIL;
	}

	CoUninitialize();
	
	LeaveCriticalSection(&m_CriticalSection);

	return hr;
}

#ifdef _DEBUG

HRESULT CLoader::Print_LoadingText()
{
	SetWindowText(g_hWnd, m_szLoadingText);

	return S_OK;
}

#endif

HRESULT CLoader::Loading_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));	
	/* Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::LOGO), TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
	{
		MSG_BOX("Faild to Add_Prototype : BackGround Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::LOGO), TEXT("Prototype_Component_Texture_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/mincraft/Title/TitleImage.png"), 1))))
	{
		MSG_BOX("Fail to Add_Prototype : Title Texture");
		return E_FAIL;
	}


	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("사운드를 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("모델를 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("객체원형를 로딩 중 입니다."));
	/* Prototype_GameObject_BackGround */

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::LOGO), TEXT("Prototype_GameObject_Title"),
		CBackGround::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Title");
		return E_FAIL;
	}


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));
	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/white.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : BackGround Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Imgui"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/mincraft/Test/block%d.png"), 4))))
	{
		MSG_BOX("Faild to Add_Prototype : Imgui Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Tool"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/white.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : BackGround Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Bunting"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/mincraft/Mesh/Prefab/Bunting/T_Bunting.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Bunting Texture");
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩 중 입니다."));
	/* Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxNorTex");
		return E_FAIL;
	}


	/* Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxMeshTex");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxAnimMesh");
		return E_FAIL;
	}


	lstrcpy(m_szLoadingText, TEXT("사운드를 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("모델를 로딩 중 입니다."));

	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
	{
		MSG_BOX("Faild to Add_Prototype : VIBuffer_Terrain");
		return E_FAIL;
	}

	_matrix			PreLocalTransformMatrix = { XMMatrixIdentity() };

	/* Prototype_Component_Model_Fiona */
	PreLocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Fiona");
		return E_FAIL;
	}

	

	/* Prototype_Component_Model_ForkLift */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Fiona");
		return E_FAIL;
	}


	PreLocalTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Bunting"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Bunting/Bunting.fbx", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Bunting");
		return E_FAIL;
	}


	lstrcpy(m_szLoadingText, TEXT("객체원형를 로딩 중 입니다."));
	/* Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Terrain");
		return E_FAIL;
	}

	/* Prototype_GameObject_FreeCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_FreeCamera");
		return E_FAIL;
	}

	/* Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Monster");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Skeleton"),
		CSkeleton::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Skeleton");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Skeleton"),
		CBody_Skeleton::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Body_Skeleton");
		return E_FAIL;
	}


	//if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bunting"),
	//	CBody_Skeleton::Create(m_pDevice, m_pContext))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Bunting");
	//	return E_FAIL;
	//}

	//번팅
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bunting"),
		CBunting::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Bunting");
		return E_FAIL;
	}


	PreLocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Skeleton"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/mincraft/Character/Monster/Skeleton.fbx", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Skeleton");
		return E_FAIL;
	}

	/* Prototype_GameObject_Player*/
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Player");
		return E_FAIL;
	}


	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/mincraft/Character/Player.fbx", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Player");
		return E_FAIL;
	}


	/* Prototype_GameObject_Body_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Body_Player");
		return E_FAIL;
	}

	/* Prototype_GameObject_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon"),
		CWeapon::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Weapon");
		return E_FAIL;
	}

	/* Prototype_Component_Model_ForkLift */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Sword"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Sword/Sword.fbx", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Fiona");
		return E_FAIL;
	}




	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Dungeon()
{

	lstrcpy(m_szLoadingText, TEXT("사운드를 로딩 중 입니다."));


	/*if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Texture_HotBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/mincraft/HotBar2/hotbarBackground%d.png"), 6))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture_HotBar");
		return E_FAIL;
	}*/

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Texture_HotBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/mincraft/UI/HotBar/Rhotbar.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture_HotBar");
		return E_FAIL;
	}


	/* Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxAnimMesh");
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_FreeCamera");
		return E_FAIL;
	}


	lstrcpy(m_szLoadingText, TEXT("모델를 로딩 중 입니다."));


	_matrix			PreLocalTransformMatrix = { XMMatrixIdentity() };
	/* Prototype_Component_Model_Fiona */
	PreLocalTransformMatrix =  XMMatrixRotationY(XMConvertToRadians(180.f))* XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Skeleton"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/mincraft/Character/Monster/Skeleton.fbx", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Fiona");
		return E_FAIL;
	}

	PreLocalTransformMatrix = { XMMatrixIdentity() };
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/mincraft/Character/Player.fbx", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Player");
		return E_FAIL;
	}

	/* Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Monster");
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_HotBar"),
		CHotBar::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_HotBar");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Skeleton"),
		CSkeleton::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Skeleton");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Body_Skeleton"),
		CBody_Skeleton::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Body_Skeleton");
		return E_FAIL;
	}

	/* Prototype_GameObject_Player*/
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Player");
		return E_FAIL;
	}

	/* Prototype_GameObject_Body_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Body_Player");
		return E_FAIL;
	}

	/* Prototype_GameObject_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Weapon"),
		CWeapon::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Weapon");
		return E_FAIL;
	}
	
	

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));


	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);
	
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
