#include "stdafx.h"
#include "Player_Dash.h"

#include "Player.h"
#include "Body_Player.h"

#include "GameInstance.h"

CPlayer_Dash::CPlayer_Dash(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::DASH_F , pOwner }		// 이거 고민이다.. 일단그냥 제일 최상위의 애니메이션 인덱스에 넣어야할듯?
{

}

HRESULT CPlayer_Dash::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Dash::Start_State()
{
	CTransform* pTransform = m_pOwner->Get_Transform();
	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();

	pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_ZeroTimer();

	switch (m_eDir)
	{
	case Engine::CState::FRONT:
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_LOOK), 70, Engine::CRigidBody::ACCELERATION);
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_LOOK), 150, Engine::CRigidBody::VELOCITYCHANGE);
		break;
	case Engine::CState::BACK:
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_LOOK), -70, Engine::CRigidBody::ACCELERATION);
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_LOOK), -150, Engine::CRigidBody::VELOCITYCHANGE);
		break;
	case Engine::CState::LEFT:
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_RIGHT), -70, Engine::CRigidBody::ACCELERATION);
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_RIGHT), -150, Engine::CRigidBody::VELOCITYCHANGE);
		break;
	case Engine::CState::RIGHT:
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_RIGHT), 70, Engine::CRigidBody::ACCELERATION);
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_RIGHT), 150, Engine::CRigidBody::VELOCITYCHANGE);
		break;
	case Engine::CState::STATE_DIR_END:
		break;
	default:
		break;
	}

	return S_OK;
}

void CPlayer_Dash::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;

	if (m_fAccTime >= 0.5f)
	{
		CFsm*         pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
 

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);

		m_fAccTime = 0.f;
	}

	Check_Jump();

}

void CPlayer_Dash::End_State()
{
	m_fAccTime = 0.f;
}


void CPlayer_Dash::Check_Jump()
{
	if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::JUMP_START, false, CPlayer::PLAYER_ANIMATIONID::JUMP_LOOP);	// Change_State보다 먼저 세팅해줘야함 Hook이나 Attack같은 같은 State를 공유하는녀석일 경우
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::JUMP_START);
	}
}

CPlayer_Dash* CPlayer_Dash::Create(class CGameObject* pOwner)
{
	CPlayer_Dash* pInstance = new CPlayer_Dash(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Dash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Dash::Free()
{
	__super::Free();
}
