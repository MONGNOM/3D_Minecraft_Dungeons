#include "Channel.h"

#include "Model.h"
#include "Bone.h"
#include "fstream"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, class CModel* pModel)
{
	m_iBoneIndex = pModel->Get_BoneIndex(pAIChannel->mNodeName.data);

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	_float3		vScale{};
	_float4		vRotation{};
	_float3		vTranslation{};


	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame{};

		if (pAIChannel->mNumScalingKeys > i)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pAIChannel->mScalingKeys[i].mTime;
		}
		if (pAIChannel->mNumRotationKeys > i)
		{
			/*memcpy(&vRotation, &pAIChannel->mRotationKeys[i].mValue, sizeof(_float4));*/
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;

			KeyFrame.fTrackPosition = pAIChannel->mRotationKeys[i].mTime;
		}
		if (pAIChannel->mNumPositionKeys > i)
		{
			memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;

		m_KeyFrames.push_back(KeyFrame);
	}



	return S_OK;
}

HRESULT CChannel::Binary_Initialize(ifstream& fin)
{
	fin.read((char*)&m_iBoneIndex, sizeof(_int));
	fin.read((char*)&m_iNumKeyFrames, sizeof(_uint));


	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame{};
		fin.read((char*)&KeyFrame, sizeof(KEYFRAME));

		m_KeyFrames.push_back(KeyFrame);
	}




	return S_OK;
}

void CChannel::Update_TransformationMatrix(_uint* pCurrentKeyFrameIndex, _float fCurrentTrackPosition, const vector<class CBone*>& Bones)
{
	// 애니메이션 매쉬깨지는이유는 물어보자
	if (0.f == fCurrentTrackPosition) // 애니메이션 반복 하려고 키프레임 위치 조절
		(*pCurrentKeyFrameIndex) = 0;

	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	_vector		vScale, vRotation, vTranslation;


	if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition) /*  마지막 키프레임의 상태를 띈다. */
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);

	}
	else /* 사이 키프레임으로 보간한다. */
	{
		while (fCurrentTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition)
			++(*pCurrentKeyFrameIndex);

		_float		fRatio = (fCurrentTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition) /
			(m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTrackPosition); // 얘와 얘 사이의 거리를 구해서 얘와 얘사이 전체 거리를 봐서 나누라고 뭔소리야

		_vector		vLeftScale, vRightScale;
		_vector		vLeftRotation, vRightRotation;
		_vector		vLeftTranslation, vRightTranslation;

		vLeftScale = XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale);
		vRightScale = XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale);
		vScale = XMVectorLerp(vLeftScale, vRightScale, fRatio);

		vLeftRotation = XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation);
		vRightRotation = XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation);
		vRotation = XMQuaternionSlerp(vLeftRotation, vRightRotation, fRatio);

		vLeftTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation), 1.f);
		vRightTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation), 1.f);
		vTranslation = XMVectorLerp(vLeftTranslation, vRightTranslation, fRatio);
	}

	// _matrix		BoneTransformationMatrix = XMMatrixScaling(vScale) * XMMatrixRotationQuaternion(vRotation) * XMMatrixTranslation(vTranslation);
	_matrix		BoneTransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Update_TransformationMatrix(BoneTransformationMatrix);
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, class CModel* pModel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, pModel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CChannel* CChannel::Create(ifstream& fin)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Binary_Initialize(fin)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CChannel::Free()
{
	__super::Free();

	m_KeyFrames.clear();

}
