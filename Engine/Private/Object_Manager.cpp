#include "Object_Manager.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"

CObject_Manager::CObject_Manager()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	m_pLayers = new LAYERS[iNumLevels];

	return S_OK;
}


HRESULT CObject_Manager::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	CGameObject* pGameObject = { nullptr };
	CLayer* pLayer = { nullptr };

	if (nullptr == m_pLayers || 
		iLayerLevelIndex >= m_iNumLevels)
		goto except;

	pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pGameObject)
		goto except;

	pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iLayerLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);

    return S_OK;

except:
	MSG_BOX("Failed To Added : CloneGameObject");
	return E_FAIL;
}

void CObject_Manager::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if(nullptr != Pair.second)
				Pair.second->Priority_Update(fTimeDelta);
	
		}
	}
}

void CObject_Manager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
			{
				Pair.second->Update(fTimeDelta);
				Pair.second->DeleteObjects();
			}
		}
	}
}

void CObject_Manager::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Late_Update(fTimeDelta);
		}
	}
}

HRESULT CObject_Manager::Clear_Layers(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();

	return S_OK;
}

CComponent* CObject_Manager::Get_Component(_wstring objName, _wstring layName, _uint iLayerIndex, _wstring componentName)
{
	CLayer* pLayer = Find_Layer(iLayerIndex, layName);
	if (nullptr == pLayer)
		return nullptr;
	
	return pLayer->Get_Component(objName, componentName);

}

vector<class CGameObject*> CObject_Manager::Get_GameObjects(_uint iLayerIndex)
{
	vector<class CGameObject*> allObjects;

	for (auto& pLayer : m_pLayers[iLayerIndex])
	{
		auto& object = pLayer.second->Get_GameObjects();
		for	(CGameObject* pObject : object)
		{ 
			allObjects.push_back(pObject);
		}
	}

	return allObjects;
}

const list<class CGameObject*>& CObject_Manager::Get_LayerObjects(_uint iLayerIndex, _wstring layName)
{
	CLayer* player = Find_Layer(iLayerIndex, layName);

	if (player == nullptr)
	{
		MSG_BOX("오브젝트 매니저_Get_LayersObjects_레이어가 존재하지 않습니다");
	}
	
	return player->Get_GameObjects();
}

map<const _wstring, class CLayer*>& CObject_Manager::Get_Layer(_uint iLayerIndex)
{
	return m_pLayers[iLayerIndex];
}


CLayer* CObject_Manager::Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	auto	iter = m_pLayers[iLayerLevelIndex].find(strLayerTag);
	if(iter == m_pLayers[iLayerLevelIndex].end())
		return nullptr;

	return iter->second;
}

CObject_Manager* CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager* pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CObject_Manager::Free()
{
    __super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	Safe_Release(m_pGameInstance);
}
