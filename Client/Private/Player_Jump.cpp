#include "stdafx.h"
#include "Player_Jump.h"

#include "Player.h"

#include "Body_Player.h"
#include "Weapon_Player.h"

#include "Animation.h"

#include "GameInstance.h"
#include "GrapplingPointUI.h"

#include "EventNotify.h"

CPlayer_Jump::CPlayer_Jump(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::JUMP_START , pOwner }
{

}

HRESULT CPlayer_Jump::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Jump::Start_State(void* pArg)
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


	if (nullptr != pArg)
	{
		if (true == *static_cast<_bool*>(pArg))
		{
			fXWeight = 3.f * (fSpeed / fOringinSpeed);
			fYWeight = 5.f;
			fZWeight = 3.f * (fSpeed / fOringinSpeed);

			m_pOwner->Get_RigidBody()->Set_GravityReduceRatio(0.7f);	// 중력을 감소시킨다 일시적으로 
		}

	}

	vJumpDirection = XMVectorSetX(vJumpDirection, XMVectorGetX(vJumpDirection) * fXWeight);
	vJumpDirection = XMVectorSetY(vJumpDirection, XMVectorGetY(vJumpDirection) * fYWeight);
	vJumpDirection = XMVectorSetZ(vJumpDirection, XMVectorGetZ(vJumpDirection) * fZWeight);


	vJumpDirection = XMVector3Normalize(vJumpDirection);

	if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP)
	{
		pRigidBody->Add_Force_Direction(vJumpDirection, 2900, Engine::CRigidBody::VELOCITYCHANGE);
	}

	m_fAccTime = 0.3f;

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
	if (pBody_Player->IsLandingWall())
		if (Check_Jump())
			return;

	
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	CWeapon_Player::WEAPON_TYPE CurWeaponType = pPlayer->Get_CurWeaponType();
	
	if (CWeapon_Player::WEAPON_TYPE::SHURIKEN == CurWeaponType)
	{
		if (Check_Sh_Attack1())
			return;

		if (m_isCanDoubleJump && Check_DoubleJump())
			return;

		if (pPlayer->IsDashActive() && Check_Sh_Dash())
			return;
	}

	
	if (pPlayer->IsDashActive() && Check_Dash())
		return;


	// SKILL
	if (pPlayer->IsCutAllActive() && Check_CutAll())
		return;
	else
	{
		if (m_pGameInstance->Get_KeyState(KEY::Q) == KEY_STATE::TAP)
		{
			static_cast<CEventNotify*>(m_pGameInstance->Find_Notify(LEVEL_GAMEPLAY))->Set_Active(true, CEventNotify::TEXT_EVENT::UNABLE_SKILL_COOLTIME);
		}
	}
	if (pPlayer->IsNamiActive() && Check_Nami())
		return;
	else
	{
		if (m_pGameInstance->Get_KeyState(KEY::ONE) == KEY_STATE::TAP)
		{
			static_cast<CEventNotify*>(m_pGameInstance->Find_Notify(LEVEL_GAMEPLAY))->Set_Active(true, CEventNotify::TEXT_EVENT::UNABLE_SKILL_COOLTIME);
		}
	}
	if (pPlayer->IsTimeStopActive() && Check_TimeStop())
		return;
	else
	{
		if (m_pGameInstance->Get_KeyState(KEY::R) == KEY_STATE::TAP)
		{
			static_cast<CEventNotify*>(m_pGameInstance->Find_Notify(LEVEL_GAMEPLAY))->Set_Active(true, CEventNotify::TEXT_EVENT::UNABLE_SKILL_COOLTIME);
		}
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
	m_fAccTime = 0.3f;
	m_fLastKeyPressTimeW = 0.f;
	m_fLastKeyPressTimeS = 0.f;
	m_fLastKeyPressTimeA = 0.f;
	m_fLastKeyPressTimeD = 0.f;



	m_isCanDoubleJump = true;

	m_pOwner->Get_RigidBody()->Set_GravityReduceRatio(1.f);
}


