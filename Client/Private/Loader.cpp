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
#include "Sky.h"
#include "DefalutBlock.h"
#include "Building.h"
#include "Bridge.h"
#include "Table.h"
#include "Tent.h"
#include "WaterBucket.h"
#include "Sack.h"
#include "Bucket.h"
#include "LampPost.h"
#include "Grass.h"
#include "Lantern.h"
#include "Creeper.h"
#include "Bow.h"
#include "Zombie.h"
#include "SkeletonVanguard.h"
#include "NameLessKing.h"
#include "JusinBox.h"
#include "Arrow.h"
#include "HpBar.h"
#include "DamageFont.h"
#include "BossMark.h"



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

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_HpBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/mincraft/UI/Monster/ReSize_health_bar_%d.png"), 2))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture_healthbar");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_BossMark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/mincraft/UI/Boss/BossTex_%d.png"), 4))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture_healthbar");
		return E_FAIL;
	}

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/white.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : BackGround Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Tool"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/white.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : BackGround Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Damage"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/NonColor.png"), 1))))
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
	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/sky.dds"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Cube Texture");
		return E_FAIL;
	}

	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Block"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/mincraft_%d.dds"), 17))))
	{
		MSG_BOX("Faild to Add_Prototype : Cube Texture");
		return E_FAIL;
	}
	
	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩 중 입니다."));


	/* Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxCube");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxDefaultCube */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxDefaultCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxDefaultCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxDefaultCube");
		return E_FAIL;
	}

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
	

	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height2.bmp")))))
	{
		MSG_BOX("Faild to Add_Prototype : VIBuffer_Terrain");
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("모델를 로딩 중 입니다."));


	_matrix			PreLocalTransformMatrix = { XMMatrixIdentity() };
	/* Prototype_Component_Model_Fiona */
	PreLocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", ("Prototype_Component_Model_Fiona"), PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Fiona");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Skeleton"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/mincraft/Character/Monster/Skeleton.fbx", "Prototype_Component_Model_Skeleton", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Skeleton");
		return E_FAIL;
	}


	///* Prototype_Component_Model_ForkLift */
	//PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //* XMMatrixRotationY(XMConvertToRadians(180.f));

	//if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_ForkLift"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", "", PreLocalTransformMatrix))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : Model_Fiona");
	//	return E_FAIL;
	//}

	

	PreLocalTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Bunting"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Bunting/Bunting.fbx", "Prototype_Component_Model_Bunting", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Bunting");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Building"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Building/Building.fbx", "Prototype_Component_Model_Building", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Building");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Bridge"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/BrownBridge/T_RetractibleBridge.fbx", "Prototype_Component_Model_Bridge", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Bridge");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LampPost"), 
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/LampPost/LampPost.fbx", "Prototype_Component_Model_LampPost", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_LampPost");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Bucket"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/BucKet/Barrel.fbx", "Prototype_Component_Model_Bucket", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Barrel");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Sack"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Sack/Sack.fbx", "Prototype_Component_Model_Sack", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Sack");
		return E_FAIL;
	}


	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_WaterBarrel"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/BucKet/WaterBarrel.fbx", "Prototype_Component_Model_WaterBarrel", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_WaterBarrel");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Tent"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Tent/Tent.fbx", "Prototype_Component_Model_Tent", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Tent");
		return E_FAIL;
	}


	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_MapTable"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/MapTable/MapTable.fbx", "Prototype_Component_Model_MapTable", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_MapTable");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Lantern"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Lantern/Lantern.fbx", "Prototype_Component_Model_Lantern", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Lantern");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Grass"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Grass/Grass.fbx", "Prototype_Component_Model_Grass", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Grass");
		return E_FAIL;
	}

	


	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/mincraft/Character/Player.fbx", "Prototype_Component_Model_Player", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Player");
		return E_FAIL;
	}

	/* Prototype_Component_Model_ForkLift */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);// * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Sword"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Sword/Sword.fbx", "Prototype_Component_Model_Sword", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Sword");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Creeper"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/mincraft/Character/Monster/Creeper.fbx", "Prototype_Component_Model_Creeper", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Creeper");
		return E_FAIL;
	}

	/* Prototype_Component_Model_ForkLift */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f))* XMMatrixRotationZ(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Bow"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Bow/Bow.fbx", "Prototype_Component_Model_Bow", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Bow");
		return E_FAIL;
	}


	/* Prototype_Component_Model_ForkLift */
	PreLocalTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Arrow"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Arrow/Arrow.fbx", "Prototype_Component_Model_Arrow", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Arrow");
		return E_FAIL;
	}


	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Zombie"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/mincraft/Character/Monster/Zombie.fbx", "Prototype_Component_Model_Zombie", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Zombie");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_SkeletonVanguard"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/mincraft/Character/Monster/SkeletonVanguard.fbx", "Prototype_Component_Model_SkeletonVanguard", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_SkeletonVanguard");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_NameLessKing"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/mincraft/Character/Monster/Boss/NameLessKing.fbx", "Prototype_Component_Model_NameLessKing", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_NameLessKing");
		return E_FAIL;
	}
	
	


	lstrcpy(m_szLoadingText, TEXT("네비게이션을 로딩 중 입니다."));
	/* Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat"), TEXT("../Bin/DataFiles/Neighbors.dat")))))
	{
		MSG_BOX("Faild to Add_Prototype : Component_Navigation");
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


	//번팅
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bunting"),
		CBunting::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Bunting");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Building"), 
		CBuilding::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Building");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bridge"),
		CBridge::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Bridge");
		return E_FAIL;
	}

	
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Lantern"),
		CLantern::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Lantern");
		return E_FAIL;
	}

	

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_LampPost"), // 추후 가로등 과 램프 각각 소환해서 부착 한다 
		CLampPost::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_LampPost");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bucket"),
		CBucket::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Bucket");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sack"),
		CSack::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Sack");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_WaterBucket"),
		CWaterBucket::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_WaterBucket");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Tent"),
		CTent::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Tent");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Table"),
		CTable::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Table");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Grass"),
		CGrass::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Grass");
		return E_FAIL;
	}


	/* Prototype_GameObject_Player*/
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Player");
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
	

	/* Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : VIBuffer_Cube");
		return E_FAIL;
	}


	/* Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Sky");
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_DefaultBlock"),
		CDefalutBlock::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_DefaultBlock");
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Creeper"),
		CCreeper::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Creeper");
		return E_FAIL;
	}
	
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Bow"),
		CBow::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Bow");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Zombie"),
		CZombie::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Zombie");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_SkeletonVanguard"),
		CSkeletonVanguard::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_SkeletonVanguard");
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_NameLessKing"),
		CNameLessKing::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_NameLessKing");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_JusinBox"),
		CJusinBox::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_JusinBox");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Arrow"),
		CArrow::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Arrow");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_HpBar"),
		CHpBar::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_HpBar");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_DamageFont"),
		CDamageFont::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_DamageFont");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_BossMark"),
		CBossMark::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_BossMark");
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

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Texture_HpBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/mincraft/UI/Monster/ReSize_health_bar_%d.png"), 2))))
	{
		MSG_BOX("Faild to Add_Prototype : Texture_healthbar");
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));
	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/white.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : BackGround Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Texture_Tool"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/white.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : BackGround Texture");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Texture_Bunting"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/mincraft/Mesh/Prefab/Bunting/T_Bunting.png"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Bunting Texture");
		return E_FAIL;
	}
	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/sky.dds"), 1))))
	{
		MSG_BOX("Faild to Add_Prototype : Cube Texture");
		return E_FAIL;
	}

	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Texture_Block"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/mincraft_%d.dds"), 17))))
	{
		MSG_BOX("Faild to Add_Prototype : Cube Texture");
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩 중 입니다."));


	/* Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxCube");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxDefaultCube */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Shader_VtxDefaultCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxDefaultCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxDefaultCube");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxNorTex");
		return E_FAIL;
	}


	/* Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxMeshTex");
		return E_FAIL;
	}

	/* Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
	{
		MSG_BOX("Faild to Add_Prototype : Shader_VtxAnimMesh");
		return E_FAIL;
	}


	lstrcpy(m_szLoadingText, TEXT("사운드를 로딩 중 입니다."));


	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height2.bmp")))))
	{
		MSG_BOX("Faild to Add_Prototype : VIBuffer_Terrain");
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("모델를 로딩 중 입니다."));


	_matrix			PreLocalTransformMatrix = { XMMatrixIdentity() };
	/* Prototype_Component_Model_Fiona */
	PreLocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", ("Prototype_Component_Model_Fiona"), PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Fiona");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Skeleton"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/mincraft/Character/Monster/Skeleton.fbx", "Prototype_Component_Model_Skeleton", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Skeleton");
		return E_FAIL;
	}


	///* Prototype_Component_Model_ForkLift */
	//PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f); //* XMMatrixRotationY(XMConvertToRadians(180.f));

	//if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_ForkLift"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", "", PreLocalTransformMatrix))))
	//{
	//	MSG_BOX("Faild to Add_Prototype : Model_Fiona");
	//	return E_FAIL;
	//}



	PreLocalTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Bunting"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Bunting/Bunting.fbx", "Prototype_Component_Model_Bunting", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Bunting");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Building"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Building/Building.fbx", "Prototype_Component_Model_Building", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Building");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Bridge"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/BrownBridge/T_RetractibleBridge.fbx", "Prototype_Component_Model_Bridge", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Bridge");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_LampPost"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/LampPost/LampPost.fbx", "Prototype_Component_Model_LampPost", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_LampPost");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Bucket"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/BucKet/Barrel.fbx", "Prototype_Component_Model_Bucket", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Barrel");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Sack"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Sack/Sack.fbx", "Prototype_Component_Model_Sack", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Sack");
		return E_FAIL;
	}


	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_WaterBarrel"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/BucKet/WaterBarrel.fbx", "Prototype_Component_Model_WaterBarrel", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_WaterBarrel");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Tent"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Tent/Tent.fbx", "Prototype_Component_Model_Tent", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Tent");
		return E_FAIL;
	}


	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_MapTable"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/MapTable/MapTable.fbx", "Prototype_Component_Model_MapTable", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_MapTable");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Lantern"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Lantern/Lantern.fbx", "Prototype_Component_Model_Lantern", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Lantern");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Grass"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Grass/Grass.fbx", "Prototype_Component_Model_Grass", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Grass");
		return E_FAIL;
	}




	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/mincraft/Character/Player.fbx", "Prototype_Component_Model_Player", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Player");
		return E_FAIL;
	}

	/* Prototype_Component_Model_ForkLift */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);// * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Sword"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Sword/Sword.fbx", "Prototype_Component_Model_Sword", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Sword");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Creeper"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/mincraft/Character/Monster/Creeper.fbx", "Prototype_Component_Model_Creeper", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Creeper");
		return E_FAIL;
	}

	/* Prototype_Component_Model_ForkLift */
	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationZ(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Bow"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Bow/Bow.fbx", "Prototype_Component_Model_Bow", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Bow");
		return E_FAIL;
	}


	/* Prototype_Component_Model_ForkLift */
	PreLocalTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Arrow"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/mincraft/Mesh/Prefab/Arrow/Arrow.fbx", "Prototype_Component_Model_Arrow", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Arrow");
		return E_FAIL;
	}


	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_Zombie"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/mincraft/Character/Monster/Zombie.fbx", "Prototype_Component_Model_Zombie", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_Zombie");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_SkeletonVanguard"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/mincraft/Character/Monster/SkeletonVanguard.fbx", "Prototype_Component_Model_SkeletonVanguard", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_SkeletonVanguard");
		return E_FAIL;
	}

	PreLocalTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Model_NameLessKing"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/mincraft/Character/Monster/Boss/NameLessKing.fbx", "Prototype_Component_Model_NameLessKing", PreLocalTransformMatrix))))
	{
		MSG_BOX("Faild to Add_Prototype : Model_NameLessKing");
		return E_FAIL;
	}




	lstrcpy(m_szLoadingText, TEXT("네비게이션을 로딩 중 입니다."));
	/* Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat"), TEXT("../Bin/DataFiles/Neighbors.dat")))))
	{
		MSG_BOX("Faild to Add_Prototype : Component_Navigation");
		return E_FAIL;
	}



	lstrcpy(m_szLoadingText, TEXT("객체원형를 로딩 중 입니다."));
	/* Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Terrain");
		return E_FAIL;
	}

	/* Prototype_GameObject_FreeCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_FreeCamera");
		return E_FAIL;
	}

	/* Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Monster");
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


	//번팅
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Bunting"),
		CBunting::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Bunting");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Building"),
		CBuilding::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Building");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Bridge"),
		CBridge::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Bridge");
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Lantern"),
		CLantern::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Lantern");
		return E_FAIL;
	}



	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_LampPost"), // 추후 가로등 과 램프 각각 소환해서 부착 한다 
		CLampPost::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_LampPost");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Bucket"),
		CBucket::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Bucket");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Sack"),
		CSack::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Sack");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_WaterBucket"),
		CWaterBucket::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_WaterBucket");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Tent"),
		CTent::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Tent");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Table"),
		CTable::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Table");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Grass"),
		CGrass::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : Prototype_GameObject_Grass");
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


	/* Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : VIBuffer_Cube");
		return E_FAIL;
	}


	/* Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Sky");
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_DefaultBlock"),
		CDefalutBlock::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_DefaultBlock");
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Creeper"),
		CCreeper::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Creeper");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Bow"),
		CBow::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Bow");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Zombie"),
		CZombie::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Zombie");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_SkeletonVanguard"),
		CSkeletonVanguard::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_SkeletonVanguard");
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_NameLessKing"),
		CNameLessKing::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_NameLessKing");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_JusinBox"),
		CJusinBox::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_JusinBox");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(ETOI(LEVEL::DUNGEON), TEXT("Prototype_GameObject_Arrow"),
		CArrow::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Faild to Add_Prototype : GameObject_Arrow");
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
