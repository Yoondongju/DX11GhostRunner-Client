#include "stdafx.h"
#include "Player_TimeStop.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
#include "Animation.h"

CPlayer_TimeStop::CPlayer_TimeStop(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::TIME_STOP , pOwner }
{

}

HRESULT CPlayer_TimeStop::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_TimeStop::Start_State(void* pArg)
{
	m_pGameInstance->ActiveRefraction(static_cast<CPlayer*>(m_pOwner)->Get_TimeStopRefractionTex() , CRenderer::REFRACTION_TYPE::TIMESTOP);

	return S_OK;
}

void CPlayer_TimeStop::Update(_float fTimeDelta)
{
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	// 시간이 느려지게 되면 



	if (CPlayer::PLAYER_ANIMATIONID::TIME_STOP == pModel->Get_CurAnimationIndex())
	{
		_double Duration = pModel->Get_CurAnimation()->Get_Duration();
		_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();
		

		if (0.1f <= TrackPos / (_float)Duration)
		{
			m_pGameInstance->Set_TimeDelayActive(true);
		}


		if (0.95f <= TrackPos / (_float)Duration)
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();

			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
		}
	}
}

void CPlayer_TimeStop::End_State()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	pPlayer->Set_StartCountTimeStopTime(false);
	pPlayer->Set_TimeStopRemainingTime(0.f);

	m_pGameInstance->Set_TimeDelayActive(false);

	m_pGameInstance->UnActiveRefraction();
}


CPlayer_TimeStop* CPlayer_TimeStop::Create(class CGameObject* pOwner)
{
	CPlayer_TimeStop* pInstance = new CPlayer_TimeStop(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_TimeStop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_TimeStop::Free()
{
	__super::Free();
}
