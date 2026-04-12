#pragma once

#include "Component.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"


NS_BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(COLLIDER eType);
	virtual HRESULT Initialize(void* pArg) override;
	_bool	m_isColl = { false };

	_bool Intersect(CCollider* target);
	class CGameObject* Get_Owner() { return m_pBounding->Get_Owner(); }
	void Set_isColl(_bool value) { m_isColl = value; }
	void SetActive_Collider(_bool value) { m_bSetActive = value; }

public:
	void Update(_fmatrix WorldMatrix);

#ifdef _DEBUG
	HRESULT Render();
#endif

private:
	COLLIDER				m_eType = { COLLIDER::END };
	class CBounding* m_pBounding = { nullptr };
	_bool m_bSetActive{ true };
#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };

#endif


public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();

};

NS_END