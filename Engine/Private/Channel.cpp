#include "..\Public\Channel.h"
#include "Bone.h"
#include "Model.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(CHANNEL_DESC* pDesc, const CModel* pModel)
{
	strcpy_s(m_szName, pDesc->pName);

	m_iBoneIndex = pModel->Get_BoneIndex(m_szName);

	m_iNumKeyFrames = pDesc->iNumKeyFrames;

	m_KeyFrames.reserve(m_iNumKeyFrames);


	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		m_KeyFrames.emplace_back(pDesc->vecKeyFrames[i]);
	}


	return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, CChannel* pNextAniChannel, _double* pCurrentTrackPosition, _bool isTransitioning, _float fTimeDelta, _double TransitionTime)
{
	if (0.0 == *pCurrentTrackPosition)
		*pCurrentKeyFrameIndex = 0;					// *pCurrentKeyFrameIndex 이녀석은 실제 그애니메이션의 키 프레임중 어디를 가르키니?하는 인덱스

	KEYFRAME	LastKeyFrame = m_KeyFrames.back();		// 제일 뒤 키 프레임

	_vector		vScale, vRotation, vTranslation;

	if (isTransitioning)
	{
		// 다음 애니메이션의->다음채널의->키 프레임?

		const vector<KEYFRAME>& TargetKeyFrames = pNextAniChannel->Get_KeyFrames();

		// 현재 애니메이션 키프레임
		_vector vCurrentScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
		_vector vCurrentRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
		_vector vCurrentTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), 1.f);

		// 목표 애니메이션 키프레임
		_vector vTargetScale = XMLoadFloat3(&TargetKeyFrames[0].vScale);
		_vector vTargetRotation = XMLoadFloat4(&TargetKeyFrames[0].vRotation);
		_vector vTargetTranslation = XMVectorSetW(XMLoadFloat3(&TargetKeyFrames[0].vTranslation), 1.f);



		//if (CurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].TrackPosition)
		//	++* pCurrentKeyFrameIndex;


		//0~1    1초에 1이니까 
		//매 프레임에는 

		if (m_fTransitionTime < 1.f)
			m_fTransitionTime += (fTimeDelta / TransitionTime);
		else
		{
			m_fTransitionTime = 1.f;
			*pCurrentTrackPosition = 0.f;
		}

		// 보간 계산
		vScale = XMVectorLerp(vCurrentScale, vTargetScale, m_fTransitionTime);
		vRotation = XMQuaternionSlerp(vCurrentRotation, vTargetRotation, m_fTransitionTime);
		vTranslation = XMVectorLerp(vCurrentTranslation, vTargetTranslation, m_fTransitionTime);

		if (*pCurrentTrackPosition == 0.f)
			m_fTransitionTime = 0.f;
	}


	else
	{
		/* 보간이 필요없는 경우 */
		if (*pCurrentTrackPosition >= LastKeyFrame.TrackPosition)
		{
			vScale = XMLoadFloat3(&LastKeyFrame.vScale);
			vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
			vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
		}

		/* 보간이 필요한 대부분 경우 */
		else
		{
			// *pCurrentKeyFrameIndex는 내 왼쪽의 키프레임에 해당한다.
			if (*pCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].TrackPosition)
				++* pCurrentKeyFrameIndex;

			_vector		vSourScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
			_vector		vDestScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale);

			_vector		vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
			_vector		vDestRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation);

			_vector		vSourTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), 1.f);
			_vector		vDestTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation), 1.f);

			/* 그거 내가 설명한거. */
			_double		Ratio = (*pCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition) / (m_KeyFrames[*pCurrentKeyFrameIndex + 1].TrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition);

			vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
			vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, (_float)Ratio);
		}
	}



	/* 이 뼈만의 상태행렬 = 회전기준은 당연히 부모기준.(자식뼈만의 상태를 만들때 원점기준으로 정의해야한다.) */

	_matrix			TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(CHANNEL_DESC* pDesc, const CModel* pModel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pDesc, pModel)))
	{
		MSG_BOX(TEXT("Failed to Created : CChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CChannel::Free()
{
	__super::Free();

	m_KeyFrames.clear();
}