_bool CPlayer_Jump::Check_Jump()
{
	if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP)
	{
		CTransform* pTransform = m_pOwner->Get_Transform();
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

		_float fLandY = pPlayer->Get_LandPosY();
		_float fOffSetY = pPlayer->Get_OffsetY();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


		if (false == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())		// 벽타기중이 아니라면 원래 점프
		{
			if (fLandY + fOffSetY == XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION)))
			{
				CFsm* pFsm = m_pOwner->Get_Fsm();
				CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::JUMP_START);
			}
		}
		else
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();
			CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

			_bool	bLandWall = true;
			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::JUMP_START, -1, &bLandWall);
		}

		return true;
	}

	return false;
}

_bool CPlayer_Jump::Check_HookUp()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	if (m_pGameInstance->Get_KeyState(KEY::F) == KEY_STATE::TAP)
	{
		_vector vPlayerPos = pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		vPlayerPos = XMVectorSetY(vPlayerPos, 0.f);

		list<CGameObject*>& GrapUIs = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_GrapplingPointUI");
		_float fMinDistance = { 9999.f };
		CGrapplingPointUI* pClosestGrapUI = nullptr;

		for (auto& GrapUI : GrapUIs)
		{
			CGrapplingPointUI* pGrapUI = static_cast<CGrapplingPointUI*>(GrapUI);

			if (false == pGrapUI->IsActive())
				continue;
			else
			{
				_vector vGrapUIPos = pGrapUI->Get_Transform()->Get_State(CTransform::STATE_POSITION);
				vGrapUIPos = XMVectorSetY(vGrapUIPos, 0.f);				// X,Z성분만 비교한다. 


				_float fDistance = XMVectorGetX(XMVector3Length(vGrapUIPos - vPlayerPos));
				if (fDistance < fMinDistance)
				{
					fMinDistance = fDistance;
					pClosestGrapUI = pGrapUI;
				}
			}
		}


		if (nullptr != pClosestGrapUI)		// 조건에 만족하는 제일가까운 GrapPoint를 찾았다.
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();
			CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

			if (CWeapon_Player::WEAPON_TYPE::KATANA == pPlayer->Get_CurWeaponType())
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::HOOK_UP, false);
			else if (CWeapon_Player::WEAPON_TYPE::SHURIKEN == pPlayer->Get_CurWeaponType())
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::HOOK_UP, false);

			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::HOOK_UP, -1, pClosestGrapUI);

			return true;
		}
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

_bool CPlayer_Jump::Check_Dash()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	_bool bWTap = m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::TAP;
	_bool bSTap = m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::TAP;
	_bool bATap = m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::TAP;
	_bool bDTap = m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::TAP;

	if (bWTap || bSTap || bATap || bDTap)
	{
		if (bWTap) {
			if (m_fLastKeyPressTimeW >= 0.f && (m_fAccTime - m_fLastKeyPressTimeW <= 0.2f))
			{
				// W 키 더블 클릭 감지
				CState::STATE_DIR eDir = CState::STATE_DIR::FRONT;
				CFsm* pFsm = m_pOwner->Get_Fsm();
				CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::DASH_F, eDir);
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::DASH_F, false);

				m_fLastKeyPressTimeW = -1.f;
				return true;
			}
			m_fLastKeyPressTimeW = m_fAccTime;
		}

		if (bSTap) {
			if (m_fLastKeyPressTimeS >= 0.f && (m_fAccTime - m_fLastKeyPressTimeS <= 0.2f))
			{
				// S 키 더블 클릭 감지
				CState::STATE_DIR eDir = CState::STATE_DIR::BACK;
				CFsm* pFsm = m_pOwner->Get_Fsm();
				CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::DASH_F, eDir);
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::DASH_B, false);

				m_fLastKeyPressTimeS = -1.f;
				return true;
			}
			m_fLastKeyPressTimeS = m_fAccTime;
		}

		if (bATap) {
			if (m_fLastKeyPressTimeA >= 0.f && (m_fAccTime - m_fLastKeyPressTimeA <= 0.2f))
			{
				// A 키 더블 클릭 감지
				CState::STATE_DIR eDir = CState::STATE_DIR::LEFT;
				CFsm* pFsm = m_pOwner->Get_Fsm();
				CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::DASH_F, eDir);
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::DASH_L, false);

				m_fLastKeyPressTimeA = -1.f;
				return true;
			}
			m_fLastKeyPressTimeA = m_fAccTime;
		}

		if (bDTap) {
			if (m_fLastKeyPressTimeD >= 0.f && (m_fAccTime - m_fLastKeyPressTimeD <= 0.2f))
			{
				// D 키 더블 클릭 감지
				CState::STATE_DIR eDir = CState::STATE_DIR::RIGHT;
				CFsm* pFsm = m_pOwner->Get_Fsm();
				CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::DASH_F, eDir);
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::DASH_R, false);

				m_fLastKeyPressTimeD = -1.f;
				return true;
			}
			m_fLastKeyPressTimeD = m_fAccTime;
		}
	}

	return false;
}

