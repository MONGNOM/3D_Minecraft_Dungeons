#include "GameInstance.h"

#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Prototype_Manager.h"
#include "Object_Manager.h"
#include "Renderer.h"
#include "PipeLine.h"
#include "Input_Device.h"
#include "Light_Manager.h"
#include "Picking_Manager.h"
#include "Font_Manager.h"
#include "ModelConverter.h"
#include "DataManager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext)
{
	/* 그래픽 디바이스를 초기화한다. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iViewportWidth, EngineDesc.iViewportHeight, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	/* 타이머 매니져를 생성해 둔다. */
	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pInput_Device = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pPicking_Manager = CPicking_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pPicking_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pModelConverter = CModelConverter::Create();
	if (nullptr == m_pModelConverter)
		return E_FAIL;

	m_pDataManager = CDataManager::Create();
	if (nullptr == m_pDataManager)
		return E_FAIL;

	
	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pInput_Device->Update();

	m_pObject_Manager->Priority_Update(fTimeDelta);

	m_pPipeLine->Update();

	m_pObject_Manager->Update(fTimeDelta);

	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pLevel_Manager->Update(fTimeDelta);
}

HRESULT CGameInstance::Draw()
{
	m_pRenderer->Draw();

	m_pLevel_Manager->Render();

	return S_OK;
}

void CGameInstance::Clear_Resources(_uint iLevelIndex)
{
	m_pObject_Manager->Clear_Layers(iLevelIndex);
	m_pPrototype_Manager->Clear_Prototypes(iLevelIndex);
}

_float CGameInstance::Random(_float fMin, _float fMax)
{
	return fMin + (static_cast<_float>(rand()) / RAND_MAX) * (fMax - fMin);
}

HRESULT CGameInstance::Clear_Buffers(const _float4* pClearColor)
{
	if (FAILED(m_pGraphic_Device->Clear_BackBuffer_View(pClearColor)))
		return E_FAIL;
	if (FAILED(m_pGraphic_Device->Clear_DepthStencil_View()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::Present()
{
	return m_pGraphic_Device->Present();	
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Change_Level(_uint iNewLevelIndex, CLevel* pNewLevel)
{
	return m_pLevel_Manager->Change_Level(iNewLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	return m_pPrototype_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pPrototype_Manager->Clone_Prototype(ePrototype, iLevelIndex, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject(iPrototypeLevelIndex, strPrototypeTag, iLayerLevelIndex, strLayerTag, pArg);
}

CComponent* CGameInstance::Get_Component(_wstring objName, _wstring layName, _uint iLayerIndex, _wstring componentName)
{
	return m_pObject_Manager->Get_Component(objName, layName, iLayerIndex, componentName);
}

vector<class CGameObject*> CGameInstance::Get_GameObjects(_uint iLayerIndex)
{
	return m_pObject_Manager->Get_GameObjects(iLayerIndex);
}

const list<class CGameObject*>& CGameInstance::Get_LayerObjects(_uint iLayerIndex, _wstring layName)
{
	return m_pObject_Manager->Get_LayerObjects(iLayerIndex, layName);
}

map<const _wstring, class CLayer*>& CGameInstance::Get_Layer(_uint iLayerIndex)
{
	return m_pObject_Manager->Get_Layer(iLayerIndex);
}

void CGameInstance::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pGameObject)
{
	return m_pRenderer->Add_RenderGroup(eRenderGroup, pGameObject);
}

HRESULT CGameInstance::Bind_CamPosition(CShader* pShader, const _char* pConstantName)
{
	return m_pPipeLine->Bind_CamPosition(pShader, pConstantName);
}

HRESULT CGameInstance::Bind_TransformMatrix(D3DTS eTransformState, CShader* pShader, const _char* pConstantName)
{
	return m_pPipeLine->Bind_TransformMatrix(eTransformState, pShader, pConstantName);
}

HRESULT CGameInstance::Bind_TransformMatrix_Inverse(D3DTS eTransformState, CShader* pShader, const _char* pConstantName)
{
	return m_pPipeLine->Bind_TransformMatrix_Inverse(eTransformState, pShader, pConstantName);
}

const _float4x4* CGameInstance::Get_Transform(D3DTS eTransformState) const
{
	return m_pPipeLine->Get_Transform(eTransformState);
}

const _float4* CGameInstance::Get_CamPosition() const
{
	return m_pPipeLine->Get_CamPosition();

}

void CGameInstance::Set_Transform(D3DTS eTransformState, _fmatrix TransformStateMatrix)
{
	m_pPipeLine->Set_Transform(eTransformState, TransformStateMatrix);
}

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(DIMB eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(DIMM eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

_bool CGameInstance::Get_DIKeyPress(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyPress(byKeyID);
}

_bool CGameInstance::Get_DIKeyDown(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyDown(byKeyID);
}

_bool CGameInstance::Get_DIKeyUp(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyUp(byKeyID);
}

_bool CGameInstance::Get_DIMousePress(DIMB eMouse)
{
	return m_pInput_Device->Get_DIMousePress(eMouse);
}

_bool CGameInstance::Get_DIMouseDown(DIMB eMouse)
{
	return m_pInput_Device->Get_DIMouseDown(eMouse);
}

_bool CGameInstance::Get_DIMouseUp(DIMB eMouse)
{
	return m_pInput_Device->Get_DIMouseUp(eMouse);
}

const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex)
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}

_bool CGameInstance::Picking_Pos(HWND hWnd, CVIBuffer_Terrain* pTerrainBufferCom, CTransform* pTerrainTransformCom, _uint numZ, _uint numX, _float3* pos)
{
	return m_pPicking_Manager->Picking_Pos(hWnd, pTerrainBufferCom, pTerrainTransformCom, numZ, numX, pos);
}

_bool CGameInstance::Picking_Object(HWND hWnd, RayHit& hit)
{
	return m_pPicking_Manager->Picking_Object(hWnd, hit);
}



HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}

void CGameInstance::Draw_Font(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor)
{
	m_pFont_Manager->Draw(strFontTag, pText, vPosition, vColor);
}

HRESULT CGameInstance::Ready_StaticBinary(_uint numMeshs, const aiScene* m_pAIScene, const string& name, _fmatrix PreTransformMatrix)
{
	return m_pModelConverter->Ready_StaticBinary(numMeshs, m_pAIScene, name, PreTransformMatrix);
}

HRESULT CGameInstance::Ready_DynamicBinary(_uint numMeshs, const aiScene* m_pAIScene, const string& name, CModel* pModel, vector<class CBone*>& bones)
{
	return m_pModelConverter->Ready_DynamicBinary(numMeshs, m_pAIScene, name, pModel, bones);
}

HRESULT CGameInstance::Save_Date(const vector<tagObjectInfo>& objectinfo)
{
	return m_pDataManager->Save_Date(objectinfo);
}

HRESULT CGameInstance::Load_Date(const _tchar* filePath, vector<tagObjectInfo>& objectinfo)
{
	return m_pDataManager->Load_Date(filePath, objectinfo);
}



void CGameInstance::Release_Engine()
{
	Safe_Release(m_pModelConverter);
	Safe_Release(m_pDataManager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pPicking_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pGraphic_Device);

	DestroyInstance();
}

void CGameInstance::Free()
{
	__super::Free();

	
}
