#include "ModelConverter.h"
#include "fstream"
#include "Model.h"
#include "Bone.h"

CModelConverter::CModelConverter()
{
}

HRESULT CModelConverter::Ready_DynamicBinary(_uint numMeshs, const aiScene* m_pAIScene, const string& name, class CModel* pModel, vector<class CBone*>& bones)
{
    string strFileName = ("../Bin/Resources/Binary/Dynamic/");
    string strFullname = strFileName + name;

    ofstream fout(strFullname.c_str(), ios::out | ios::binary);
    fout.write((char*)&numMeshs, sizeof(int));
    

    for (int i = 0; i < numMeshs; i++)
    {
        int iMaterialIndex = m_pAIScene->mMeshes[i]->mMaterialIndex;

        int iNumVerts = m_pAIScene->mMeshes[i]->mNumVertices;
        int iNumIndices = m_pAIScene->mMeshes[i]->mNumFaces * 3;

        int iNumBones = m_pAIScene->mMeshes[i]->mNumBones;

        VTXANIMMESH* pVertices = new VTXANIMMESH[iNumVerts];
        ZeroMemory(pVertices, sizeof(VTXANIMMESH) * iNumVerts);

        for (size_t j = 0; j < iNumVerts; j++)
        {
            memcpy(&pVertices[j].vPosition, &m_pAIScene->mMeshes[i]->mVertices[j], sizeof(_float3));
            memcpy(&pVertices[j].vNormal, &m_pAIScene->mMeshes[i]->mNormals[j], sizeof(_float3));
            memcpy(&pVertices[j].vTangent, &m_pAIScene->mMeshes[i]->mTangents[j], sizeof(_float3));
            memcpy(&pVertices[j].vTexcoord, &m_pAIScene->mMeshes[i]->mTextureCoords[0][j], sizeof(_float2));
        }

        vector<_int> vecBoneIndices;
        vector<_float4x4> vecOffsetMatrices;

        for (size_t k = 0; k < iNumBones; k++)
        {
            aiBone* pAIBone = m_pAIScene->mMeshes[i]->mBones[k];

            _int iBoneIndex = pModel->Get_BoneIndex(pAIBone->mName.data);
            if (-1 == iBoneIndex) return E_FAIL;

            //  파일에 바로 쓰지 않고 가방에 담습니다.
            vecBoneIndices.push_back(iBoneIndex);

            _float4x4 OffsetMatrix = {};
            memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
            XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

            //  파일에 바로 쓰지 않고 가방에 담습니다.
            vecOffsetMatrices.push_back(OffsetMatrix);
            for (size_t l = 0; l < pAIBone->mNumWeights; l++)
            {
                /* 이 뼈가 j번째로 영향을 주는 정점의 정보? */
                aiVertexWeight	AIVertexWeight = pAIBone->mWeights[l];

                if (0 == pVertices[AIVertexWeight.mVertexId].vBlendWeight.x)
                {
                    pVertices[AIVertexWeight.mVertexId].vBlendIndex.x = k;
                    pVertices[AIVertexWeight.mVertexId].vBlendWeight.x = AIVertexWeight.mWeight;
                }

                else if (0 == pVertices[AIVertexWeight.mVertexId].vBlendWeight.y)
                {
                    pVertices[AIVertexWeight.mVertexId].vBlendIndex.y = k;
                    pVertices[AIVertexWeight.mVertexId].vBlendWeight.y = AIVertexWeight.mWeight;
                }

                else if (0 == pVertices[AIVertexWeight.mVertexId].vBlendWeight.z)
                {
                    pVertices[AIVertexWeight.mVertexId].vBlendIndex.z = k;
                    pVertices[AIVertexWeight.mVertexId].vBlendWeight.z = AIVertexWeight.mWeight;
                }

                else if (0 == pVertices[AIVertexWeight.mVertexId].vBlendWeight.w)
                {
                    pVertices[AIVertexWeight.mVertexId].vBlendIndex.w = k;
                    pVertices[AIVertexWeight.mVertexId].vBlendWeight.w = AIVertexWeight.mWeight;
                }
            }
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

        fout.write(reinterpret_cast<const char*>(&iMaterialIndex), sizeof(int));
        fout.write((char*)&iNumVerts, sizeof(int));
        fout.write((char*)&iNumIndices, sizeof(int));
        fout.write((char*)pVertices, sizeof(VTXANIMMESH) * iNumVerts);
        fout.write((char*)pIndices, sizeof(_ulong) * iNumIndices);

        //  새롭게 추가된 부분: 뼈대 개수를 쓰고, 모아둔 뼈대 정보를 쫙 씁니다!
        fout.write((char*)&iNumBones, sizeof(int)); // Read 쪽의 iNumBonesInMesh 와 매칭됨!

        for (size_t k = 0; k < iNumBones; k++)
        {
            fout.write((char*)&vecBoneIndices[k], sizeof(_int));
            fout.write((char*)&vecOffsetMatrices[k], sizeof(_float4x4));
        }

        delete[] pVertices;
        delete[] pIndices;
    }

    // 1. 이 모델이 가진 전체 옷(머테리얼)의 개수를 구웁니다.
    int iNumMaterials = m_pAIScene->mNumMaterials;
    fout.write(reinterpret_cast<const char*>(&iNumMaterials), sizeof(int));

    // 2. 옷의 개수만큼 반복하면서 텍스처 이름을 하나씩 꺼내옵니다.
    for (int i = 0; i < iNumMaterials; ++i)
    {
        aiMaterial* pMaterial = m_pAIScene->mMaterials[i];
        aiString aiTexName;



        // Assimp에게 "이 옷의 기본 텍스처 이름 좀 줘!" 라고 물어봅니다.
        if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexName))
        {
            // Assimp가 준 이름을 깔끔한 C++ string으로 받습니다. (예: "fiona_D.png")
            string strTextureName = aiTexName.data;

            // 3.  글자 수(길이)를 먼저 구웁니다! (택배 상자 크기 예고)
            int iLength = strTextureName.length();
            fout.write(reinterpret_cast<const char*>(&iLength), sizeof(int));

            // 4.  진짜 글자들을 구웁니다!
            fout.write(strTextureName.c_str(), iLength);
        }
        else
        {
            // 만약 텍스처 이미지가 없는 단색 재질이라면? 
            // "글자 수가 0이다!" 라고 구워서 빈 상자임을 표시합니다.
            int iZeroLength = 0;
            fout.write(reinterpret_cast<const char*>(&iZeroLength), sizeof(int));
        }
    }


    _uint iNumBone = bones.size();
    fout.write((char*)&iNumBone, sizeof(_uint));

    for (size_t i = 0; i < iNumBone; i++)
    {
        CBone* bone = bones[i];

        string strBoneName = bone->Get_Name();
        int iNameLenth = strBoneName.length();
        fout.write((char*)&iNameLenth, sizeof(int));
        fout.write(strBoneName.c_str(), iNameLenth);

        int iParenindex = bone->Get_ParentIndex();
        fout.write((char*)&iParenindex, sizeof(int));

        _float4x4 localMatrix = bone->Get_TransformMatrix();
        fout.write((char*)&localMatrix, sizeof(_float4x4));
    }

    _uint iNumAnimtion = m_pAIScene->mNumAnimations;

    fout.close(); // 파일 문 닫기!

    for (size_t i = 0; i < iNumAnimtion; i++)
    {
        string strFileNameAni = ("../Bin/Resources/Binary/Dynamic/Animation/");
        aiAnimation* pAnimation = m_pAIScene->mAnimations[i];
        string strFullname = strFileNameAni + pAnimation->mName.data + ".Anim"; // 애니메에션 이름으로 저장

        ofstream fAniout(strFullname.c_str(), ios::out | ios::binary);

        _uint strNum = strlen(pAnimation->mName.data); // 애니메이션 이름의 갯수
        _float duration = pAnimation->mDuration;
        _float tickPerSecond = pAnimation->mTicksPerSecond;
        _uint numChannels = pAnimation->mNumChannels;

        fAniout.write((char*)&strNum, sizeof(_uint)); // 애니메이션 이름 갯수 저장
        fAniout.write(pAnimation->mName.data, strNum); // 애니메이션 이름 갯수 저장

        fAniout.write((char*)&duration, sizeof(_float)); // 애니메이션 길이 저장
        fAniout.write((char*)&tickPerSecond, sizeof(_float)); // 애니메이션 재생 속도 저장
        fAniout.write((char*)&numChannels, sizeof(_uint)); // 애니메이션 재생 속도 저장


        for (size_t j = 0; j < numChannels; j++)
        {
            _uint  iBonindex = pModel->Get_BoneIndex(pAnimation->mChannels[j]->mNodeName.data);
            _uint NumScale   = pAnimation->mChannels[j]->mNumScalingKeys;
            _uint NumRot     = pAnimation->mChannels[j]->mNumRotationKeys;
            _uint NumPos     = pAnimation->mChannels[j]->mNumPositionKeys;

            fAniout.write((char*)&iBonindex, sizeof(_uint)); // 애니메이션 이름 갯수 저장
           //fAniout.write((char*)&NumScale, sizeof(_uint)); //  채널 크기 크기 갯수
           //fAniout.write((char*)&NumRot, sizeof(_uint)); //    채널 회전 갯수 저장
           //fAniout.write((char*)&NumPos, sizeof(_uint)); //    채널 위치 갯수 저장


            _uint iNumKeyFrame = max(NumScale, NumRot);
            iNumKeyFrame = max(iNumKeyFrame, NumPos);

            fAniout.write((char*)&iNumKeyFrame, sizeof(_uint)); //    채널 위치 갯수 저장


            _float3 scale{};
            _float4 rotation{};
            _float3 pos{};

            for (size_t k = 0; k < iNumKeyFrame; k++)
            {
                KEYFRAME keyframe{};
                if (NumScale > k)
                {
                    memcpy(&scale, &pAnimation->mChannels[j]->mScalingKeys[k].mValue, sizeof(_float3));
                    keyframe.fTrackPosition = pAnimation->mChannels[j]->mScalingKeys[k].mTime;
                }
                
                if (NumRot > k)
                {
                    rotation.x = pAnimation->mChannels[j]->mRotationKeys[k].mValue.x;
                    rotation.y = pAnimation->mChannels[j]->mRotationKeys[k].mValue.y;
                    rotation.z = pAnimation->mChannels[j]->mRotationKeys[k].mValue.z;
                    rotation.w = pAnimation->mChannels[j]->mRotationKeys[k].mValue.w;
                    keyframe.fTrackPosition = pAnimation->mChannels[j]->mRotationKeys[k].mTime;
                }

                if (NumPos > k)
                {
                    memcpy(&pos, &pAnimation->mChannels[j]->mPositionKeys[k].mValue, sizeof(_float3));
                    keyframe.fTrackPosition = pAnimation->mChannels[j]->mPositionKeys[k].mTime;

                }

                keyframe.vScale = scale;
                keyframe.vRotation = rotation;
                keyframe.vTranslation = pos;

                fAniout.write((char*)&keyframe, sizeof(KEYFRAME));
            }


        }
       


        fAniout.close(); // 파일 문 닫기!
    }

   

	return S_OK;
}

