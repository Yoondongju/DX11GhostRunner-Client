#include "stdafx.h"
#include "Hel_DashAttack.h"

#include "Hel.h"
#include "Weapon_Hel.h"

#include "GameInstance.h"
#include "Animation.h"

#include "HelSwordTrail.h"
#include "Player.h"

#include "HelMotionTrail.h"

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


	CHel* pHel = static_cast<CHel*>(m_pOwner);
	CHelSwordTrail* pSwordTrail = static_cast<CWeapon_Hel*>(pHel->Get_Part(CHel::PARTID::PART_WEAPON))->Get_SwordTrail();
	pSwordTrail->Set_Active(false);
	pSwordTrail->Set_Active(true);


	if (true == pHel->IsPage2())
		pHel->Get_Part(CHel::PARTID::PART_PARTICLE_SWIRL)->SetActiveMyParticle(true);

	pHel->Set_ActiveParticleAttack(true);
	pHel->Set_ActiveParticleElectric(true);
	pHel->Get_MotionTrail()->Set_Active(true);

	m_pGameInstance->Play_Sound(TEXT("RunJump.ogg"), SOUND_HEL_MOVEMENT, 10.f);
	m_pGameInstance->Play_Sound(TEXT("Electric.ogg"), SOUND_HEL_ELECTRIC, 1.7f);

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

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
	_float fOffSetY = pPlayer->Get_OffsetY();
	_vector vPlayerPos = pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);


	_float fSpeed = 5.2f;
	if (true == pHel->IsPage2())
	{
		fSpeed *= 2.f;
		pHelTransform->LookAt_XZ(vPlayerPos);
	}
		

	_vector vHelPos = pHelTransform->Get_State(CTransform::STATE_POSITION);
	vPlayerPos.m128_f32[1] -= fOffSetY;
	_float fCurDistance = XMVectorGetX(XMVector3Length(vPlayerPos - vHelPos));


	if (40.f < fCurDistance)
	{
		pHelTransform->Go_Straight(fTimeDelta * fSpeed);
	}

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
	CHel* pHel = static_cast<CHel*>(m_pOwner);
	pHel->Set_ActiveParticleAttack(false);
	pHel->Set_ActiveParticleElectric(false);
	pHel->Get_MotionTrail()->Set_Active(false);
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
