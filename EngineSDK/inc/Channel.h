#pragma once

#include "Base.h"

/* 특정 애니메이션 내에서 사용되는 뼈의 상태행렬정보들을 가지는 객체 */


NS_BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, class CModel* pModel);
	void Update_TransformationMatrix(_uint* pCurrentKeyFrameIndex, _float fCurrentTrackPosition, const vector<class CBone*>& Bones);

private:
	vector<KEYFRAME>			m_KeyFrames; // 사실상 애니메이션의 사용되는 뼈의 상태 행렬
	_uint						m_iNumKeyFrames = {}; //애니메이션에 사용되는 뼈의 프레임 갯수
	
	_int						m_iBoneIndex = { -1 };

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, class CModel* pModel);
	virtual void Free() override;
};

NS_END