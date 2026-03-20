#include "Texture.h"

#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent { pDevice, pContext } 
{
}

CTexture::CTexture(const CTexture& Prototype)
    : CComponent(Prototype)
    , m_iNumSRVs { Prototype.m_iNumSRVs }
    , m_SRVs { Prototype.m_SRVs }
{
    for (auto& pSRV : m_SRVs)
        Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumSRVs)
{
    m_iNumSRVs = iNumSRVs;

    for (size_t i = 0; i < iNumSRVs; i++)
    {
        ID3D11ShaderResourceView* pSRV = { nullptr };

        _tchar          szTextureFilePath[MAX_PATH] = {};

        wsprintf(szTextureFilePath, pTextureFilePath, i);

        _tchar          szEXT[MAX_PATH] = {};

        _wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

        HRESULT         hr = {};

        if (false == lstrcmp(szEXT, TEXT(".dds")))
            hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

        else if (false == lstrcmp(szEXT, TEXT(".tga")))
            hr = E_FAIL;

        else
            hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

        if (FAILED(hr))
            return E_FAIL;

        m_SRVs.push_back(pSRV);
    }

    return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CTexture::Bind_ShaderResourceView(CShader* pShader, const _char* pConstantName, _uint iIndex)
{
    if (iIndex >= m_iNumSRVs)
        return E_FAIL;

    return pShader->Bind_SRV(pConstantName, m_SRVs[iIndex]);    
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumSRVs)
{
    CTexture* pInstance = new CTexture(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumSRVs)))
    {
        MSG_BOX("Failed to Created : CTexture");
        Safe_Release(pInstance);
    }
    return pInstance;
}


CComponent* CTexture::Clone(void* pArg)
{
    CTexture* pInstance = new CTexture(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CTexture");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CTexture::Free()
{
    __super::Free();

    for (auto& pSRV : m_SRVs)
        Safe_Release(pSRV);

   m_SRVs.clear();


   
}
