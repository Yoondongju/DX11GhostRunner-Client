#pragma once

#include "Base.h"

BEGIN(Engine)

/* 애님에시녀을 구동을 위해 사용하는 뼈 중 하나 */
/* 시간에 따른 상태 정보를 가지고 있는다. 초기 생성시 모든 시간에 따른 상태값들을 미리 저장해둔다. */
/* 추후 런타임간에 이 채널에게 현재 애니메이션의 재생 위치를 전달해주면 재생위치에 맞는 상태행렬(m_TransformatoinMatrix )을 만든다. */

/* KEYFRAME : 특정 재생위치에서 이 뼈가 취해야할 상태(행렬)를 분해(스자이)하여 표현한 구조체.*/

class CChannel final : public CBase
{
public:
	typedef struct CHANNEL_DESC
	{
		_char			pName[MAX_PATH] = { "" };
		_uint			iBoneIndex = {};
		_uint			iNumKeyFrames = {};

		vector<KEYFRAME> vecKeyFrames;

	}CHANNEL_DESC;

private:
	CChannel();
	virtual ~CChannel() = default;

public:
	_char* Get_Name() { return m_szName; }
	_uint  Get_BoneIndex() { return m_iBoneIndex; }
	_uint  Get_NumKeyFrames() { return m_iNumKeyFrames; }

	vector<KEYFRAME> Get_KeyFrames() { return m_KeyFrames; }

public:
	HRESULT Initialize(CHANNEL_DESC* pDesc, const class CModel* pModel);
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, CChannel* pNextAniChannel, _double* pCurrentTrackPosition, _bool isTransitioning, _float fTimeDelta, _double TransitionTime);

private:
	_char				m_szName[MAX_PATH] = {};				// 이 채널의 이름

	_uint				m_iBoneIndex = { 0 };					// 이 채널의 정보가 갱신될때 실제 어떤 뼈의 행렬을 바꿔줄까? 어떤 뼈에 대한 인덱스
	
	_uint				m_iNumKeyFrames = {};					// 내 애니메이션의  키  프레임 갯수

	vector<KEYFRAME>	m_KeyFrames;							// 내 키 프레임에 대한 정보를 담아두는 벡터

	_float				m_fTransitionTime = { 0.f };			// 애니메이션 변화 시간

public:
	static CChannel* Create(CHANNEL_DESC* pDesc, const class CModel* pModel);
	virtual void Free() override;
};

END