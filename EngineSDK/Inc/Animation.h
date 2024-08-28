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
	_double					 Get_SpeedPerSec() { return m_SpeedPerSec; }

	void					 Set_SpeedPerSec(_double SpeedPersec) { m_SpeedPerSec = SpeedPersec; }


public:
	HRESULT Initialize(ANIMATION_DESC* pDesc, const class CModel* pModel);
	_bool Update_TransformationMatrices(const vector<class CBone*>& Bones, _double* pCurrentTrackPosition, vector<_uint>& CurrentKeyFrameIndices ,_bool isLoop, _float fTimeDelta, _bool isTransitioning, CAnimation* pNextAnimation);


private:
	_char						m_szName[MAX_PATH] = {};
	/* �� �ִϸ��̼��� ����ϴ� ���� ���� */
	_uint						m_iNumChannels = { 0 };
	/* ���� ������ ����*/
	/* Channel : �� (�ð��� ���� �� ���� ���� ����� �����Ѵ�) */
	vector<class CChannel*>		m_Channels;

	/* �� �ִϸ��̼��� �����ϱ����� �Ÿ��� ��ü �Ÿ�. */
	_double						m_Duration = {};

	/* �ִϸ��̼��� �ʴ� ����ӵ� */
	_double						m_SpeedPerSec = {};




public:
	static CAnimation* Create(ANIMATION_DESC* pDesc, const class CModel* pModel);
	virtual void Free() override;
};

END
