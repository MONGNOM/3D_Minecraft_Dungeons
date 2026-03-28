#pragma once

#include "Base.h"

/* 뼈하나의 정보. */
/* 부모 뼈의 정보와 현재 뼈의 상태행렬들을 가진다. */
/* assimp 내에서 이용되는 뼈 : aiNode, aiBone, aiNodeAnim */

NS_BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;
public:
	const _float4x4* Get_CombinedTransformationMatrixPtr() const {
		return &m_CombinedTransformationMatrix;
	}

	void Update_TransformationMatrix(_fmatrix TransformMatrix)
	{
		XMStoreFloat4x4(&m_TransformationMatrix, TransformMatrix);
	}

public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentBoneIndex);
	HRESULT Binary_Initialize(const string& name, _int iParentBoneIndex, const _float4x4& localmatrix);
	_bool isCompare(const _char* pBoneName) {
		return !strcmp(pBoneName, m_szName);
	}
	const _char* Get_Name() { return m_szName; }
	_int Get_ParentIndex() { return m_iParentBoneIndex; }
	_float4x4 Get_TransformMatrix() { return m_TransformationMatrix; }

	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);



private:
	_char					m_szName[MAX_PATH] = {};
	_float4x4				m_TransformationMatrix = {};
	_float4x4				m_CombinedTransformationMatrix = {};
	_int					m_iParentBoneIndex = { -1 };

public:
	static CBone* Create(const aiNode* pAINode, _int iParentBoneIndex);
	static CBone* Create(const string& name, _int iParentBoneIndex, const _float4x4& localmatrix);
	CBone* Clone();
	virtual void Free() override;
};

NS_END