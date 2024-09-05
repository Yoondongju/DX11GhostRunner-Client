#include "stdafx.h"
#include "Player_Attack1.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"

CPlayer_Attack1::CPlayer_Attack1(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::ATTACK_1 , pOwner }
{

}

HRESULT CPlayer_Attack1::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Attack1::Start_State()
{


	return S_OK;
}

void CPlayer_Attack1::Update(_float fTimeDelta)
{
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::ATTACK_1 != pModel->Get_NextAnimationIndex())
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
	}
}

void CPlayer_Attack1::End_State()
{

}


CPlayer_Attack1* CPlayer_Attack1::Create(class CGameObject* pOwner)
{
	CPlayer_Attack1* pInstance = new CPlayer_Attack1(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Attack1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Attack1::Free()
{
	__super::Free();
}
