#include "stdafx.h"
#include "Player_Attack2.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"

CPlayer_Attack2::CPlayer_Attack2(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::ATTACK_2 , pOwner }
{

}

HRESULT CPlayer_Attack2::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Attack2::Start_State()
{


	return S_OK;
}

void CPlayer_Attack2::Update(_float fTimeDelta)
{
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::ATTACK_2 != pModel->Get_NextAnimationIndex())
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
	}
}

void CPlayer_Attack2::End_State()
{

}


CPlayer_Attack2* CPlayer_Attack2::Create(class CGameObject* pOwner)
{
	CPlayer_Attack2* pInstance = new CPlayer_Attack2(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Attack2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Attack2::Free()
{
	__super::Free();
}
