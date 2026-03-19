#include "Mesh.h"

#include "GameInstance.h"
#include "Model.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{

}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CMesh::Initialize_Prototype(MODEL eType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PreTransformMatrix)
{
	m_iMaterialIndex = pAIMesh->mMaterialIndex;

	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iIndexStride = 4;
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	/* Vertex Buffer */
	HRESULT		hr = MODEL::NONANIM == eType ?
		Ready_VertexBuffer_For_NonAnim(pAIMesh, PreTransformMatrix) :
		Ready_VertexBuffer_For_Anim(pAIMesh, pModel);

	if (FAILED(hr))
		return E_FAIL;

	/* Index Buffer */
	D3D11_BUFFER_DESC	IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.StructureByteStride = m_iIndexStride;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;

	_uint	iNumIndices = {};

	_uint* pIndices = new _uint[m_iNumIndices];

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		aiFace	AIFace = pAIMesh->mFaces[i];

		pIndices[iNumIndices++] = AIFace.mIndices[0];
		pIndices[iNumIndices++] = AIFace.mIndices[1];
		pIndices[iNumIndices++] = AIFace.mIndices[2];
	}

	D3D11_SUBRESOURCE_DATA			IndexInitialData{};
	IndexInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
		return E_FAIL;


	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{

	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<CBone*>& Bones)
{
	ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * 512);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) *
			XMLoadFloat4x4(Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrixPtr()));
	}

	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{

	m_iVertexStride = sizeof(VTXMESH);
	D3D11_BUFFER_DESC	VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_iVertexStride;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent,
			XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
	}


	D3D11_SUBRESOURCE_DATA			VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_Anim(const aiMesh* pAIMesh, class CModel* pModel)
{
	m_iVertexStride = sizeof(VTXANIMMESH);
	D3D11_BUFFER_DESC	VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = m_iVertexStride;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
	}

	m_iNumBones = pAIMesh->mNumBones;

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		_int		iBoneIndex = pModel->Get_BoneIndex(pAIBone->mName.data);
		if (-1 == iBoneIndex)
			return E_FAIL;

		m_BoneIndices.push_back(iBoneIndex);

		_float4x4		OffsetMatrix = {};

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));


		m_OffsetMatrices.push_back(OffsetMatrix);


		/*pAIBone->mNumWeights : 이 뼈가 몇개의 정점에게 영향을 주는가? */

		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			/* 이 뼈가 j번째로 영향을 주는 정점의 정보? */
			aiVertexWeight	AIVertexWeight = pAIBone->mWeights[j];

			if (0 == pVertices[AIVertexWeight.mVertexId].vBlendWeight.x)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.x = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.x = AIVertexWeight.mWeight;
			}

			else if (0 == pVertices[AIVertexWeight.mVertexId].vBlendWeight.y)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.y = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.y = AIVertexWeight.mWeight;
			}

			else if (0 == pVertices[AIVertexWeight.mVertexId].vBlendWeight.z)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.z = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.z = AIVertexWeight.mWeight;
			}

			else if (0 == pVertices[AIVertexWeight.mVertexId].vBlendWeight.w)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.w = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.w = AIVertexWeight.mWeight;
			}
		}
	}

	D3D11_SUBRESOURCE_DATA			VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);


	return S_OK;
}



CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pAIMesh, pModel, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	return nullptr;
}

void CMesh::Free()
{
	__super::Free();
}
