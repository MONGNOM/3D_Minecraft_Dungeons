#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct tagPartObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pParentMatrix = { nullptr };
	}PARTOBJECT_DESC;
protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& Prototype);
	virtual ~CPartObject() = default;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


protected:
	const _float4x4* m_pParentMatrix = { nullptr };
	_float4x4				m_CombinedWorldMatrix = {};

protected:
	void Update_CombinedWorldMatrix(_fmatrix ChildMatrix) {
		XMStoreFloat4x4(&m_CombinedWorldMatrix, ChildMatrix * XMLoadFloat4x4(m_pParentMatrix));
	}

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END