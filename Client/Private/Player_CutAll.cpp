#include "stdafx.h"
#include "Player_CutAll.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"

#include "Animation.h"

CPlayer_CutAll::CPlayer_CutAll(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::FURR_AIM_TRICUT , pOwner }
{

}

HRESULT CPlayer_CutAll::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_CutAll::Start_State()
{


	return S_OK;
}

void CPlayer_CutAll::Update(_float fTimeDelta)
{
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


	if (CPlayer::PLAYER_ANIMATIONID::FURR_AIM_TRICUT == pModel->Get_CurAnimationIndex())
	{
		_double Duration = pModel->Get_CurAnimation()->Get_Duration();
		const _double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();

		if (0.8f <= (TrackPos / Duration))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();

			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
		}
	}
}

void CPlayer_CutAll::End_State()
{
	m_fAccTime = 0.f;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	pPlayer->Set_StartCountCutAllTime(false);
	pPlayer->Set_CutAllRemainingTime(0.f);
}


CPlayer_CutAll* CPlayer_CutAll::Create(class CGameObject* pOwner)
{
	CPlayer_CutAll* pInstance = new CPlayer_CutAll(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_CutAll"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_CutAll::Free()
{
	__super::Free();
}
