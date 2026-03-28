#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
protected:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	size_t Get_NumMeshes() {
		return m_Meshes.size();
	}

	vector<class CMaterial*> Get_m_Materials()
	{
		return m_Materials;
	}

	void Set_Animation(_uint iIndex, _bool isLoop)
	{
		m_iCurrentAnimIndex = iIndex;
		m_isAnimLoop = isLoop;
	}

	_int Get_BoneIndex(const _char* pBoneName);

	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName);

public:
	virtual HRESULT Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreLocalTransformMatrix, const string& name);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	_bool Play_Animation(_float fTimeDelta);

public:
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType, _uint iTextureIndex = 0);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Ready_Static_Model_Load(const string& strFilePath, const string& name);
	HRESULT Ready_Dynamic_Model_Load(const string& strFilePath, const string& name);
	HRESULT Ready_Animations(string strAniFileName); /* 각 뼈들이 시간에 따라서 어떤 상태를 띈다. */

private:
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer = {};

	MODEL						m_eType = { MODEL::END };
	_float4x4					m_PreLocalTransformMatrix = {};

private:
	_uint						m_iNumMeshes = {};
	vector<class CMesh*>		m_Meshes;

	_uint						m_iNumMaterials = {};
	vector<class CMaterial*>	m_Materials;

	vector<class CBone*>		m_Bones;

	_bool						m_isAnimLoop = { false };
	_uint						m_iCurrentAnimIndex = {}; // 현재 애니메이션 인덱스 -> 뼈들을 컨트롤
	_uint						m_iNumAnimations = {};
	vector<class CAnimation*>	m_Animations;

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex); /* 계층구조를 가진 뼈들을 생성한다. */
	HRESULT Ready_Animations(); /* 각 뼈들이 시간에 따라서 어떤 상태를 띈다. */

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, const string& name = nullptr, _fmatrix PreLocalTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END