#pragma once

#include "Component.h"

/* 모델 정점들을 객체 기준으로 월드로 변환시켜주기위한 월드변환행렬을 보관하낟.  */
/* 이 행렬의 상태변환(월드에서의 상태변환)에 해당하는 인터페이스(기능)을 담당한다. */

NS_BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec = {};
		_float		fDegreePerSec = {};
	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& Prototype);
	virtual ~CTransform() = default;

public:
	void Set_WorldMatrix(_float4x4 _matrix) { XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(&_matrix)); }

	_float3 Get_Rotation() { return m_fRotation; }

	_vector Get_State(STATE eState) {
		return XMLoadFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ETOI(eState)][0]));
	}

	_matrix Get_WorldInverse()
	{
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	_float3 Get_Scaled() {
		return _float3(
			XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),
			XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
			XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK)))
		);
	}

	const _float4x4* Get_WorldMatrixPtr() const {
		return &m_WorldMatrix;
	}

	void Set_State(STATE eState, _fvector vState) {
		XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ETOI(eState)][0]), vState);
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Set_Rotation(_float3 rotation);
	void SetUp_Scale(_float fScaleX, _float fScaleY, _float fScaleZ);
	void Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);
	void Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Roll(_float fTimeDelta, _float fSpeed);
	void Go_Backward(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Down(_float fTimeDelta);
	void Go_Up(_float fTimeDelta);
	void Rotation(_fvector vAxis, _float fDegree);
	void Turn(_fvector vAxis, _float fTimeDelta);
	void LookAt(_fvector vAt);

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);


private:
	_float4x4				m_WorldMatrix = {};
	_float					m_fSpeedPerSec = {};
	_float					m_fRadianPerSec = {};
	_float3					m_fRotation = {};

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END