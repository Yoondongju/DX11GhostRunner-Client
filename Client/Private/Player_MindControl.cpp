#include "stdafx.h"
#include "Player_MindControl.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"

CPlayer_MindControl::CPlayer_MindControl(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::MIND_CONTROL_START_START , pOwner }
{

}

HRESULT CPlayer_MindControl::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_MindControl::Start_State()
{


	return S_OK;
}

void CPlayer_MindControl::Update(_float fTimeDelta)
{
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::NAMI_AIM_ATTACK_TO_IDLE != pModel->Get_NextAnimationIndex())
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
	}
}

void CPlayer_MindControl::End_State()
{

}


CPlayer_MindControl* CPlayer_MindControl::Create(class CGameObject* pOwner)
{
	CPlayer_MindControl* pInstance = new CPlayer_MindControl(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_MindControl"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_MindControl::Free()
{
	__super::Free();
}
