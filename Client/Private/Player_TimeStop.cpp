#include "stdafx.h"
#include "Player_TimeStop.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"

CPlayer_TimeStop::CPlayer_TimeStop(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::TIME_STOP , pOwner }
{

}

HRESULT CPlayer_TimeStop::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_TimeStop::Start_State()
{


	return S_OK;
}

void CPlayer_TimeStop::Update(_float fTimeDelta)
{
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::FURR_AIM_TO_IDLE != pModel->Get_NextAnimationIndex())
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
	}
}

void CPlayer_TimeStop::End_State()
{

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
