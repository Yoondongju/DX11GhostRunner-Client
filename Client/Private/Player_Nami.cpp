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

#include "SwordTrail.h"

#include "EventNotify.h"

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
	if (m_Targets.capacity() < 10)
		m_Targets.reserve(10);

	FindShortestDistanceEnemy();

	if (XMVector3Equal(XMLoadFloat3(&m_TargetPos), XMVectorZero()))
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);

		static_cast<CEventNotify*>(m_pGameInstance->Find_Notify(LEVEL_GAMEPLAY))->Set_Active(true, CEventNotify::TEXT_EVENT::UNABLE_CUTALL);
		
		return S_OK;
	}


	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::NAMI_IDLE_TO_AIM, true);

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

	
	CTransform* pPlayerTransform = m_pOwner->Get_Transform();
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::STATE_POSITION);


	if (false == XMVector3Equal(XMLoadFloat3(&m_TargetPos), XMVectorZero()))
	{
		_vector vDistance = XMVectorSubtract(XMLoadFloat3(&m_TargetPos), vPlayerPos);
		_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

		_float4x4* pPlayerRotationMatrix = static_cast<CPlayer*>(m_pOwner)->Get_RotationMatrixPtr();


		_float fAngleGap = {};

		pPlayerTransform->LookAt_Smooth(XMLoadFloat3(&m_TargetPos), fTimeDelta, pPlayerRotationMatrix, &fAngleGap);


		if (false == m_isFindTargets && fAngleGap < 30.f)		// 5도는 현재 디그리 각도임
			FindVisiableEnemy();								// UI그려줄 타겟 찾았지 그담에 머해
		else if (true == m_isFindTargets)
		{
			m_pOwner->Get_RigidBody()->Set_GravityReduceRatio(0.35f);	// 절반만큼 줄이고

			if (m_iTargetIndex < m_Targets.size())
			{
				m_fTargetCreateTime += fTimeDelta;

				if (m_fTargetCreateTime > 0.1f)
				{
					static_cast<CEnemy*>(m_Targets[m_iTargetIndex++])->Set_Targeting(true);
					m_fTargetCreateTime = 0.f;
				}

				return;
			}

			

			m_fTargetCreateTime += fTimeDelta;


			if (m_fTargetCreateTime >= 0.6f)
			{
				for (_uint i = 0; i < m_Targets.size(); i++)
				{
					_vector vRealTargetPos = m_Targets[i]->Get_Transform()->Get_State(CTransform::STATE_POSITION);

					vRealTargetPos = XMVectorSetY(vRealTargetPos, vRealTargetPos.m128_f32[1] + 20.f);	// 이거 꼭필요해 내 타겟좌표 y를 20올렷거든

					if (XMVector3Equal(vRealTargetPos, XMLoadFloat3(&m_TargetPos)))
					{
						static_cast<CEnemy*>(m_Targets[i])->Set_FinalTargeting(true);
						continue;
					}
					else
						static_cast<CEnemy*>(m_Targets[i])->Set_Targeting(false);
				}

				m_isHuntActive = true;
			}
			

			if(true == m_isHuntActive)
			{
				m_fStartTime += fTimeDelta;

				if (fDistance <= 40.f)
				{
					CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
					_double Duration = pModel->Get_CurAnimation()->Get_Duration();
					_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();

					if (CPlayer::PLAYER_ANIMATIONID::NAMI_AIM_ATTACK_TO_IDLE != pModel->Get_CurAnimationIndex())
					{
						pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::NAMI_AIM_ATTACK_TO_IDLE, true);

						CSwordTrail* pSwordTrail = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON))->Get_SwordTrail();
						pSwordTrail->Set_Active(true);
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
	}
}

void CPlayer_Nami::End_State()
{
	m_Targets.clear();

	if (!XMVector3Equal(XMLoadFloat3(&m_TargetPos), XMVectorZero()))
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
		pPlayer->Set_StartCountNamiTime(false);
		pPlayer->Set_NamiRemainingTime(0.f);
	}

	XMStoreFloat3(&m_TargetPos, XMVectorZero());
	XMStoreFloat3(&m_TargetLook, XMVectorZero());

	m_fMinDistance = 999999.f;

	m_fAccSpeed = 0.f;
	m_fStartTime = 0.f;
	m_iTargetIndex = 0;
	m_fTargetCreateTime = 0.f;
	m_isFindTargets = false;
	m_isHuntActive = false;

	m_pOwner->Get_RigidBody()->Set_GravityReduceRatio(1.f);		// 원상복구

CSwordTrail* pSwordTrail = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON))->Get_SwordTrail();
	pSwordTrail->Set_Active(false);
}


