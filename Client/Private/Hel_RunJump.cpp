#include "stdafx.h"
#include "Hel_RunJump.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

#include "Player.h"

#include "HelMotionTrail.h"

CHel_RunJump::CHel_RunJump(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::RUN_JUMP , pOwner }
{

}

HRESULT CHel_RunJump::Initialize()
{
	return S_OK;
}

HRESULT CHel_RunJump::Start_State(void* pArg)
{
	m_fAttackRange = 40.f;


	CModel* pModel = static_cast<CHel*>(m_pOwner)->Get_Model();
	_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
	TrackPos = 0.0;


	CHel* pHel = static_cast<CHel*>(m_pOwner);
	if(true == pHel->IsPage2())
		pHel->Get_Part(CHel::PARTID::PART_PARTICLE_SWIRL)->SetActiveMyParticle(true);


	pHel->Get_MotionTrail()->Set_Active(true);

	m_pGameInstance->Play_Sound(TEXT("RunJump.ogg"), SOUND_HEL_MOVEMENT, 10.f);

	return S_OK;
}


void CHel_RunJump::Update(_float fTimeDelta)
{
	CTransform* pHelTransform = m_pOwner->Get_Transform();
	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
	_vector vPlayerPos = pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	pHelTransform->LookAt_XZ(vPlayerPos);

	CModel* pModel = static_cast<CHel*>(m_pOwner)->Get_Model();
	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();

	_float fRatio = (_float)TrackPos / Duration;


	_float fSpeed = 25.f;
	CHel* pHel = static_cast<CHel*>(m_pOwner);
	if (true == pHel->IsPage2())
		fSpeed = 30.f;

	pHelTransform->Go_Straight(fTimeDelta * fSpeed * fRatio);

	if (0.9f < fRatio)
	{
		// 그냥 기본공격, 뒤로 대쉬한번갈기고 대쉬공격, 
		CModel* pModel = m_pOwner->Get_Model();
		CFsm* pFsm = m_pOwner->Get_Fsm();


		pModel->SetUp_Animation(CHel::HEL_ANIMATION::ATTACK1, true);
		pFsm->Change_State(CHel::HEL_ANIMATION::ATTACK1);
	}
}



void CHel_RunJump::End_State()
{
	CHel* pHel = static_cast<CHel*>(m_pOwner);
	pHel->Get_MotionTrail()->Set_Active(false);
}


CHel_RunJump* CHel_RunJump::Create(class CGameObject* pOwner)
{
	CHel_RunJump* pInstance = new CHel_RunJump(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_RunJump"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_RunJump::Free()
{
	__super::Free();
}
