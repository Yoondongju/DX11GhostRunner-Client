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
		*pCurrentKeyFrameIndex = 0;					// *pCurrentKeyFrameIndex �̳༮�� ���� �׾ִϸ��̼��� Ű �������� ��� ����Ű��?�ϴ� �ε���

	KEYFRAME	LastKeyFrame = m_KeyFrames.back();		// ���� �� Ű ������

	_vector		vScale, vRotation, vTranslation;

	if (isTransitioning)
	{
		// ���� �ִϸ��̼���->����ä����->Ű ������?

		const vector<KEYFRAME>& TargetKeyFrames = pNextAniChannel->Get_KeyFrames();

		// ���� �ִϸ��̼� Ű������
		_vector vCurrentScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
		_vector vCurrentRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
		_vector vCurrentTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), 1.f);

		// ��ǥ �ִϸ��̼� Ű������
		_vector vTargetScale = XMLoadFloat3(&TargetKeyFrames[0].vScale);
		_vector vTargetRotation = XMLoadFloat4(&TargetKeyFrames[0].vRotation);
		_vector vTargetTranslation = XMVectorSetW(XMLoadFloat3(&TargetKeyFrames[0].vTranslation), 1.f);



		//if (CurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].TrackPosition)
		//	++* pCurrentKeyFrameIndex;


		//0~1    1�ʿ� 1�̴ϱ� 
		//�� �����ӿ��� 

		if (m_fTransitionTime < 1.f)
			m_fTransitionTime += (fTimeDelta / TransitionTime);
		else
		{
			m_fTransitionTime = 1.f;
			*pCurrentTrackPosition = 0.f;
		}

		// ���� ���
		vScale = XMVectorLerp(vCurrentScale, vTargetScale, m_fTransitionTime);
		vRotation = XMQuaternionSlerp(vCurrentRotation, vTargetRotation, m_fTransitionTime);
		vTranslation = XMVectorLerp(vCurrentTranslation, vTargetTranslation, m_fTransitionTime);

		if (*pCurrentTrackPosition == 0.f)
			m_fTransitionTime = 0.f;
	}


	else
	{
		/* ������ �ʿ���� ��� */
		if (*pCurrentTrackPosition >= LastKeyFrame.TrackPosition)
		{
			vScale = XMLoadFloat3(&LastKeyFrame.vScale);
			vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
			vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
		}

		/* ������ �ʿ��� ��κ� ��� */
		else
		{
			// *pCurrentKeyFrameIndex�� �� ������ Ű�����ӿ� �ش��Ѵ�.
			if (*pCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].TrackPosition)
				++* pCurrentKeyFrameIndex;

			_vector		vSourScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
			_vector		vDestScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale);

			_vector		vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
			_vector		vDestRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation);

			_vector		vSourTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), 1.f);
			_vector		vDestTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation), 1.f);

			/* �װ� ���� �����Ѱ�. */
			_double		Ratio = (*pCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition) / (m_KeyFrames[*pCurrentKeyFrameIndex + 1].TrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].TrackPosition);

			vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
			vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, (_float)Ratio);
		}
	}



	/* �� ������ ������� = ȸ�������� �翬�� �θ����.(�ڽĻ����� ���¸� ���鶧 ������������ �����ؾ��Ѵ�.) */

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
