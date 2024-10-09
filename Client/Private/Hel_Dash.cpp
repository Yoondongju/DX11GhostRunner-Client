#include "stdafx.h"
#include "Hel_Dash.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

CHel_Dash::CHel_Dash(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::DASH_BACK , pOwner }
{

}

HRESULT CHel_Dash::Initialize()
{
	return S_OK;
}

HRESULT CHel_Dash::Start_State(void* pArg)
{
	CTransform* pHelTransform = m_pOwner->Get_Transform();
	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();

	pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_ZeroTimer();


	_vector vHelLookNor = XMVector3Normalize(pHelTransform->Get_State(CTransform::STATE_LOOK));
	pRigidBody->Add_Force_Direction(-vHelLookNor, 150, Engine::CRigidBody::ACCELERATION);
	pRigidBody->Add_Force_Direction(-vHelLookNor, 150, Engine::CRigidBody::VELOCITYCHANGE);


	CModel* pModel = static_cast<CHel*>(m_pOwner)->Get_Model();
	_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
	TrackPos = 0.0;


	return S_OK;
}


void CHel_Dash::Update(_float fTimeDelta)
{
	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();

	CModel* pModel = static_cast<CHel*>(m_pOwner)->Get_Model();
	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();

	if (0.9f < (_float)TrackPos / Duration)
	{
		CModel* pModel = m_pOwner->Get_Model();
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pModel->SetUp_Animation(CHel::HEL_ANIMATION::ATTACK2, true);
		pFsm->Change_State(CHel::HEL_ANIMATION::DASH_TO_IDLE_ATTACK);
	}
}



void CHel_Dash::End_State()
{

}


CHel_Dash* CHel_Dash::Create(class CGameObject* pOwner)
{
	CHel_Dash* pInstance = new CHel_Dash(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Dash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Dash::Free()
{
	__super::Free();
}
