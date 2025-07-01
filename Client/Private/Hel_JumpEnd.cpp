#include "stdafx.h"
#include "Hel_JumpEnd.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

#include "Player.h"

CHel_JumpEnd::CHel_JumpEnd(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::JUMP_TO_IDLE , pOwner }
{

}

HRESULT CHel_JumpEnd::Initialize()
{
	return S_OK;
}

HRESULT CHel_JumpEnd::Start_State(void* pArg)
{
	// 나타난다는 효과주고

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
	_float fOffSetY = pPlayer->Get_OffsetY();


	CTransform* pPlayerTransform = pPlayer->Get_Transform();

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerLookNor = XMVector3Normalize(pPlayerTransform->Get_State(CTransform::STATE_LOOK));

	

	CTransform* pHelTransform = m_pOwner->Get_Transform();
	_vector vHelPos = pHelTransform->Get_State(CTransform::STATE_POSITION);

	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();


	_vector vTargetPos = {};
	vTargetPos = vPlayerPos + (vPlayerLookNor * 40.f);
	vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vPlayerPos) + 1000.f);

	pHelTransform->Set_State(CTransform::STATE_POSITION, vTargetPos);



	pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_ZeroTimer();


	pRigidBody->Add_Force_Direction(XMVectorSet(0.f, 1.f, 0.f, 0.f), -800, Engine::CRigidBody::ACCELERATION);
	pRigidBody->Add_Force_Direction(XMVectorSet(0.f, 1.f, 0.f, 0.f), -300, Engine::CRigidBody::VELOCITYCHANGE);

	CHel* pHel = static_cast<CHel*>(m_pOwner);

	CModel* pModel = pHel->Get_Model();
	_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
	TrackPos = 0.0;

	return S_OK;
}


void CHel_JumpEnd::Update(_float fTimeDelta)
{
	CHel* pHel = static_cast<CHel*>(m_pOwner);

	CModel* pModel = pHel->Get_Model();
	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();


	if (false == m_isFirstParticleActive && 0.2f < (_float)TrackPos / Duration)
	{
		CHel* pHel = static_cast<CHel*>(m_pOwner);
		pHel->Get_Part(CHel::PARTID::PART_PARTICLE_JUMPEND)->SetActiveMyParticle(true);

		m_isFirstParticleActive = true;

		if (false == m_pGameInstance->Check_IsPlaying(SOUND_HEL_JUMPEND))
			m_pGameInstance->Play_Sound(TEXT("Sprint.ogg"), SOUND_HEL_JUMPEND, 20.f);
	}

	if (0.9f < (_float)TrackPos / Duration)
	{
		CModel* pModel = m_pOwner->Get_Model();
		CFsm* pFsm = m_pOwner->Get_Fsm();

		CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();
		pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
		pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
		pRigidBody->Set_ZeroTimer();

		pModel->SetUp_Animation(CHel::HEL_ANIMATION::ATTACK1, true);
		pFsm->Change_State(CHel::HEL_ANIMATION::ATTACK1);
	}
}



void CHel_JumpEnd::End_State()
{
	m_isFirstParticleActive = false;

}


CHel_JumpEnd* CHel_JumpEnd::Create(class CGameObject* pOwner)
{
	CHel_JumpEnd* pInstance = new CHel_JumpEnd(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_JumpEnd"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_JumpEnd::Free()
{
	__super::Free();
}
