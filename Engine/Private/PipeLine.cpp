#include "PipeLine.h"
#include "Shader.h"

CPipeLine::CPipeLine()
{
}

HRESULT CPipeLine::Bind_CamPosition(CShader* pShader, const _char* pConstantName)
{	
	return pShader->Bind_RawValue(pConstantName, Get_CamPosition(), sizeof(_float4));
}

HRESULT CPipeLine::Bind_TransformMatrix(D3DTS eTransformState, CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_TransformStateMatrices[ETOI(eTransformState)]);
}

HRESULT CPipeLine::Bind_TransformMatrix_Inverse(D3DTS eTransformState, CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_TransformStateInverseMatrices[ETOI(eTransformState)]);
}

void CPipeLine::Update()
{
	for (size_t i = 0; i < ETOI(D3DTS::END); i++)
	{
		XMStoreFloat4x4(&m_TransformStateInverseMatrices[i],
			XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformStateMatrices[i])));
	}
}

CPipeLine* CPipeLine::Create()
{
    return new CPipeLine;
}

void CPipeLine::Free()
{
	__super::Free();


}
