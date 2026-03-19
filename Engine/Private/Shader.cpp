#include "Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CShader::CShader(const CShader& Prototype)
	: CComponent{ Prototype }
	, m_iNumPasses{ Prototype.m_iNumPasses }
	, m_pEffect{ Prototype.m_pEffect }
	, m_InputLayouts{ Prototype.m_InputLayouts }
{
	Safe_AddRef(m_pEffect);

	for (auto& pInputLayout : m_InputLayouts)
		Safe_AddRef(pInputLayout);
	{

	}

}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	_uint			iHlslFlag = {};

#ifdef _DEBUG
	iHlslFlag |= D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
#else
	iHlslFlag |= D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif	

	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0,
		m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc{};

	pTechnique->GetDesc(&TechniqueDesc);

	m_iNumPasses = TechniqueDesc.Passes;

	m_InputLayouts.reserve(m_iNumPasses);

	for (size_t i = 0; i < m_iNumPasses; i++)
	{
		ID3D11InputLayout* pInputLayout = { nullptr };

		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);
		if (nullptr == pPass)
			return E_FAIL;

		D3DX11_PASS_DESC		PassDesc{};
		pPass->GetDesc(&PassDesc);

		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;

		m_InputLayouts.push_back(pInputLayout);
	}

	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_iNumPasses ||
		nullptr == m_InputLayouts[iPassIndex])
		return E_FAIL;

	m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIndex)->Apply(0, m_pContext);

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

	return S_OK;
}

HRESULT CShader::Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
	{
		MSG_BOX("Failed to throw value to shader");
		return E_FAIL;
	}

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
	{
		MSG_BOX("Shader types do not match");
		return E_FAIL;
	}

	return pSRVariable->SetResource(pSRV);
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
	{
		MSG_BOX("Failed to throw value to shader");
		return E_FAIL;
	}

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
	{
		MSG_BOX("Shader types do not match");
		return E_FAIL;
	}

	return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(pMatrix));
}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
	{
		MSG_BOX("Failed to throw value to shader");
		return E_FAIL;
	}

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
	{
		MSG_BOX("Shader types do not match");
		return E_FAIL;
	}

	return pMatrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(pMatrices), 0, iNumMatrices);
}


HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
	{
		MSG_BOX("Failed to throw value to shader");
		return E_FAIL;
	}

	return pVariable->SetRawValue(pData, 0, iLength);
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElements)))
	{
		MSG_BOX("Failed to Created : CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CShader::Free()
{
	__super::Free();

	for (auto& pInputLayout : m_InputLayouts)
		Safe_Release(pInputLayout);

	m_InputLayouts.clear();

	Safe_Release(m_pEffect);
}
