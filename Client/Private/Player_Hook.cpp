#include "stdafx.h"
#include "Player_Hook.h"

#include "Player.h"
#include "Body_Player.h"

#include "GameInstance.h"

CPlayer_Hook::CPlayer_Hook(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::HOOK_UP , pOwner }
{

}

HRESULT CPlayer_Hook::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Hook::Start_State()
{
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	CTransform* pTransform = m_pOwner->Get_Transform();
	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();

	pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_ZeroTimer();


	
	if (CPlayer::PLAYER_ANIMATIONID::HOOK_UP == pModel->Get_NextAnimationIndex())
	{
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_LOOK), 220, Engine::CRigidBody::ACCELERATION);
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_LOOK), 300, Engine::CRigidBody::VELOCITYCHANGE);
	}


	return S_OK;
}

void CPlayer_Hook::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;

	if (m_fAccTime >= 0.5f)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);	
	}
	
	if (m_fAccTime >= 0.3f && m_pGameInstance->Get_KeyState(KEY::F) == KEY_STATE::TAP)	// µô·¹ÀÌ 0.3ÃÊ
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
	
		if (CPlayer::PLAYER_ANIMATIONID::HOOK_UP == pModel->Get_CurAnimationIndex())
		{
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::HOOK_DOWN, false, CPlayer::PLAYER_ANIMATIONID::IDLE);
			
			if(CPlayer::PLAYER_ANIMATIONID::HOOK_DOWN == pModel->Get_CurAnimationIndex())
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::HOOK_UP);		
		}	
	}
}

void CPlayer_Hook::End_State()
{
	m_fAccTime = 0.f;
}


CPlayer_Hook* CPlayer_Hook::Create(class CGameObject* pOwner)
{
	CPlayer_Hook* pInstance = new CPlayer_Hook(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Hook"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Hook::Free()
{
	__super::Free();
}
