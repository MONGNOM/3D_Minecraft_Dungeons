#include "Model.h"

#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Material.h"
#include "Animation.h"
#include "fstream"
#include "GameInstance.h"


CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
    
}

CModel::CModel(const CModel& Prototype)
    : CComponent( Prototype )
    , m_pAIScene{ Prototype.m_pAIScene }
    , m_eType{ Prototype.m_eType }
    , m_PreLocalTransformMatrix{ Prototype.m_PreLocalTransformMatrix }
    , m_iNumMeshes{ Prototype.m_iNumMeshes }
    , m_Meshes{ Prototype.m_Meshes }
    , m_iNumMaterials{ Prototype.m_iNumMaterials }
    , m_Materials{ Prototype.m_Materials }
    , m_iNumAnimations{ Prototype.m_iNumAnimations }
    
    //, m_Bones{ Prototype.m_Bones }
    //, m_Animations{Prototype.m_Animations}

{
    for (auto& pPrototypeAnim : Prototype.m_Animations)
        m_Animations.push_back(pPrototypeAnim->Clone());

    for (auto& pPrototypeBone : Prototype.m_Bones)
        m_Bones.push_back(pPrototypeBone->Clone());


    for (auto& pMaterial : m_Materials)
        Safe_AddRef(pMaterial);

    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);

    
}

_int CModel::Get_BoneIndex(const _char* pBoneName)
{
    _int    iBoneIndex = { -1 };

    auto    iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool {
        ++iBoneIndex;
        return pBone->isCompare(pBoneName);
        });

    if (iter == m_Bones.end())
        return -1;

    return iBoneIndex;
}

const _float4x4* CModel::Get_BoneMatrixPtr(const _char* pBoneName)
{
    auto    iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool {
        return pBone->isCompare(pBoneName);
        });

    if (iter == m_Bones.end())
        return nullptr;

    return (*iter)->Get_CombinedTransformationMatrixPtr();
}

HRESULT CModel::Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreLocalTransformMatrix, const string& name)
{
    _uint       iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

    if (MODEL::NONANIM == eType)
        iFlag |= aiProcess_PreTransformVertices;

    /* 실제 로드해야할 모든 데이터를 다 읽어들였다. */
    /* 모든 데이터는 aiScene이라는 타입안에 보관된다. */
    m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
    if (nullptr == m_pAIScene)
        return E_FAIL;

    m_eType = eType;
    XMStoreFloat4x4(&m_PreLocalTransformMatrix, PreLocalTransformMatrix);
    string modelName = name + ".bin";
    string strBinFilePath;

    switch (m_eType)
    {
    case MODEL::NONANIM:
         strBinFilePath = "../Bin/Resources/Binary/Static/";
         break;
    case MODEL::ANIM:
         strBinFilePath = "../Bin/Resources/Binary/Dynamic/";
        break;
    }


    
    ifstream fin(strBinFilePath.c_str() + modelName, ios::in | ios::binary);

    if (fin.is_open())
    {
        fin.close();

        if (FAILED(Ready_TestLoad(strBinFilePath, modelName)))
            return E_FAIL;

     /*   if (FAILED(Ready_Materials(strBinFilePath.c_str())))
            return E_FAIL;*/

        if (FAILED(Ready_Animations()))
            return E_FAIL;
    }
    else
    {
       string a =  name;
      // 바이너리 안됐을떄 
      /* 뼈들의 정보를 생성한다. */
        if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
            return E_FAIL;

        /* 메시를 생성한다. */
        if (FAILED(Ready_Meshes()))
            return E_FAIL;

        /* 재질 정보를 생성한다. */
        if (FAILED(Ready_Materials(pModelFilePath)))
            return E_FAIL;

        if (FAILED(Ready_Animations()))
            return E_FAIL;

        switch (eType)
        {
        case MODEL::NONANIM:
            m_pGameInstance->Ready_StaticBinary(m_iNumMeshes, m_pAIScene, modelName, XMLoadFloat4x4(&m_PreLocalTransformMatrix));
            break;

        case MODEL::ANIM:

            break;
        }

    }


    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
  
    m_Meshes[iMeshIndex]->Bind_Resources();
    m_Meshes[iMeshIndex]->Render();

    return S_OK;
}

