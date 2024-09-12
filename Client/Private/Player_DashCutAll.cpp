#include "stdafx.h"
#include "Player_DashCutAll.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"

#include "Animation.h"

CPlayer_DashCutAll::CPlayer_DashCutAll(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::FURR_DASH_TRICUT , pOwner }
{

}

HRESULT CPlayer_DashCutAll::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_DashCutAll::Start_State()
{
	CTransform* pTransform = m_pOwner->Get_Transform();
	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();

	pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_ZeroTimer();



	pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_LOOK), 140, Engine::CRigidBody::ACCELERATION);
	pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_LOOK), 60, Engine::CRigidBody::VELOCITYCHANGE);

	return S_OK;
}

void CPlayer_DashCutAll::Update(_float fTimeDelta)
{
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


	if (CPlayer::PLAYER_ANIMATIONID::FURR_DASH_TRICUT == pModel->Get_CurAnimationIndex())
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

void CPlayer_DashCutAll::End_State()
{
	m_fAccTime = 0.f;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	pPlayer->Set_StartCountCutAllTime(false);
	pPlayer->Set_CutAllRemainingTime(0.f);
}


CPlayer_DashCutAll* CPlayer_DashCutAll::Create(class CGameObject* pOwner)
{
	CPlayer_DashCutAll* pInstance = new CPlayer_DashCutAll(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_DashCutAll"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_DashCutAll::Free()
{
	__super::Free();
}