HRESULT CModelConverter::Ready_StaticBinary(_uint numMeshs, const aiScene* m_pAIScene, const string& name, _fmatrix PreTransformMatrix)
{
    
    string strFileName = ("../Bin/Resources/Binary/Static/");
    string strFullname = strFileName + name;

    ofstream fout(strFullname.c_str(), ios::out | ios::binary);
    fout.write((char*)&numMeshs, sizeof(int));

    for (size_t i = 0; i < numMeshs; i++)
    {

        int iNumVerts = m_pAIScene->mMeshes[i]->mNumVertices;
        int iNumIndices = m_pAIScene->mMeshes[i]->mNumFaces * 3; // 면 1개당 인덱스 3개
        int iMaterialIndex = m_pAIScene->mMeshes[i]->mMaterialIndex;

        // 2. 파일에 이 번호표를 구워줍니다! (보통 메쉬 이름이나 정점 개수 구울 때 같이 굽습니다)
       

        VTXMESH* pVertices = new VTXMESH[iNumVerts];

        for (int j = 0; j < iNumVerts; ++j)
        {
            memcpy(&pVertices[j].vPosition, &m_pAIScene->mMeshes[i]->mVertices[j], sizeof(_float3));
            XMStoreFloat3(&pVertices[j].vPosition,
                XMVector3TransformCoord(XMLoadFloat3(&pVertices[j].vPosition), PreTransformMatrix));

            memcpy(&pVertices[j].vNormal, &m_pAIScene->mMeshes[i]->mNormals[j], sizeof(_float3));
            XMStoreFloat3(&pVertices[j].vNormal,
                XMVector3TransformCoord(XMLoadFloat3(&pVertices[j].vNormal), PreTransformMatrix));

            memcpy(&pVertices[j].vTangent, &m_pAIScene->mMeshes[i]->mTangents[j], sizeof(_float3));
            XMStoreFloat3(&pVertices[j].vTangent,
                XMVector3TransformNormal(XMLoadFloat3(&pVertices[j].vTangent), PreTransformMatrix));

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

        fout.write(reinterpret_cast<const char*>(&iMaterialIndex), sizeof(int));
        // 알아낸 개수를 내 커스텀 바이너리 파일(.bin)에 기록합니다.
        fout.write((char*)&iNumVerts, sizeof(int));
        fout.write((char*)&iNumIndices, sizeof(int));
        fout.write((char*)pVertices, sizeof(VTXMESH) * iNumVerts);
        fout.write((char*)pIndices, sizeof(_ulong) * iNumIndices);


        delete[] pVertices;
        delete[] pIndices;
    }


    // ====================================================================
    // [1] 머테리얼(옷장) 정보 굽기 시작!
    // ====================================================================

    // 1. 이 모델이 가진 전체 옷(머테리얼)의 개수를 구웁니다.
    int iNumMaterials = m_pAIScene->mNumMaterials;
    fout.write(reinterpret_cast<const char*>(&iNumMaterials), sizeof(int));

    // 2. 옷의 개수만큼 반복하면서 텍스처 이름을 하나씩 꺼내옵니다.
    for (int i = 0; i < iNumMaterials; ++i)
    {
        aiMaterial* pMaterial = m_pAIScene->mMaterials[i];
        aiString aiTexName;

        

        // Assimp에게 "이 옷의 기본 텍스처 이름 좀 줘!" 라고 물어봅니다.
        if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexName))
        {
            // Assimp가 준 이름을 깔끔한 C++ string으로 받습니다. (예: "fiona_D.png")
            string strTextureName = aiTexName.data;

            // 3.  글자 수(길이)를 먼저 구웁니다! (택배 상자 크기 예고)
            int iLength = strTextureName.length();
            fout.write(reinterpret_cast<const char*>(&iLength), sizeof(int));

            // 4.  진짜 글자들을 구웁니다!
            fout.write(strTextureName.c_str(), iLength);
        }
        else
        {
            // 만약 텍스처 이미지가 없는 단색 재질이라면? 
            // "글자 수가 0이다!" 라고 구워서 빈 상자임을 표시합니다.
            int iZeroLength = 0;
            fout.write(reinterpret_cast<const char*>(&iZeroLength), sizeof(int));
        }
    }

    fout.close(); // 파일 문 닫기!

    return S_OK;
}

CModelConverter* CModelConverter::Create()
{
    return new CModelConverter();
}

void CModelConverter::Free()
{
    __super::Free();

}