_bool CModel::Play_Animation(_float fTimeDelta)
{
    _bool       isFinish = { false };
    /* 현재 애니메이션에 맞는 상태대로 뼈의 Transformaion을 갱신해준다. */
    isFinish = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(fTimeDelta, m_Bones, m_isAnimLoop); // ? bool로 한 의ㅏ도는?

    /* 모든 뼈를 순회하며 CombinedTransformaion를 셋팅해준다. */
    for (auto& pBone : m_Bones)
    {
        pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreLocalTransformMatrix));
    }

    return isFinish;
}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType, _uint iTextureIndex)
{
    return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()]->Bind_Material(pShader, pConstantName, eMaterialType, iTextureIndex);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
    return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);
}



HRESULT CModel::Ready_Meshes()
{
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, m_pAIScene->mMeshes[i], this, XMLoadFloat4x4(&m_PreLocalTransformMatrix));
        if (nullptr == pMesh)
            return E_FAIL;
        
        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
    m_iNumMaterials = m_pAIScene->mNumMaterials;

    for (size_t i = 0; i < m_iNumMaterials; i++)
    {
        CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, m_pAIScene->mMaterials[i], pModelFilePath);
        if (nullptr == pMaterial)
            return E_FAIL;

        m_Materials.push_back(pMaterial);
    }

    return S_OK;
}
HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
    CBone* pBone = CBone::Create(pAINode, iParentIndex);
    if (nullptr == pBone)
        return E_FAIL;

    m_Bones.push_back(pBone);

    _int        iPIndex = m_Bones.size() - 1;

    for (size_t i = 0; i < pAINode->mNumChildren; i++)
    {
        Ready_Bones(pAINode->mChildren[i], iPIndex);
    }

    return S_OK;
}

HRESULT CModel::Ready_Animations()
{
    m_iNumAnimations = m_pAIScene->mNumAnimations; // 애니메이션의 갯수

    for (size_t i = 0; i < m_iNumAnimations; i++)
    {
        CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], this); // 애니메이션 갯수 만큼 생성
        if (nullptr == pAnimation)
            return E_FAIL;

        m_Animations.push_back(pAnimation); // 생성된 애니메이션을 저장
    }

    return S_OK;
}

