#include "stdafx.h"
#include "Player_Nami.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
#include "Animation.h"

#include "Sniper.h"
#include "Pistol.h"
#include "Mira.h"
#include "Jetpack.h"

CPlayer_Nami::CPlayer_Nami(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::NAMI_IDLE_TO_AIM , pOwner }
{

}

HRESULT CPlayer_Nami::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Nami::Start_State(void* pArg)
{
	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();

	pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_ZeroTimer();



	_vector vJumpDirection = _vector{ 0.f, 1.f, 0.f };
	

	vJumpDirection = XMVector3Normalize(vJumpDirection);

	
	pRigidBody->Add_Force_Direction(vJumpDirection, 300, Engine::CRigidBody::ACCELERATION);
	pRigidBody->Add_Force_Direction(vJumpDirection, 2500, Engine::CRigidBody::VELOCITYCHANGE);
	
	static_cast<CPlayer*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_PARTICLE_NAMI)->SetActiveMyParticle(true);

	return S_OK;
}

void CPlayer_Nami::Update(_float fTimeDelta)
{
	Check_Collision();

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


	CTransform* pPlayerTransform = m_pOwner->Get_Transform();
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::STATE_POSITION);


	if (CPlayer::PLAYER_ANIMATIONID::NAMI_IDLE_TO_AIM == pModel->Get_CurAnimationIndex())
	{
		_double Duration = pModel->Get_CurAnimation()->Get_Duration();
		_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();

		if (XMVector3Equal(XMLoadFloat3(&m_TargetPos) , XMVectorZero()) &&
			0.6f <= TrackPos / (_float)Duration)
		{
			list<CGameObject*>& Snipers = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Sniper");
			for (auto& Sniper : Snipers)
			{
				_vector vSniperPos = Sniper->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION);

				_vector vDistance = XMVectorSubtract(vPlayerPos, vSniperPos);
				_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

				// 가장 가까운 몬스터 찾기
				if (fDistance < m_fMinDistance)
				{
					m_fMinDistance = fDistance;
					XMStoreFloat3(&m_TargetPos , vSniperPos);
					XMStoreFloat3(&m_TargetLook, Sniper->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));
				}
			}

			list<CGameObject*>& Pistols = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Pistol");
			for (auto& Pistol : Pistols)
			{
				_vector vPistolPos = Pistol->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION);

				_vector vDistance = XMVectorSubtract(vPlayerPos, vPistolPos);
				_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

				// 가장 가까운 몬스터 찾기
				if (fDistance < m_fMinDistance)
				{
					m_fMinDistance = fDistance;
					XMStoreFloat3(&m_TargetPos, vPistolPos);
					XMStoreFloat3(&m_TargetLook, Pistol->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));
				}
			}

			list<CGameObject*>& Miras = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Mira");
			for (auto& Mira : Miras)
			{
				_vector vMiraPos = Mira->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION);

				_vector vDistance = XMVectorSubtract(vPlayerPos, vMiraPos);
				_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

				// 가장 가까운 몬스터 찾기
				if (fDistance < m_fMinDistance)
				{
					m_fMinDistance = fDistance;
					XMStoreFloat3(&m_TargetPos, vMiraPos);
					XMStoreFloat3(&m_TargetLook, Mira->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));
				}
			}

		}
	}



	if (false == XMVector3Equal(XMLoadFloat3(&m_TargetPos), XMVectorZero()))
	{
		_vector vDistance = XMVectorSubtract(XMLoadFloat3(&m_TargetPos), vPlayerPos);
		_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

		_float4x4* pPlayerRotationMatrix = static_cast<CPlayer*>(m_pOwner)->Get_RotationMatrixPtr();


		m_fStartTime += fTimeDelta;
		pPlayerTransform->LookAt_Smooth(XMLoadFloat3(&m_TargetPos), fTimeDelta, pPlayerRotationMatrix);



		if (fDistance <= 40.f)
		{
			_double Duration = pModel->Get_CurAnimation()->Get_Duration();
			_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
			

			if (CPlayer::PLAYER_ANIMATIONID::NAMI_AIM_ATTACK_TO_IDLE != pModel->Get_CurAnimationIndex())
			{
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::NAMI_AIM_ATTACK_TO_IDLE, true);		
			}
				
			else if (CPlayer::PLAYER_ANIMATIONID::NAMI_AIM_ATTACK_TO_IDLE == pModel->Get_CurAnimationIndex())
			{
				if (0.9f <= TrackPos / (_float)Duration)
				{
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
				//_vector vPlayerLookNor = XMVector3Normalize(pPlayerTransform->Get_State(CTransform::STATE::STATE_LOOK));
				//_float fRotationSpeed = 15.f; // 적절한 회전 속도 설정
				//pPlayerTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * fRotationSpeed, pPlayerRotationMatrix);

				m_fAccSpeed += fTimeDelta;
				pPlayerTransform->Go_Dir(vDistance, m_fAccSpeed);
			
			}
		}
	}
	


}

void CPlayer_Nami::End_State()
{
	XMStoreFloat3(&m_TargetPos, XMVectorZero());
	XMStoreFloat3(&m_TargetLook, XMVectorZero());

	m_fMinDistance = 999999.f;

	m_fAccSpeed = 0.f;
	m_fStartTime = 0.f;
	m_isFirstFound = true;

}


void CPlayer_Nami::Check_Collision()
{
	list<CGameObject*>& Snipers = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Sniper");
	for (auto& Sniper : Snipers)
	{
		_bool b = static_cast<CSniper*>(Sniper)->Check_Collision();
	}

	list<CGameObject*>& Pistols = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Pistol");
	for (auto& Pistol : Pistols)
	{
		_bool b = static_cast<CPistol*>(Pistol)->Check_Collision();
	}

	list<CGameObject*>& Miras = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Mira");
	for (auto& Mira : Miras)
	{
		_bool b = static_cast<CMira*>(Mira)->Check_Collision();
	}

	list<CGameObject*>& Jetpacks = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Jetpack");
	for (auto& Jetpack : Jetpacks)
	{
		_bool b = static_cast<CJetpack*>(Jetpack)->Check_Collision();
	}
}

CPlayer_Nami* CPlayer_Nami::Create(class CGameObject* pOwner)
{
	CPlayer_Nami* pInstance = new CPlayer_Nami(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Nami"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Nami::Free()
{
	__super::Free();
}
