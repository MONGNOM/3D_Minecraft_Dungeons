#include "Material.h"
#include "Shader.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CMaterial::Initialize(const aiMaterial* pAIMaterial, const _char* pModelFilePath)
{
    _char               szDrive[MAX_PATH] = {};
    _char               szDir[MAX_PATH] = {};

    _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);


    for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
    {
        _uint       iNumTextures = { pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i)) };

        if (0 == iNumTextures)
            continue;

        m_MaterialTextures[i].reserve(iNumTextures);
        
        for (size_t j = 0; j < iNumTextures; j++)
        {
            aiString          strTexturePath = {};           


            if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(i), j, &strTexturePath)))
                continue;

            _char               szFileName[MAX_PATH] = {};
            _char               szEXT[MAX_PATH] = {};

            _splitpath_s(strTexturePath.C_Str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);


            _char               szFullPath[MAX_PATH] = {};

            strcpy_s(szFullPath, szDrive);
            strcat_s(szFullPath, szDir);
            strcat_s(szFullPath, szFileName);
            strcat_s(szFullPath, szEXT);


            _tchar              szTextureFilePath[MAX_PATH] = {};
            MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szTextureFilePath, MAX_PATH);

            HRESULT         hr = {};
            ID3D11ShaderResourceView* pSRV = { nullptr };

            if (false == strcmp(szEXT, (".dds")))
                hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

            else if (false == strcmp(szEXT, (".tga")))
                hr = E_FAIL;

            else
                hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

            if (FAILED(hr))
                return E_FAIL;

            m_MaterialTextures[i].push_back(pSRV);
        }
            
    }

    return S_OK;
}

HRESULT CMaterial::Bind_Material(CShader* pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iTextureIndex)
{
    if (nullptr == m_MaterialTextures[eMaterialType][iTextureIndex]||
        iTextureIndex >= m_MaterialTextures[eMaterialType].size())
        return E_FAIL;

    pShader->Bind_SRV(pConstantName, m_MaterialTextures[eMaterialType][iTextureIndex]);

    return S_OK;
}

HRESULT CMaterial::Binary_Mat(const _char* pstrFilePath)
{
    ID3D11ShaderResourceView* pSRV = nullptr;

    _tchar szTextureFilePath[MAX_PATH] = {};
    MultiByteToWideChar(CP_ACP, 0, pstrFilePath, strlen(pstrFilePath), szTextureFilePath, MAX_PATH);

    // 1. 텍스처를 딱 한 번만 로드합니다!
    HRESULT hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

    if (FAILED(hr))
    {
        // 파일이 없으면 그냥 S_OK를 넘기거나 예외 처리를 합니다.
        // (색깔만 있는 텍스처 없는 재질일 수도 있으니까요!)
        return E_FAIL;
    }

    // 2. 알아낸 텍스처를 딱 '디퓨즈(Diffuse)' 주머니에만 안전하게 넣습니다!! 
    // (aiTextureType_DIFFUSE 는 어심프에서 기본 색상 슬롯을 의미합니다. 보통 1번입니다.)
    m_MaterialTextures[aiTextureType_DIFFUSE].push_back(pSRV);

    return S_OK;
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMaterial* pAIMaterial, const _char* pModelFilePath)
{
    CMaterial* pInstance = new CMaterial(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pAIMaterial, pModelFilePath)))
    {
        MSG_BOX("Failed to Created : CMaterial");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CMaterial* CMaterial::CreateBinary(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMaterial* pAIMaterial, const _char* pstrFilePath)
{
    CMaterial* pInstance = new CMaterial(pDevice, pContext);

    if (FAILED(pInstance->Binary_Mat(pstrFilePath)))
    {
        MSG_BOX("Failed to Created : CMaterial");
        Safe_Release(pInstance);
    }
    return pInstance;
}




void CMaterial::Free()
{
    __super::Free();

    for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
    {
        for (auto& pSRV : m_MaterialTextures[i])
            Safe_Release(pSRV);
        m_MaterialTextures[i].clear();
    }

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

}