_bool CPlayer_Jump::Check_Sh_Dash()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	_bool bWTap = m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::TAP;
	_bool bSTap = m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::TAP;
	_bool bATap = m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::TAP;
	_bool bDTap = m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::TAP;

	if (bWTap || bSTap || bATap || bDTap)
	{
		if (bWTap) {
			if (m_fLastKeyPressTimeW >= 0.f && (m_fAccTime - m_fLastKeyPressTimeW <= 0.2f))
			{
				// W 키 더블 클릭 감지
				CState::STATE_DIR eDir = CState::STATE_DIR::FRONT;
				CFsm* pFsm = m_pOwner->Get_Fsm();
				CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::SH_DASH_F, eDir);
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_DASH_F, false);

				m_fLastKeyPressTimeW = -1.f;
				return true;
			}
			m_fLastKeyPressTimeW = m_fAccTime;
		}

		if (bSTap) {
			if (m_fLastKeyPressTimeS >= 0.f && (m_fAccTime - m_fLastKeyPressTimeS <= 0.2f))
			{
				// S 키 더블 클릭 감지
				CState::STATE_DIR eDir = CState::STATE_DIR::BACK;
				CFsm* pFsm = m_pOwner->Get_Fsm();
				CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::SH_DASH_F, eDir);
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_DASH_B, false);

				m_fLastKeyPressTimeS = -1.f;
				return true;
			}
			m_fLastKeyPressTimeS = m_fAccTime;
		}

		if (bATap) {
			if (m_fLastKeyPressTimeA >= 0.f && (m_fAccTime - m_fLastKeyPressTimeA <= 0.2f))
			{
				// A 키 더블 클릭 감지
				CState::STATE_DIR eDir = CState::STATE_DIR::LEFT;
				CFsm* pFsm = m_pOwner->Get_Fsm();
				CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::SH_DASH_F, eDir);
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_DASH_L, false);

				m_fLastKeyPressTimeA = -1.f;
				return true;
			}
			m_fLastKeyPressTimeA = m_fAccTime;
		}

		if (bDTap) {
			if (m_fLastKeyPressTimeD >= 0.f && (m_fAccTime - m_fLastKeyPressTimeD <= 0.2f))
			{
				// D 키 더블 클릭 감지
				CState::STATE_DIR eDir = CState::STATE_DIR::RIGHT;
				CFsm* pFsm = m_pOwner->Get_Fsm();
				CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::SH_DASH_F, eDir);
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_DASH_R, false);

				m_fLastKeyPressTimeD = -1.f;
				return true;
			}
			m_fLastKeyPressTimeD = m_fAccTime;
		}
	}

	return false;
}

_bool CPlayer_Jump::Check_CutAll()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::FURR_AIM_LOOP != pModel->Get_CurAnimationIndex() &&
		m_pGameInstance->Get_KeyState(KEY::Q) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();


		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::FURR_AIM_LOOP);

		return true;
	}

	return false;
}

_bool CPlayer_Jump::Check_Nami()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (m_pGameInstance->Get_KeyState(KEY::ONE) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::NAMI_IDLE_TO_AIM);

		return true;
	}

	return false;
}

_bool CPlayer_Jump::Check_TimeStop()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (m_pGameInstance->Get_KeyState(KEY::R) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();



		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::TIME_STOP, true);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::TIME_STOP);

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
