#pragma once

#include "Base.h"

/* 객체들을 그리는 순서대로 그룹별로 모아서 저장한다. */
/* 저장된 순서대로 객체들의 드로우콜(Render)을 수행해준다. */
/* */

NS_BEGIN(Engine)

class CRenderer final : public CBase
{
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	void Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	void Draw();

private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	class CGameInstance*				m_pGameInstance = { nullptr }; 
	list<class CGameObject*>			m_RenderObjects[ETOI(RENDERGROUP::END)];

private:
	void Render_Priority();
	void Render_NonBlend();
	void Render_Blend();
	void Render_UI();
	void Render_INVEN();
	void Render_CURSOR();


public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
