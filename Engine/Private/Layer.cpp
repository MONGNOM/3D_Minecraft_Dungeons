#include "Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
	{
		MSG_BOX("Failed to Add to Layer : GameObject");
		return E_FAIL;
	}

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Priority_Update(fTimeDelta);
	}
}

void CLayer::Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Update(fTimeDelta);
	}
}

void CLayer::Late_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Late_Update(fTimeDelta);
	}
}

CComponent* CLayer::Get_Component(_wstring objName, _wstring componentName)
{
	auto iter = find_if(m_GameObjects.begin(), m_GameObjects.end(), [&](CGameObject* gameObejct)
		{
			return (lstrcmp(objName.c_str(), gameObejct->Get_ObjectName().c_str()) == 0);
		});

	if (iter == m_GameObjects.end())
		return nullptr;

	
	return (*iter)->Get_Component(componentName);
}

list<CGameObject*>& CLayer::Get_GameObjects()
{
	return m_GameObjects;
}

void CLayer::DeleteObjects()
{
	for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if ((*iter)->Get_ObjectType() != CGameObject::OBJECTTYPE::ENVIRONMENT && true == (*iter)->Get_Dead())
		{
			Safe_Release(*iter);
			
			iter = m_GameObjects.erase(iter);

		}
		else
		{
			++iter;
		}
	}
}

CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}
