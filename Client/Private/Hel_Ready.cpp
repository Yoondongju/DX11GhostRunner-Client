#include "stdafx.h"
#include "Hel_Ready.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

#include "PartObject.h"

#include  "FreeCamera.h"

CHel_Ready::CHel_Ready(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::READY , pOwner }
{

}

HRESULT CHel_Ready::Initialize()
{
	return S_OK;
}

HRESULT CHel_Ready::Start_State(void* pArg)
{
	CModel* pModel = m_pOwner->Get_Model();
	_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
	TrackPos = 0.0;


	//CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();
	//pRigidBody->Add_Force_Direction(XMVectorSet(0.f, 1.f, 0.f, 0.f), 1200, Engine::CRigidBody::ACCELERATION);
	//pRigidBody->Add_Force_Direction(XMVectorSet(0.f, 1.f, 0.f, 0.f), 1000, Engine::CRigidBody::VELOCITYCHANGE);

	return S_OK;
}


void CHel_Ready::Update(_float fTimeDelta)
{
	CModel* pModel = m_pOwner->Get_Model();
	CFsm* pFsm = m_pOwner->Get_Fsm();
	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();

	if (0.98f < (_float)TrackPos / Duration)
	{
		CModel* pModel = m_pOwner->Get_Model();
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pModel->SetUp_Animation(CHel::HEL_ANIMATION::IDLE, true);
		pFsm->Change_State(CHel::HEL_ANIMATION::IDLE);

		CHel* pHel = static_cast<CHel*>(m_pOwner);
		pHel->Get_Part(CHel::PARTID::PART_PARTICLE_JUMPSTART)->SetActiveMyParticle(true);
	}
}


void CHel_Ready::End_State()
{
	CFreeCamera* pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY));
	CTransform* pCamTransform = pCamera->Get_Transform();
	pCamera->Set_FollowPlayer(true);
}


CHel_Ready* CHel_Ready::Create(class CGameObject* pOwner)
{
	CHel_Ready* pInstance = new CHel_Ready(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Ready"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Ready::Free()
{
	__super::Free();
}
