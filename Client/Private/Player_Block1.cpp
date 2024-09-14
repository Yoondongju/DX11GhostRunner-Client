#include "stdafx.h"
#include "Player_Block1.h"

#include "Player.h"
#include "GameInstance.h"

#include "Animation.h"

CPlayer_Block1::CPlayer_Block1(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::BLOCK_R1 , pOwner }
{

}

HRESULT CPlayer_Block1::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Block1::Start_State()
{

	return S_OK;
}

void CPlayer_Block1::Update(_float fTimeDelta)
{
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::BLOCK_R1 == pModel->Get_CurAnimationIndex() ||
		CPlayer::PLAYER_ANIMATIONID::BLOCK_L1 == pModel->Get_CurAnimationIndex())
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

void CPlayer_Block1::End_State()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	pPlayer->Set_StartCountBlockTime(false);
	pPlayer->Set_BlockRemainingTime(0.f);
}


CPlayer_Block1* CPlayer_Block1::Create(class CGameObject* pOwner)
{
	CPlayer_Block1* pInstance = new CPlayer_Block1(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Block1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Block1::Free()
{
	__super::Free();
}