HRESULT CModel::Ready_TestBinary(const string& name)
{
    m_iNumMeshes = m_pAIScene->mNumMeshes;
    string strFileName = ("../Bin/Resources/Models/");
    string strModelName = ("Monster_Model.bin");
    string strFullname = strFileName + strModelName;

    ofstream fout(strFullname.c_str(), ios::out | ios::binary);
    fout.write((char*)&m_iNumMeshes, sizeof(int));


    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        int iNumVerts = m_pAIScene->mMeshes[i]->mNumVertices;
        int iNumIndices = m_pAIScene->mMeshes[i]->mNumFaces * 3; // 면 1개당 인덱스 3개

        VTXMESH* pVertices = new VTXMESH[iNumVerts];

        for (int j = 0; j < iNumVerts; ++j)
        {

            memcpy(&pVertices[j].vPosition, &m_pAIScene->mMeshes[i]->mVertices[j], sizeof(_float3));
            XMStoreFloat3(&pVertices[j].vPosition,
                XMVector3TransformCoord(XMLoadFloat3(&pVertices[j].vPosition), XMLoadFloat4x4(&m_PreLocalTransformMatrix)));

            memcpy(&pVertices[j].vNormal, &m_pAIScene->mMeshes[i]->mNormals[j], sizeof(_float3));
            XMStoreFloat3(&pVertices[j].vNormal,
                XMVector3TransformCoord(XMLoadFloat3(&pVertices[j].vNormal), XMLoadFloat4x4(&m_PreLocalTransformMatrix)));

            memcpy(&pVertices[j].vTangent, &m_pAIScene->mMeshes[i]->mTangents[j], sizeof(_float3));
           XMStoreFloat3(&pVertices[j].vTangent,
               XMVector3TransformNormal(XMLoadFloat3(&pVertices[j].vTangent), XMLoadFloat4x4(&m_PreLocalTransformMatrix)));

            memcpy(&pVertices[j].vTexcoord, &m_pAIScene->mMeshes[i]->mTextureCoords[0][j], sizeof(_float2));
        }

        _ulong* pIndices = new _ulong[iNumIndices];
        int iIndexOffset = 0;

        // mNumFaces(삼각형 면의 개수)만큼 돌면서 꼭짓점 3개의 번호를 빼옵니다.
        for (unsigned int j = 0; j < m_pAIScene->mMeshes[i]->mNumFaces; ++j)
        {
            aiFace face = m_pAIScene->mMeshes[i]->mFaces[j];
            pIndices[iIndexOffset++] = face.mIndices[0];
            pIndices[iIndexOffset++] = face.mIndices[1];
            pIndices[iIndexOffset++] = face.mIndices[2];
        }

        // 알아낸 개수를 내 커스텀 바이너리 파일(.bin)에 기록합니다.
        fout.write((char*)&iNumVerts, sizeof(int));
        fout.write((char*)&iNumIndices, sizeof(int));
        fout.write((char*)pVertices, sizeof(VTXMESH) * iNumVerts);
        fout.write((char*)pIndices, sizeof(_ulong) * iNumIndices);


        delete[] pVertices;
        delete[] pIndices;
    }

    fout.close(); // 파일 문 닫기!

    return S_OK;
}

