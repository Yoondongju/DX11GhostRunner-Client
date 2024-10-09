#include "stdafx.h"
#include "Hel_DashAttack.h"

#include "Hel.h"
#include "Weapon_Hel.h"

#include "GameInstance.h"
#include "Animation.h"

CHel_DashAttack::CHel_DashAttack(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::DASH_TO_IDLE_ATTACK , pOwner }
{

}

HRESULT CHel_DashAttack::Initialize()
{
	return S_OK;
}

HRESULT CHel_DashAttack::Start_State(void* pArg)
{
	CTransform* pHelTransform = m_pOwner->Get_Transform();
	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();

	pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_ZeroTimer();



	CModel* pModel = static_cast<CHel*>(m_pOwner)->Get_Model();
	_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
	TrackPos = 0.0;

	return S_OK;
}


void CHel_DashAttack::Update(_float fTimeDelta)
{
	CHel* pHel = static_cast<CHel*>(m_pOwner);
	static_cast<CWeapon_Hel*>(pHel->Get_Part(CHel::PART_WEAPON))->Check_Collision();


	CModel* pModel = static_cast<CHel*>(m_pOwner)->Get_Model();
	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();

	CTransform* pHelTransform = m_pOwner->Get_Transform();
	pHelTransform->Go_Straight(fTimeDelta * 3.5f);

	if (0.9f < (_float)TrackPos / Duration)
	{
		CModel* pModel = m_pOwner->Get_Model();
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pModel->SetUp_Animation(CHel::HEL_ANIMATION::IDLE, true);
		pFsm->Change_State(CHel::HEL_ANIMATION::IDLE);
	}

}


void CHel_DashAttack::End_State()
{

}


CHel_DashAttack* CHel_DashAttack::Create(class CGameObject* pOwner)
{
	CHel_DashAttack* pInstance = new CHel_DashAttack(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_DashAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_DashAttack::Free()
{
	__super::Free();
}
