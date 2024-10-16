#include "stdafx.h"
#include "Player_Nami_CutScene.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
#include "Animation.h"

#include "Hel.h"

#include "SwordTrail.h"

#include "EventNotify.h"

CPlayer_Nami_CutScene::CPlayer_Nami_CutScene(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::DUMMY3 , pOwner }
{

}

HRESULT CPlayer_Nami_CutScene::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Nami_CutScene::Start_State(void* pArg)
{
	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();
	CTransform* pTransform = m_pOwner->Get_Transform();
	
	pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_ZeroTimer();



	_vector vJumpDirection = _vector{ 0.f, 1.f, 0.f };
	vJumpDirection = XMVector3Normalize(vJumpDirection);



	pRigidBody->Add_Force_Direction(vJumpDirection, 950, Engine::CRigidBody::ACCELERATION);
	pRigidBody->Add_Force_Direction(vJumpDirection, 2600, Engine::CRigidBody::VELOCITYCHANGE);

	static_cast<CPlayer*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_PARTICLE_NAMI)->SetActiveMyParticle(true);


	
	m_pGameInstance->ActiveBlur(nullptr, CRenderer::BLUR_TYPE::MOTION_BLUR);

	m_pGameInstance->Play_Sound(TEXT("NamiStart.ogg"), SOUND_PLAYEREFFECT, 3.f);

	return S_OK;
}

void CPlayer_Nami_CutScene::Update(_float fTimeDelta)
{
	Check_Collision();

	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();
	pRigidBody->Set_GravityReduceRatio(0.35f);

	CTransform* pPlayerTransform = m_pOwner->Get_Transform();
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::STATE_POSITION);

	_float4x4* pPlayerRotationMatrix = static_cast<CPlayer*>(m_pOwner)->Get_RotationMatrixPtr();
	
	CHel* pHel = static_cast<CHel*>(m_pGameInstance->Get_GameObjects(LEVEL_STAGE2_BOSS, L"Layer_Hel").back());
	_vector vTargetPos = pHel->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	vTargetPos = XMVectorSetY(vTargetPos, vTargetPos.m128_f32[1] + 70.f);	// 원래 40이 맞는데 Hel 막기할때 위로올라온단말이야 그래서 70
	_vector vDistance = XMVectorSubtract(vTargetPos, vPlayerPos);
	_float fDistance = XMVectorGetX(XMVector3Length(vDistance));


	pPlayerTransform->LookAt(vTargetPos, pPlayerRotationMatrix);


	m_fStartTime += fTimeDelta;
	if (fDistance <= 60.f)
	{
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
		_double Duration = pModel->Get_CurAnimation()->Get_Duration();
		_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();

		pRigidBody->Set_IsGravity(false);

		if (CPlayer::PLAYER_ANIMATIONID::NAMI_AIM_ATTACK_TO_IDLE != pModel->Get_CurAnimationIndex())
		{
			pHel->Get_Model()->SetUp_Animation(CHel::HEL_ANIMATION::BACK_BLOCK , true);
			pHel->Get_Fsm()->Change_State(CHel::HEL_ANIMATION::BACK_BLOCK);
	

			vector<class CAnimation*>& Anims = pModel->Get_Animations();
			Anims[CPlayer::PLAYER_ANIMATIONID::NAMI_AIM_ATTACK_TO_IDLE]->Set_SpeedPerSec(8.f);


			if(false == m_pGameInstance->Check_IsPlaying(SOUND_PLAYEREFFECT))
				m_pGameInstance->Play_Sound(TEXT("NamiAttack.ogg"), SOUND_PLAYEREFFECT, 3.f);


			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::NAMI_AIM_ATTACK_TO_IDLE, true);

			CSwordTrail* pSwordTrail = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON))->Get_SwordTrail();
			pSwordTrail->Set_Active(true);

			m_pGameInstance->UnActiveBlur();
		}

		else if (CPlayer::PLAYER_ANIMATIONID::NAMI_AIM_ATTACK_TO_IDLE == pModel->Get_CurAnimationIndex())
		{
			if (0.45f <= TrackPos / (_float)Duration)
			{
				pRigidBody->Set_IsGravity(true);

				CFsm* pFsm = m_pOwner->Get_Fsm();
			
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
			}
		}
	}
	else
	{
		if (m_fStartTime >= 1.f)
		{
			_float fMaxSpeed = 90.f;  
			_float fMinSpeed = 5.f;   
			_float fSpeedFactor = 0.1f; 

			// 거리에 비례한 속도 계산 (최대 속도를 넘지 않도록)
			m_fAccSpeed = min(fDistance * fSpeedFactor, fMaxSpeed);	
			pPlayerTransform->Go_Dir(vDistance , m_fAccSpeed * fTimeDelta);
		}
	}


}

void CPlayer_Nami_CutScene::End_State()
{
	m_fAccTime = 0.f;

	m_pOwner->Get_RigidBody()->Set_GravityReduceRatio(1.f);		// 원상복구

	CSwordTrail* pSwordTrail = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON))->Get_SwordTrail();
	pSwordTrail->Set_Active(false);




	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
	vector<class CAnimation*>& Anims = pModel->Get_Animations();
	Anims[CPlayer::PLAYER_ANIMATIONID::NAMI_AIM_ATTACK_TO_IDLE]->Set_SpeedPerSec(40.f);


	m_pGameInstance->UnActiveBlur();
}



void CPlayer_Nami_CutScene::Check_Collision()
{

}

CPlayer_Nami_CutScene* CPlayer_Nami_CutScene::Create(class CGameObject* pOwner)
{
	CPlayer_Nami_CutScene* pInstance = new CPlayer_Nami_CutScene(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Nami_CutScene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Nami_CutScene::Free()
{
	__super::Free();
}
