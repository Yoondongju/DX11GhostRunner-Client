#include "../Public/Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(ANIMATION_DESC* pDesc, const class CModel* pModel)
{
	strcpy_s(m_szName, pDesc->szName);

	m_Duration = pDesc->Duration;				// 이 애니메이션의 전체길이

	m_SpeedPerSec = pDesc->SpeedPerSec;			// 이 애니메이션의 초당 속도
	m_OriginSpeedPerSec = m_SpeedPerSec;

	/* 이 애니메이션이 사용하는 뼈의 갯수저장한다. */
	m_iNumChannels = pDesc->iNumChannels;		// 걷는 애니메이션이라 가정하면 이 애니메이션이 동작할때 영향을 받는 뼈의갯수

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(&pDesc->vecChannelDesc[i], pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(const vector<CBone*>& Bones, _double* pCurrentTrackPosition, vector<_uint>& CurrentKeyFrameIndices , _bool isLoop, _float fTimeDelta, _bool isTransitioning, CAnimation* pNextAnimation)
{
	/* 현재 재생위치를 계산하낟. */
	*pCurrentTrackPosition += m_SpeedPerSec * fTimeDelta;

	if (*pCurrentTrackPosition >= m_Duration)
	{
		*pCurrentTrackPosition = 0.0;

		if (false == isLoop)
			return true;
		
	}

	/* 현재 재생위치에 맞게 현재 애니메이션이 컨트롤해야 할 뼈의 상태들을 갱신해준다. */
	_uint		iChannelIndex = { 0 };

	vector<CChannel*>& NextAniChannels = pNextAnimation->Get_Channels();

	for (auto& pChannel : m_Channels)
	{												// 내 걷기 애니메이션에 사용하는 채널이 106개 잇꼬 거기서 0번째 채널
		// 마지막인자 : 몇초에 걸쳐 변환시키라
		pChannel->Update_TransformationMatrix(Bones, &CurrentKeyFrameIndices[iChannelIndex], NextAniChannels[iChannelIndex], pCurrentTrackPosition, isTransitioning, fTimeDelta, m_fNextAnimLerpDuration);
		iChannelIndex++;
	}

	return false;
}

CAnimation* CAnimation::Create(ANIMATION_DESC* pDesc, const class CModel* pModel)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pDesc, pModel)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();
}
