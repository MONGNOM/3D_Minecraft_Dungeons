#include "Animation.h"

#include "Channel.h"

CAnimation::CAnimation()
{

}

CAnimation::CAnimation(const CAnimation& Prototype)
	: m_fDuration{ Prototype.m_fDuration }
	, m_fTickPerSecond{ Prototype.m_fTickPerSecond }
	, m_fCurrentTrackPosition{ Prototype.m_fCurrentTrackPosition }
	, m_iNumChannels{ Prototype.m_iNumChannels }
	, m_Channels{ Prototype.m_Channels }
	, m_CurrentKeyFrameIndices{ Prototype.m_CurrentKeyFrameIndices }
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, class CModel* pModel)
{
	m_fDuration = pAIAnimation->mDuration; // 애니메이션 늦추는거?
	m_fTickPerSecond = pAIAnimation->mTicksPerSecond; // 하나의 객체의 애니메이션의 관련된 속도

	/* 이 애니메이션을 재생하는데 필요한 뼈의 갯수 .*/
	m_iNumChannels = pAIAnimation->mNumChannels; // 현재 애니메이션을 재생할 때 필요한 뼈의 수

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++) // 재생할 때 필요한 뼈의 수 만큼 반복
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel); // 애니메이션에 사용되는 뼈들을 생성..? 근데 이미 뼈는 존재하지않나
		if (nullptr == pChannel) // 근데 그러면 모든뼈들을 생성했고 채널을 또 만드는거라면 뼈가 이중적으로 몇개나 생성되는거아닌가?
			return E_FAIL;

		m_Channels.push_back(pChannel);	// 애니메이션의 필요한 뼈들을 담았다
	}

	return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop)
{
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta; // 현재 재생시간을 누적했다면 초당 재생속도 * timedelta

	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		if (false == isLoop)
			return true;

		m_fCurrentTrackPosition = 0.f;		 // 애니메이션 반복 재생
	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Update_TransformationMatrix(&m_CurrentKeyFrameIndices[i], m_fCurrentTrackPosition, Bones);

	}
	

	return false; // 애니메이션이 끝나지 않았어를 반환
}


CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, class CModel* pModel)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}


void CAnimation::Free()
{
	__super::Free();

	for (auto& mChannel : m_Channels)
		Safe_Release(mChannel);

	m_Channels.clear();

}
