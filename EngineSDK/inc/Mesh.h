#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(MODEL eType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT TestInitialize(MODEL eType, VTXMESH* pVertices, _uint iNumVertices, _ulong* pIndices, _uint iNumIndices, _uint iMaterialIndex, _fmatrix PreTransformMatrix);

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones);

private:
	_uint					m_iMaterialIndex = {};
	_uint					m_iNumBones = {}; /* 이 메시 덩어리가 사용해야할 뼈의 갯수 */
	vector<_uint>			m_BoneIndices;
	_float4x4				m_BoneMatrices[512] = {};
	vector<_float4x4>		m_OffsetMatrices;
	_uint					m_Indices;



private:
	HRESULT Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(const aiMesh* pAIMesh, class CModel* pModel);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PreTransformMatrix);
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, VTXMESH* pVertices, _uint iNumVertices, _ulong* pIndices, _uint iNumIndices, _uint iMaterialIndex, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;


};

NS_END