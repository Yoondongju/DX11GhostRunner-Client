#pragma once

#include "Base.h"
#include "Channel.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
public:
	typedef struct ANIMATION_DESC
	{
		_uint		iNumChannels = {};

		_char		szName[MAX_PATH] = {};
		_double		Duration = {};
		_double		SpeedPerSec = {};

		vector<CChannel::CHANNEL_DESC>  vecChannelDesc;

		vector<ANIMATION_DESC>	vecAnimationDesc;

	}ANIMATION_DESC;


private:
	CAnimation();
	virtual ~CAnimation() = default;


public:
	vector<class CChannel*>& Get_Channels() { return m_Channels; }

	_uint					 Get_NumChannel() { return m_iNumChannels; }
	_double					 Get_Duration() { return m_Duration; }

	void					 Set_Duration(_double d) { m_Duration = d; }

	_double					 Get_OriginSpeedPerSec() { return m_OriginSpeedPerSec; }
	_double					 Get_SpeedPerSec() { return m_SpeedPerSec; }

	void					 Set_SpeedPerSec(_double SpeedPersec) { m_SpeedPerSec = SpeedPersec; }

public:
	void					 Set_NextAnimLerpDuration(_float fLerp) { m_fNextAnimLerpDuration = fLerp; }	// 다음애니메이션으로 가기위한 선보시간 기본 0.2f세팅


public:
	HRESULT Initialize(ANIMATION_DESC* pDesc, const class CModel* pModel);
	_bool Update_TransformationMatrices(_bool isFrustumCulling , const vector<class CBone*>& Bones, _double* pCurrentTrackPosition, vector<_uint>& CurrentKeyFrameIndices ,_bool isLoop, _float fTimeDelta, _bool isTransitioning, CAnimation* pNextAnimation);


private:
	_char						m_szName[MAX_PATH] = {};
	/* 이 애니메이션이 사용하는 뼈의 갯수 */
	_uint						m_iNumChannels = { 0 };
	/* 실제 뼈들의 정보*/
	/* Channel : 뼈 (시간에 따른 이 뼈의 상태 행렬을 보관한다) */
	vector<class CChannel*>		m_Channels;

	/* 이 애니메이션을 구동하기위해 거리는 전체 거리. */
	_double						m_Duration = {};

	/* 애니메이션의 초당 재생속도 */
	_double						m_OriginSpeedPerSec = {};
	_double						m_SpeedPerSec = {};


	_float						m_fNextAnimLerpDuration = { 0.2f };		// 공격같은애들은 다하고 돌아올때 선보시간을 더 줄여


public:
	static CAnimation* Create(ANIMATION_DESC* pDesc, const class CModel* pModel);
	virtual void Free() override;
};

END
