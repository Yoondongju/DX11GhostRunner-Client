#include "stdafx.h"
#include "Player_Sh_Dash.h"

#include "Player.h"
#include "Body_Player.h"

#include "GameInstance.h"

CPlayer_Sh_Dash::CPlayer_Sh_Dash(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::SH_DASH_F , pOwner }		// 이거 고민이다.. 일단그냥 제일 최상위의 애니메이션 인덱스에 넣어야할듯?
{

}

HRESULT CPlayer_Sh_Dash::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Sh_Dash::Start_State(void* pArg)
{
	CTransform* pTransform = m_pOwner->Get_Transform();
	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();

	pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_ZeroTimer();
	pRigidBody->Set_IsGravity(false);

	switch (m_eDir)
	{
	case Engine::CState::FRONT:
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_LOOK), 200, Engine::CRigidBody::ACCELERATION);
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_LOOK), 450, Engine::CRigidBody::VELOCITYCHANGE);
		break;
	case Engine::CState::BACK:
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_LOOK), -200, Engine::CRigidBody::ACCELERATION);
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_LOOK), -450, Engine::CRigidBody::VELOCITYCHANGE);
		break;
	case Engine::CState::LEFT:
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_RIGHT), -200, Engine::CRigidBody::ACCELERATION);
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_RIGHT), -450, Engine::CRigidBody::VELOCITYCHANGE);
		break;
	case Engine::CState::RIGHT:
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_RIGHT), 200, Engine::CRigidBody::ACCELERATION);
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_RIGHT), 450, Engine::CRigidBody::VELOCITYCHANGE);
		break;
	case Engine::CState::STATE_DIR_END:
		break;
	default:
		break;
	}



	return S_OK;
}

void CPlayer_Sh_Dash::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;

	if (m_fAccTime >= 0.5f)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_IDLE, true);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);


		m_fAccTime = 0.f;
	}

	if (Check_Jump())
		return;

}

void CPlayer_Sh_Dash::End_State()
{
	m_fAccTime = 0.f;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	pPlayer->Set_StartCountDashTime(false);
	pPlayer->Set_DashRemainingTime(0.f);

	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();
	pRigidBody->Set_IsGravity(true);
}



_bool CPlayer_Sh_Dash::Check_Jump()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP)
	{
		CTransform* pTransform = m_pOwner->Get_Transform();
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

		_float fLandY = pPlayer->Get_LandPosY();
		_float fOffSetY = pPlayer->Get_OffsetY();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

		if (0.1f >= fabs((fLandY + fOffSetY) - XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION))))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();
			CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::JUMP_START);
		}

		return true;
	}

	return false;
}

CPlayer_Sh_Dash* CPlayer_Sh_Dash::Create(class CGameObject* pOwner)
{
	CPlayer_Sh_Dash* pInstance = new CPlayer_Sh_Dash(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Sh_Dash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Sh_Dash::Free()
{
	__super::Free();
}
