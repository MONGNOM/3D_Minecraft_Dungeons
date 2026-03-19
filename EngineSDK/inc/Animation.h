#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, class CModel* pModel);
	_bool Update_TransformationMatrices(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop);

private:
	_float		m_fDuration = {};		// ?? 애니메이션을 늦추는게 뭐야 
	_float		m_fTickPerSecond = {};  // 애니메이션 재생 속도?


	_float		m_fCurrentTrackPosition = {}; // 현재 재생 위치 -> 계속 위치를 저장

	_uint		m_iNumChannels = {};	// 현재 애니메이션에서 사용할 뼈의 갯수
	vector<class CChannel*>	m_Channels; // 현재 애니메이션에서 사용할 뼈들?
	vector<_uint>	m_CurrentKeyFrameIndices; // 현재 내가 있는 키프레임 번호

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, class CModel* pModel);
	CAnimation* Clone(); 
	virtual void Free() override;
};

NS_END