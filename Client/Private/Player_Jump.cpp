#include "stdafx.h"
#include "Player_Jump.h"

#include "Player.h"

#include "Body_Player.h"
#include "Weapon_Player.h"

#include "Animation.h"

#include "GameInstance.h"
#include "GrapplingPointUI.h"

CPlayer_Jump::CPlayer_Jump(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::JUMP_START , pOwner }
{

}

HRESULT CPlayer_Jump::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Jump::Start_State()
{
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	CTransform* pTransform = m_pOwner->Get_Transform();
	CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();

	pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
	pRigidBody->Set_ZeroTimer();



	// 내가 Space로누른 의도한 점프라면 Force를 주고
	_vector vJumpDirection = _vector{ 0.f, 1.f, 0.f };
	if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD)
	{
		vJumpDirection += pTransform->Get_State(CTransform::STATE_LOOK);
	}
	if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD)
	{
		vJumpDirection -= pTransform->Get_State(CTransform::STATE_LOOK);
	}
	if (m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD)
	{
		vJumpDirection -= pTransform->Get_State(CTransform::STATE_RIGHT);
	}
	if (m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
	{
		vJumpDirection += pTransform->Get_State(CTransform::STATE_RIGHT);
	}


	_float fSpeed = pTransform->Get_SpeedPerSec();
	_float fOringinSpeed = pTransform->Get_OriginSpeedPerSec();

	_float fXWeight = 1.f * (fSpeed / fOringinSpeed);
	_float fYWeight = 3.f;
	_float fZWeight = 1.f * (fSpeed / fOringinSpeed);

	vJumpDirection = XMVectorSetX(vJumpDirection, XMVectorGetX(vJumpDirection) * fXWeight);
	vJumpDirection = XMVectorSetY(vJumpDirection, XMVectorGetY(vJumpDirection) * fYWeight);
	vJumpDirection = XMVectorSetZ(vJumpDirection, XMVectorGetZ(vJumpDirection) * fZWeight);


	vJumpDirection = XMVector3Normalize(vJumpDirection);

	if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP)
	{
		pRigidBody->Add_Force_Direction(vJumpDirection, 1600, Engine::CRigidBody::VELOCITYCHANGE);
	}

	return S_OK;
}

void CPlayer_Jump::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;		// 점프 진행시간으로 쓰자

	CBody_Player* pBody_Player = static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY));
	if (pBody_Player->IsClimbing())
	{
		if(Check_Climb())
			return;
	}
	
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	CWeapon_Player::WEAPON_TYPE CurWeaponType = pPlayer->Get_CurWeaponType();
	
	if (CWeapon_Player::WEAPON_TYPE::SHURIKEN == CurWeaponType)
	{
		if (Check_Sh_Attack1())
			return;

		if (m_isCanDoubleJump && Check_DoubleJump())
			return;
	}

	



	CTransform* pTransform = m_pOwner->Get_Transform();
	_float fLandY = pPlayer->Get_LandPosY();
	_float fOffSetY = pPlayer->Get_OffsetY();
	  
	CModel* pModel = pBody_Player->Get_Model();
	

	if (CWeapon_Player::WEAPON_TYPE::KATANA == CurWeaponType)
	{
		if (0.1f >= fabs((fLandY + fOffSetY) - XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION))))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();

			if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD ||
				m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD ||
				m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD ||
				m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
			{
				if (m_pGameInstance->Get_KeyState(KEY::LSHIFT) == KEY_STATE::HOLD)
				{
					//pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::RUN, true);
					pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::RUN);
				}		
				else
				{
					//pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::WALK, true);
					pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::WALK);
				}
					
			}
			else
			{
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
			}
		}
	}
	else if (CWeapon_Player::WEAPON_TYPE::SHURIKEN == CurWeaponType)
	{
		// 더블점프를 줄까?
		if (0.1f >= fabs((fLandY + fOffSetY) - XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION))))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();

			if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD ||
				m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD ||
				m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD ||
				m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
			{
				if (m_pGameInstance->Get_KeyState(KEY::LSHIFT) == KEY_STATE::HOLD)
				{				
					pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::SH_RUN);
				}					
				else
				{
					pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::SH_WALK);
				}
					
			}
			else
			{				
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_IDLE, true);
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
			}
		}

	}


	

	if (Check_HookUp())
		return;

}


