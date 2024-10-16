#include "stdafx.h"
#include "Hel_Jump.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

#include "PartObject.h"

CHel_Jump::CHel_Jump(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::IDLE_TO_JUMP , pOwner }
{

}

HRESULT CHel_Jump::Initialize()
{
	return S_OK;
}

HRESULT CHel_Jump::Start_State(void* pArg)
{
	CTransform* pHelTransform = m_pOwner->Get_Transform();
	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();

	pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_ZeroTimer();


	CHel* pHel = static_cast<CHel*>(m_pOwner);

	CModel* pModel = pHel->Get_Model();
	_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
	TrackPos = 0.0;

	return S_OK;
}


void CHel_Jump::Update(_float fTimeDelta)
{
	CModel* pModel = static_cast<CHel*>(m_pOwner)->Get_Model();
	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();


	if (true == m_isJumpStart)
	{
		CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();

		pRigidBody->Add_Force_Direction(XMVectorSet(0.f, 1.f, 0.f, 0.f), 1200, Engine::CRigidBody::ACCELERATION);
		pRigidBody->Add_Force_Direction(XMVectorSet(0.f, 1.f, 0.f, 0.f), 1000, Engine::CRigidBody::VELOCITYCHANGE);

		CHel* pHel = static_cast<CHel*>(m_pOwner);
		pHel->Get_Part(CHel::PARTID::PART_PARTICLE_JUMPSTART)->SetActiveMyParticle(true);

		m_isJumpStart = false;
	}


	if (false == m_isJumpExcute && 0.4f <= (_float)TrackPos / Duration)
	{
		m_isJumpStart = true;
		m_isJumpExcute = true;
	}

	

	if (0.9f < (_float)TrackPos / Duration)
	{
		CModel* pModel = m_pOwner->Get_Model();
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pModel->SetUp_Animation(CHel::HEL_ANIMATION::JUMP_TO_IDLE, true);
		pFsm->Change_State(CHel::HEL_ANIMATION::JUMP_TO_IDLE);
	}
}


void CHel_Jump::End_State()
{
	m_isJumpStart = false;
	m_isJumpExcute = false;
}


CHel_Jump* CHel_Jump::Create(class CGameObject* pOwner)
{
	CHel_Jump* pInstance = new CHel_Jump(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Jump"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Jump::Free()
{
	__super::Free();
}
