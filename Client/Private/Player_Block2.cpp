#include "stdafx.h"
#include "Player_Block2.h"

#include "Player.h"
#include "GameInstance.h"

#include "Animation.h"

CPlayer_Block2::CPlayer_Block2(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::BLOCK_R2 , pOwner }
{

}

HRESULT CPlayer_Block2::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Block2::Start_State()
{


	return S_OK;
}

void CPlayer_Block2::Update(_float fTimeDelta)
{
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::BLOCK_R2 == pModel->Get_CurAnimationIndex())
	{
		_double Duration = pModel->Get_CurAnimation()->Get_Duration();
		const _double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();

		if (0.8f <= (TrackPos / Duration))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();

			_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
			TrackPos = 0.0;

			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
		}
	}
}

void CPlayer_Block2::End_State()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	pPlayer->Set_StartCountBlockTime(false);
	pPlayer->Set_BlockRemainingTime(0.f);
}


CPlayer_Block2* CPlayer_Block2::Create(class CGameObject* pOwner)
{
	CPlayer_Block2* pInstance = new CPlayer_Block2(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Block2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Block2::Free()
{
	__super::Free();
}
