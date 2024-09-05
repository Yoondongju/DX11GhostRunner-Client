#include "stdafx.h"
#include "Player_Attack3.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"

CPlayer_Attack3::CPlayer_Attack3(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::ATTACK_3 , pOwner }
{

}

HRESULT CPlayer_Attack3::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Attack3::Start_State()
{


	return S_OK;
}

void CPlayer_Attack3::Update(_float fTimeDelta)
{
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::ATTACK_3 != pModel->Get_NextAnimationIndex())
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
	}
}

void CPlayer_Attack3::End_State()
{

}


CPlayer_Attack3* CPlayer_Attack3::Create(class CGameObject* pOwner)
{
	CPlayer_Attack3* pInstance = new CPlayer_Attack3(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Attack3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Attack3::Free()
{
	__super::Free();
}