void CPlayer_Nami::FindVisiableEnemy()
{
	CTransform* pPlayerTransform = m_pOwner->Get_Transform();

	_vector vPlayerLookNor = XMVector3Normalize(pPlayerTransform->Get_State(CTransform::STATE_LOOK));
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_float fVisiableAngle = m_fVisibleAngle * 0.5f;


	list<CGameObject*>& Snipers = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Sniper");
	for (auto Sniper : Snipers)
	{
		_vector vSniperPos = Sniper->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		_vector vDifference = XMVectorSubtract(vSniperPos, vPlayerPos);		// 벡터의 차이
		_float fDistacne = XMVectorGetX(XMVector3Length(vDifference));		// 그 차이난 벡터의 길이

		_vector vDir = XMVector3Normalize(vDifference);
		_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLookNor, vDir));		// 내적의 결과고

		_float fAngle = XMConvertToDegrees(acos(fDot));

		if (fVisiableAngle >= fAngle &&
			m_fVisiableDistance >= fDistacne)	// 시야각 안에있고 거리도 만족하니
		{
			m_Targets.emplace_back(Sniper);
		}
	}

	list<CGameObject*>& Pistols = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Pistol");
	for (auto Pistol : Pistols)
	{
		_vector vPistolPos = Pistol->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		_vector vDifference = XMVectorSubtract(vPistolPos, vPlayerPos);		// 벡터의 차이
		_float fDistacne = XMVectorGetX(XMVector3Length(vDifference));		// 그 차이난 벡터의 길이

		_vector vDir = XMVector3Normalize(vDifference);
		_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLookNor, vDir));		// 내적의 결과고

		_float fAngle = XMConvertToDegrees(acos(fDot));

		if (fVisiableAngle >= fAngle &&
			m_fVisiableDistance >= fDistacne)	// 시야각 안에 잇니
		{
			m_Targets.emplace_back(Pistol);
		}
	}

	list<CGameObject*>& Miras = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Mira");
	for (auto Mira : Miras)
	{
		_vector vMiraPos = Mira->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		_vector vDifference = XMVectorSubtract(vMiraPos, vPlayerPos);		// 벡터의 차이
		_float fDistacne = XMVectorGetX(XMVector3Length(vDifference));		// 그 차이난 벡터의 길이

		_vector vDir = XMVector3Normalize(vDifference);
		_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLookNor, vDir));		// 내적의 결과고

		_float fAngle = XMConvertToDegrees(acos(fDot));

		if (fVisiableAngle >= fAngle &&
			m_fVisiableDistance >= fDistacne)	// 시야각 안에 잇니
		{
			m_Targets.emplace_back(Mira);
		}
	}

	m_isFindTargets = true;
}

void CPlayer_Nami::FindShortestDistanceEnemy()
{
	CTransform* pPlayerTransform = m_pOwner->Get_Transform();
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::STATE_POSITION);

	if (XMVector3Equal(XMLoadFloat3(&m_TargetPos), XMVectorZero()))
	{
		list<CGameObject*>& Snipers = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Sniper");
		for (auto& Sniper : Snipers)
		{
			if (true == static_cast<CEnemy*>(Sniper)->IsDead())
				return;

			_vector vSniperPos = Sniper->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION);

			vSniperPos = XMVectorSetY(vSniperPos, vSniperPos.m128_f32[1] + 20.f);
			_vector vDistance = XMVectorSubtract(vPlayerPos, vSniperPos);
			_float fDistance = XMVectorGetX(XMVector3Length(vDistance));


			// 가장 가까운 몬스터 찾기
			if (m_fVisiableDistance >= fDistance && fDistance < m_fMinDistance)
			{
				m_fMinDistance = fDistance;
				XMStoreFloat3(&m_TargetPos, vSniperPos);
				XMStoreFloat3(&m_TargetLook, Sniper->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));
			}
		}

		list<CGameObject*>& Pistols = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Pistol");
		for (auto& Pistol : Pistols)
		{
			if (true == static_cast<CEnemy*>(Pistol)->IsDead())
				return;


			_vector vPistolPos = Pistol->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION);

			vPistolPos = XMVectorSetY(vPistolPos, vPistolPos.m128_f32[1] + 20.f);
			_vector vDistance = XMVectorSubtract(vPlayerPos, vPistolPos);
			_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

			// 가장 가까운 몬스터 찾기
			if (m_fVisiableDistance >= fDistance &&  fDistance < m_fMinDistance)
			{
				m_fMinDistance = fDistance;
				XMStoreFloat3(&m_TargetPos, vPistolPos);
				XMStoreFloat3(&m_TargetLook, Pistol->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));
			}
		}

		list<CGameObject*>& Miras = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Mira");
		for (auto& Mira : Miras)
		{
			if (true == static_cast<CEnemy*>(Mira)->IsDead())
				return;

			_vector vMiraPos = Mira->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION);


			vMiraPos = XMVectorSetY(vMiraPos, vMiraPos.m128_f32[1] + 20.f);
			_vector vDistance = XMVectorSubtract(vPlayerPos, vMiraPos);
			_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

			// 가장 가까운 몬스터 찾기
			if (m_fVisiableDistance >= fDistance &&  fDistance < m_fMinDistance)
			{
				m_fMinDistance = fDistance;
				XMStoreFloat3(&m_TargetPos, vMiraPos);
				XMStoreFloat3(&m_TargetLook, Mira->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));
			}
		}

	}
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
