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
	HRESULT Binary_Initialize(const string& strFilePath);
	_bool Update_TransformationMatrices(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop, _float ratio);
	_char		animationName[MAX_PATH];

	_float Get_Duration() { return m_fDuration; }
	_float Get_TickPerSecond() { return m_fTickPerSecond; }
	vector<class CChannel*>& Get_CChannel() { return m_Channels; }
	_uint  Get_iNumChannels() { return m_iNumChannels; }

	_float Get_CurrentTrackPostion() { return m_fCurrentTrackPosition; }

	vector<_uint>& Get_CurrentKeyFrameIndices() { return m_CurrentKeyFrameIndices; }

	void Reset_TrackPosition() 
	{
		m_fCurrentTrackPosition = 0;

		for (size_t i = 0; i < m_iNumChannels; ++i)
		{
			m_CurrentKeyFrameIndices[i] = 0;
		}
	}

private:
	_float		m_fDuration = {};		// ?? 애니메이션을 늦추는게 뭐야 
	_float		m_fTickPerSecond = {};  // 애니메이션 재생 속도?

	

	_float		m_fCurrentTrackPosition = {}; // 현재 재생 위치 -> 계속 위치를 저장

	_uint		m_iNumChannels = {};	// 현재 애니메이션에서 사용할 뼈의 갯수
	vector<class CChannel*>	m_Channels; // 현재 애니메이션에서 사용할 뼈들?
	vector<_uint>	m_CurrentKeyFrameIndices; // 현재 내가 있는 키프레임 번호

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, class CModel* pModel);
	static CAnimation* Create(const string& strFilePath);
	CAnimation* Clone(); 
	virtual void Free() override;
};

NS_END