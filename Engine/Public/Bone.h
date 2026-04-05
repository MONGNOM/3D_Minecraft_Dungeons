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


	void Set_CurrentSRT(_fvector vScale, _fvector vRot, _fvector vTrans) {
		XMStoreFloat3(&m_vCurrentScale, vScale);
		XMStoreFloat4(&m_vCurrentRot, vRot);
		XMStoreFloat3(&m_vCurrentTrans, vTrans);
	}

	// 2.  애니메이션이 바뀔 때, 현재 상태를 사진첩(Blend)으로 통째로 복사!
	void Snapshot_For_Blending() {
		m_vBlendScale = m_vCurrentScale;
		m_vBlendRotation = m_vCurrentRot;
		m_vBlendTranslation = m_vCurrentTrans;
	}

	// 3. 사진첩 꺼내기 Getter (생략)
	_vector Get_BlendScale() { return XMLoadFloat3(&m_vBlendScale); }
	_vector Get_BlendRotation() { return XMLoadFloat4(&m_vBlendRotation); }
	_vector Get_BlendTranslation() { return XMLoadFloat3(&m_vBlendTranslation); }

	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);

	_float3 m_vBlendScale;
	_float4 m_vBlendRotation;
	_float3 m_vBlendTranslation;

	// 매 프레임 계산되는 '현재' 벡터 (사진 찍기 용도)
	_float3 m_vCurrentScale;
	_float4 m_vCurrentRot;
	_float3 m_vCurrentTrans;


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