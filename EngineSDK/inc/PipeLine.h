#pragma once

/* 뷰, 투영행려을 보관하낟. */
/* 반복적인 갱신을 통해서 역행렬도 보관한다. */
/* 카메라 우ㅡㅓㄹ드 위치까지 젖앟나다. */
#include "Base.h"

NS_BEGIN(Engine)

class CPipeLine final : public CBase
{
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	const _float4x4* Get_Transform(D3DTS eTransformState) const {
		return &m_TransformStateMatrices[ETOI(eTransformState)];
	}
	const _float4* Get_CamPosition() const {
		return reinterpret_cast<const _float4*>(&m_TransformStateInverseMatrices[ETOI(D3DTS::VIEW)].m[3]);
	}

public:
	void Set_Transform(D3DTS eTransformState, _fmatrix TransformStateMatrix) {
		XMStoreFloat4x4(&m_TransformStateMatrices[ETOI(eTransformState)], TransformStateMatrix);
	}

	

public:
	HRESULT Bind_CamPosition(class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_TransformMatrix(D3DTS eTransformState, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_TransformMatrix_Inverse(D3DTS eTransformState, class CShader* pShader, const _char* pConstantName);
	void Update();
	

private:
	_float4x4				m_TransformStateMatrices[ETOI(D3DTS::END)] = { };
	_float4x4				m_TransformStateInverseMatrices[ETOI(D3DTS::END)] = { };


public:
	static CPipeLine* Create();
	virtual void Free() override;

};

NS_END