HRESULT CModel::Ready_TestLoad(const string& strFilePath, const string& name)
{
    string fullname = strFilePath + name;
    // 1. 쓰기(out) 대신 읽기(in) 모드로 파일을 엽니다.
    ifstream fin(fullname.c_str(), ios::in | ios::binary);

    //  [방어 코드] 만약 .bin 파일이 없다면 실패 반환!
    if (!fin.is_open())
        return E_FAIL;

    // 2. 맨 처음에 썼던 '전체 메쉬 개수'부터 읽어옵니다.
    fin.read((char*)&m_iNumMeshes, sizeof(int));

    // 3. 메쉬 개수만큼 반복하며 상자를 뜯습니다!
    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        int iNumVerts = 0;
        int iNumIndices = 0;
        _uint iMaterialIndex = 0;
        fin.read(reinterpret_cast<char*>(&iMaterialIndex), sizeof(int));
        // [목차 뜯기] 이번 메쉬의 정점과 인덱스가 몇 개인지 읽어옵니다.
        fin.read((char*)&iNumVerts, sizeof(int));
        fin.read((char*)&iNumIndices, sizeof(int));

        // [빈 상자 준비] 알아낸 개수만큼 데이터를 담을 수 있는 빈 배열을 만듭니다.
        VTXMESH* pVertices = new VTXMESH[iNumVerts];
        _ulong* pIndices = new _ulong[iNumIndices];

        //  [대망의 알맹이 들이붓기] 
        // 파일에서 정점/인덱스 크기 덩어리만큼 통째로 긁어와서 내 빈 배열에 덮어씁니다! (로딩 0.01초 컷)
      
        fin.read((char*)pVertices, sizeof(VTXMESH) * iNumVerts);
        fin.read((char*)pIndices, sizeof(_ulong) * iNumIndices);

     
        // 1. 파일에서 번호표를 먼저 싹 읽어옵니다!
        // 2. 내 번호표를 꽉 쥐고 있습니다!!
       
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, pVertices, iNumVerts, pIndices, iNumIndices, iMaterialIndex, XMLoadFloat4x4(&m_PreLocalTransformMatrix));
        if (nullptr == pMesh)
            return E_FAIL;

        m_Meshes.push_back(pMesh);
        // ==========================================================
        //  이제 pVertices 와 pIndices 안에는 완벽한 3D 모델 데이터가 들어있습니다!
        // 여기서 CMesh를 생성하시고, 다이렉트X 버텍스/인덱스 버퍼를 만드시면 됩니다!
        //
        // 예시: 
        // CMesh* pMesh = CMesh::Create_FromBinary(m_pDevice, ..., pVertices, iNumVerts, pIndices, iNumIndices);
        // m_Meshes.push_back(pMesh);
        // ==========================================================

        //  버퍼에 다 넘겨줬다면 쓸모없어진 임시 배열은 메모리 누수 방지를 위해 지워줍니다!
        Safe_Delete_Array(pVertices);
        Safe_Delete_Array(pIndices);
    }


    int iNumMaterials = 0;

    // 1. 전체 옷(머테리얼)의 개수를 먼저 읽어옵니다.
    fin.read(reinterpret_cast<char*>(&iNumMaterials), sizeof(int));

    // 2. 개수만큼 반복하면서 이름표를 하나씩 받습니다.
    for (int i = 0; i < iNumMaterials; ++i)
    {
        int iLength = 0;

        // 3.  이번에 읽을 글자 수(길이)를 알아냅니다.
        fin.read(reinterpret_cast<char*>(&iLength), sizeof(int));

        // 길이가 0보다 크면 텍스처 이름이 존재한다는 뜻입니다!
        if (iLength > 0)
        {
            // 4. 글자 수만큼 빈 방(char 배열)을 만듭니다. 
            // (+1을 하는 이유: C++ 문자열의 끝을 알리는 마침표 '\0' 자리입니다)
            char* pTextureName = new char[iLength + 1];

            // 5.  빈 방에 파일의 진짜 글자들을 쫙 퍼담습니다.
            fin.read(pTextureName, iLength);

            // 6. "여기가 글자의 끝이야!" 하고 마침표를 딱 찍어줍니다.
            pTextureName[iLength] = '\0';

            // 7. 쓰기 편한 string으로 변환합니다!
            string strFileName = pTextureName;

            // 다 쓴 빈 방은 메모리 누수가 나지 않게 바로 삭제합니다.
            delete[] pTextureName;

            // ============================================================
            // 여기가 핵심!! 읽어온 이름표와 현재 게임 폴더 주소를 합칩니다!!
            // ============================================================
            string strFullPath = strFilePath + strFileName;

            CMaterial* pMaterial = CMaterial::CreateBinary(m_pDevice, m_pContext, m_pAIScene->mMaterials[i], strFullPath.c_str());
            if (nullptr == pMaterial)
                return E_FAIL;

            m_Materials.push_back(pMaterial);

            // 이제 완성된 strFullPath 를 사용해서 다이렉트X 텍스처를 Load 하시면 됩니다!
            // 예: Create_Texture(strFullPath);
        }
        else
        {
            // 텍스처가 없는 재질일 경우의 처리 (그냥 넘어가거나 기본색 지정)
        }

    }

    fin.close(); // 파일 문 닫기

    return S_OK;

}



CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, const string& name, _fmatrix PreLocalTransformMatrix)
{
    CModel* pInstance = new CModel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreLocalTransformMatrix, name)))
    {
        MSG_BOX("Failed to Created : CModel");
        Safe_Release(pInstance);
    }
    return pInstance;
}




CComponent* CModel::Clone(void* pArg)
{
    CModel* pInstance = new CModel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CModel");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CModel::Free()
{
    __super::Free();

    for (auto& pMaterial : m_Materials)
        Safe_Release(pMaterial);

    m_Materials.clear();


    for (auto& pMesh : m_Meshes)
        Safe_Release(pMesh);

    m_Meshes.clear();

    for (auto& pBone : m_Bones)
        Safe_Release(pBone);
    m_Bones.clear();
    
    for (auto& mAnimation : m_Animations)
        Safe_Release(mAnimation);
    m_Animations.clear();

    m_Importer.FreeScene();


}