void CPlayer_Jump::End_State()
{
	m_fAccTime = 0.f;

	m_isCanDoubleJump = true;
}


_bool CPlayer_Jump::Check_HookUp()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	if (false == pPlayer->Get_GrapplingPoint()->Get_FindPlayer())
		return false;

	if (m_pGameInstance->Get_KeyState(KEY::F) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

		if (CWeapon_Player::WEAPON_TYPE::KATANA == pPlayer->Get_CurWeaponType())
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::HOOK_UP, false);
		else if (CWeapon_Player::WEAPON_TYPE::SHURIKEN == pPlayer->Get_CurWeaponType())
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::HOOK_UP, false);

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::HOOK_UP);

		return true;
	}

	return false;
}


_bool CPlayer_Jump::Check_Climb()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	CFsm* pFsm = m_pOwner->Get_Fsm();
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CWeapon_Player::WEAPON_TYPE::KATANA == pPlayer->Get_CurWeaponType())
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::CLIMB, false);
	else if (CWeapon_Player::WEAPON_TYPE::SHURIKEN == pPlayer->Get_CurWeaponType())
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::CLIMB, false);

	pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::CLIMB);

	return true;
}

_bool CPlayer_Jump::Check_DoubleJump()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	_bool bSpaceTap = m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP;

	if (bSpaceTap)
	{
		if (m_fLastKeyPressTimeSpace >= 0.f && (m_fAccTime - m_fLastKeyPressTimeSpace <= 0.2f))
		{
	
			CTransform* pTransform = m_pOwner->Get_Transform();
			CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();

			pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
			pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
			pRigidBody->Set_ZeroTimer();


			// 내가 Space로누른 의도한 점프라면 Force를 주고
			_vector vJumpDirection = _vector{ 0.f, 1.f, 0.f };
			if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD)
			{
				vJumpDirection += pTransform->Get_State(CTransform::STATE_LOOK);
			}
			if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD)
			{
				vJumpDirection -= pTransform->Get_State(CTransform::STATE_LOOK);
			}
			if (m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD)
			{
				vJumpDirection -= pTransform->Get_State(CTransform::STATE_RIGHT);
			}
			if (m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
			{
				vJumpDirection += pTransform->Get_State(CTransform::STATE_RIGHT);
			}


			_float fSpeed = pTransform->Get_SpeedPerSec();
			_float fOringinSpeed = pTransform->Get_OriginSpeedPerSec();

			_float fXWeight = 1.f * (fSpeed / fOringinSpeed);
			_float fYWeight = 3.f;
			_float fZWeight = 1.f * (fSpeed / fOringinSpeed);

			vJumpDirection = XMVectorSetX(vJumpDirection, XMVectorGetX(vJumpDirection) * fXWeight);
			vJumpDirection = XMVectorSetY(vJumpDirection, XMVectorGetY(vJumpDirection) * fYWeight);
			vJumpDirection = XMVectorSetZ(vJumpDirection, XMVectorGetZ(vJumpDirection) * fZWeight);


			vJumpDirection = XMVector3Normalize(vJumpDirection);

	

			pRigidBody->Add_Force_Direction(vJumpDirection, 1600, Engine::CRigidBody::VELOCITYCHANGE);

			m_fLastKeyPressTimeSpace = -1.f;
			m_isCanDoubleJump = false;
			return true;
		}
		m_fLastKeyPressTimeSpace = m_fAccTime;
	}

	return false;
}

_bool CPlayer_Jump::Check_Sh_Attack1()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::SH_ATTACK != pModel->Get_CurAnimationIndex() &&
		m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
		TrackPos = 0.0;


 		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_ATTACK, false);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::SH_ATTACK);

		return true;
	}

	return false;
}


CPlayer_Jump* CPlayer_Jump::Create(class CGameObject* pOwner)
{
	CPlayer_Jump* pInstance = new CPlayer_Jump(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Jump"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Jump::Free()
{
	__super::Free();
}
