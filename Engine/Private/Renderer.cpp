#include "Renderer.h"

#include "GameInstance.h"
#include "GameObject.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pGameInstance { CGameInstance::GetInstance() }
	, m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderer::Initialize()
{
	return S_OK;
}

void CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pGameObject)
{
	if (nullptr == pGameObject ||
		eRenderGroup >= RENDERGROUP::END)
	{
		MSG_BOX("Failed to Add : Renderer");
		return;
	}

	m_RenderObjects[ETOI(eRenderGroup)].push_back(pGameObject);

	Safe_AddRef(pGameObject);
}

void CRenderer::Draw()
{
	Render_Priority();

	Render_NonBlend();

	Render_Blend();

	Render_UI();
}

void CRenderer::Render_Priority()
{
	for (auto& pRenderObject : m_RenderObjects[ETOI(RENDERGROUP::PRIORITY)])
	{
		if (nullptr != pRenderObject)		
			pRenderObject->Render();

		Safe_Release(pRenderObject);				
	}

	m_RenderObjects[ETOI(RENDERGROUP::PRIORITY)].clear();
}

void CRenderer::Render_NonBlend()
{
	for (auto& pRenderObject : m_RenderObjects[ETOI(RENDERGROUP::NONBLEND)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ETOI(RENDERGROUP::NONBLEND)].clear();
}

void CRenderer::Render_Blend()
{
	for (auto& pRenderObject : m_RenderObjects[ETOI(RENDERGROUP::BLEND)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ETOI(RENDERGROUP::BLEND)].clear();
}

void CRenderer::Render_UI()
{
	for (auto& pRenderObject : m_RenderObjects[ETOI(RENDERGROUP::UI)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ETOI(RENDERGROUP::UI)].clear();
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	for (size_t i = 0; i < ETOI(RENDERGROUP::END); i++)
	{
		for (auto& pRenderObject : m_RenderObjects[i])
			Safe_Release(pRenderObject);

		m_RenderObjects[i].clear();
	}
}
