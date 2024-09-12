#include "stdafx.h"
#include "Player_Nami.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
#include "Animation.h"

CPlayer_Nami::CPlayer_Nami(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::NAMI_IDLE_TO_AIM , pOwner }
{

}

HRESULT CPlayer_Nami::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Nami::Start_State()
{


	return S_OK;
}

void CPlayer_Nami::Update(_float fTimeDelta)
{
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (m_pGameInstance->Get_KeyState(KEY::TWO) == KEY_STATE::AWAY)
	{
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::NAMI_AIM_ATTACK_TO_IDLE, false);
	}


	if (CPlayer::PLAYER_ANIMATIONID::NAMI_AIM_ATTACK_TO_IDLE == pModel->Get_CurAnimationIndex())
	{
		_double Duration = pModel->Get_CurAnimation()->Get_Duration();
		_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();

		if (0.9f <= TrackPos / (_float)Duration)
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();
			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
		}
	}
}

void CPlayer_Nami::End_State()
{

}


CPlayer_Nami* CPlayer_Nami::Create(class CGameObject* pOwner)
{
	CPlayer_Nami* pInstance = new CPlayer_Nami(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Nami"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Nami::Free()
{
	__super::Free();
}
