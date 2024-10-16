#include "stdafx.h"
#include "Player_Idle.h"

#include "Player.h"

#include "Body_Player.h"
#include "Weapon_Player.h"
#include "SwordTrail.h"

#include "GameInstance.h"
#include "GrapplingPointUI.h"

#include "Animation.h"

#include "EventNotify.h"

CPlayer_Idle::CPlayer_Idle(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::IDLE , pOwner }
{

}

HRESULT CPlayer_Idle::Initialize()
{
    return S_OK;
}

HRESULT CPlayer_Idle::Start_State(void* pArg)
{
	

    return S_OK;
}

void CPlayer_Idle::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;
	
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	CModel* pModel = static_cast<CBody_Player*>(pPlayer->Get_Part(CPlayer::PARTID::PART_BODY))->Get_Model();

	if (CWeapon_Player::WEAPON_TYPE::KATANA == pPlayer->Get_CurWeaponType())
	{
		m_fSwordSpinTime -= fTimeDelta;
		if (m_fSwordSpinTime < 0.f)
		{
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SPIN_B, true);
			m_fSwordSpinTime = 5.f;

			m_fFristSpinCall = true;


		CSwordTrail* pSwordTrail = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON))->Get_SwordTrail();
			pSwordTrail->Set_Active(true);
		}
		else
		{				
			if (true == m_fFristSpinCall && CPlayer::PLAYER_ANIMATIONID::SPIN_B == pModel->Get_CurAnimationIndex())
			{
				_double Duration = pModel->Get_CurAnimation()->Get_Duration();
				const _double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();

				if (0.8f <= (TrackPos / Duration))
				{
					CFsm* pFsm = m_pOwner->Get_Fsm();
			
					pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
					m_fFristSpinCall = false;

				CSwordTrail* pSwordTrail = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON))->Get_SwordTrail();
					pSwordTrail->Set_Active(false);
				}
			}
		}


		_uint iRandomValue = static_cast<_uint>(m_pGameInstance->Get_Random(0, 3));
		switch (iRandomValue)
		{
		case 0:
		{
			if (Check_Attack1())
				return;
			if (pPlayer->IsBlockActive() && Check_Block1())
				return;
			else
			{
				if (m_pGameInstance->Get_KeyState(KEY::E) == KEY_STATE::TAP)
				{
					static_cast<CEventNotify*>(m_pGameInstance->Find_Notify(LEVEL_GAMEPLAY))->Set_Active(true, CEventNotify::TEXT_EVENT::UNABLE_SKILL_COOLTIME);
				}
			}
		}
		break;
		case 1:
		{
			if (Check_Attack2())
				return;
			if (pPlayer->IsBlockActive() && Check_Block2())
				return;
			else
			{
				if (m_pGameInstance->Get_KeyState(KEY::E) == KEY_STATE::TAP)
				{
					static_cast<CEventNotify*>(m_pGameInstance->Find_Notify(LEVEL_GAMEPLAY))->Set_Active(true, CEventNotify::TEXT_EVENT::UNABLE_SKILL_COOLTIME);
				}
			}
		}
		break;
		case 2:
		{
			if (Check_Attack3())
				return;
			if (pPlayer->IsBlockActive() && Check_Block3())
				return;
			else
			{
				if (m_pGameInstance->Get_KeyState(KEY::E) == KEY_STATE::TAP)
				{
					static_cast<CEventNotify*>(m_pGameInstance->Find_Notify(LEVEL_GAMEPLAY))->Set_Active(true, CEventNotify::TEXT_EVENT::UNABLE_SKILL_COOLTIME);
				}
			}
		}
		break;
		default:
			break;
		}


		// DASH
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
		if (pPlayer->IsMindControlActive() && Check_MindControl())
			return;
		else
		{
			if (m_pGameInstance->Get_KeyState(KEY::TWO) == KEY_STATE::TAP)
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
	}

	else if (CWeapon_Player::WEAPON_TYPE::SHURIKEN == static_cast<CPlayer*>(m_pOwner)->Get_CurWeaponType())
	{
		if (Check_Sh_Attack1())
			return;
		if (pPlayer->IsHomingShActive() && Check_HomingShuriken())
			return;
		else
		{
			if (m_pGameInstance->Get_KeyState(KEY::Z) == KEY_STATE::TAP)
			{
				static_cast<CEventNotify*>(m_pGameInstance->Find_Notify(LEVEL_GAMEPLAY))->Set_Active(true, CEventNotify::TEXT_EVENT::UNABLE_SKILL_COOLTIME);
			}
		}

		if (pPlayer->IsDashActive() && Check_Sh_Dash())
			return;
			
	}



	if (Check_HookUp())
		return;

	if (Check_Jump())
		return;

	if (Check_RunAndWalk())
		return;

	if (pPlayer->IsCanSwapWeapon() && Check_SwapWeapon())
		return;
	else
	{
		if (m_pGameInstance->Get_KeyState(KEY::G) == KEY_STATE::TAP)
		{
			static_cast<CEventNotify*>(m_pGameInstance->Find_Notify(LEVEL_GAMEPLAY))->Set_Active(true, CEventNotify::TEXT_EVENT::UNABLE_SWAPWEAPON);
		}
	}


}

void CPlayer_Idle::End_State()
{
	m_fSwordSpinTime = 5.f;
}


_bool CPlayer_Idle::Check_RunAndWalk()
{
	if (CWeapon_Player::WEAPON_TYPE::KATANA == static_cast<CPlayer*>(m_pOwner)->Get_CurWeaponType())
	{
		if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD ||
			m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD ||
			m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD ||
			m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();
			CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
			CPlayer::PLAYER_ANIMATIONID eID = CPlayer::PLAYER_ANIMATIONID::WALK;

			if (m_pGameInstance->Get_KeyState(KEY::LSHIFT) == KEY_STATE::HOLD)
				eID = CPlayer::PLAYER_ANIMATIONID::RUN;

			pModel->SetUp_Animation(eID, true);
			pFsm->Change_State(eID);

			return true;
		}
	}
	else if (CWeapon_Player::WEAPON_TYPE::SHURIKEN == static_cast<CPlayer*>(m_pOwner)->Get_CurWeaponType())
	{
		if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD ||
			m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD ||
			m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD ||
			m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();
			CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
			CPlayer::PLAYER_ANIMATIONID eID = CPlayer::PLAYER_ANIMATIONID::SH_WALK;

			if (m_pGameInstance->Get_KeyState(KEY::LSHIFT) == KEY_STATE::HOLD)
				eID = CPlayer::PLAYER_ANIMATIONID::SH_RUN;

			pModel->SetUp_Animation(eID, true);
			pFsm->Change_State(eID);

			return true;
		}
	}

	
	
	return false;
}

_bool CPlayer_Idle::Check_Dash()
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

_bool CPlayer_Idle::Check_Sh_Dash()
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

_bool CPlayer_Idle::Check_HookUp()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	if (m_pGameInstance->Get_KeyState(KEY::F) == KEY_STATE::TAP)
	{
		_vector vPlayerPos = pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		vPlayerPos = XMVectorSetY(vPlayerPos, 0.f);
		
		list<CGameObject*>& GrapUIs =  m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_GrapplingPointUI");
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

			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::HOOK_UP, -1 , pClosestGrapUI);

			return true;
		}

	}

	return false;
}

_bool CPlayer_Idle::Check_Jump()
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
			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::JUMP_START,-1, &bLandWall);
		}

		return true;
	}
	
	return false;
}

_bool CPlayer_Idle::Check_Block1()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::BLOCK_R1 != pModel->Get_CurAnimationIndex() &&
		CPlayer::PLAYER_ANIMATIONID::BLOCK_L1 != pModel->Get_CurAnimationIndex() &&
		m_pGameInstance->Get_KeyState(KEY::E) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();


		_uint iRandomValue = static_cast<_uint>(m_pGameInstance->Get_Random(0, 2));

		switch (iRandomValue)
		{
		case 0:
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::BLOCK_R1, false);
			break;
		case 1:
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::BLOCK_L1, false);
			break;
		default:
			break;
		}

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::BLOCK_R1);

		return true;
	}
	return false;
}

_bool CPlayer_Idle::Check_Block2()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::BLOCK_R2 != pModel->Get_CurAnimationIndex() &&
		CPlayer::PLAYER_ANIMATIONID::BLOCK_L2 != pModel->Get_CurAnimationIndex() &&
		m_pGameInstance->Get_KeyState(KEY::E) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();

		_uint iRandomValue = static_cast<_uint>(m_pGameInstance->Get_Random(0, 2));

		switch (iRandomValue)
		{
		case 0:
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::BLOCK_R2, false);
			break;
		case 1:
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::BLOCK_L2, false);
			break;
		default:
			break;
		}
		
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::BLOCK_R2);

		return true;
	}
	return false;
}

_bool CPlayer_Idle::Check_Block3()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::BLOCK_R3 != pModel->Get_CurAnimationIndex() &&
		CPlayer::PLAYER_ANIMATIONID::BLOCK_L3 != pModel->Get_CurAnimationIndex() &&
		m_pGameInstance->Get_KeyState(KEY::E) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();

		_uint iRandomValue = static_cast<_uint>(m_pGameInstance->Get_Random(0, 2));

		switch (iRandomValue)
		{
		case 0:
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::BLOCK_R3, false);
			break;
		case 1:
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::BLOCK_L3, false);
			break;
		default:
			break;
		}

		
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::BLOCK_R3);

		return true;
	}
	return false;
}

_bool CPlayer_Idle::Check_Attack1()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
	
	if (CPlayer::PLAYER_ANIMATIONID::ATTACK_R1 != pModel->Get_CurAnimationIndex() &&
		CPlayer::PLAYER_ANIMATIONID::ATTACK_L1 != pModel->Get_CurAnimationIndex() &&
		m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		
		_uint iRandomValue = static_cast<_uint>(m_pGameInstance->Get_Random(0, 2));

		switch (iRandomValue)
		{
		case 0:
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::ATTACK_R1, false);	
			break;
		case 1:
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::ATTACK_L1, false);
			break;
		default:
			break;
		}
	
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::ATTACK_R1);

		return true;
	}


	return false;
}

_bool CPlayer_Idle::Check_Attack2()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


	if (CPlayer::PLAYER_ANIMATIONID::ATTACK_R2 != pModel->Get_CurAnimationIndex() && 
		CPlayer::PLAYER_ANIMATIONID::ATTACK_L2 != pModel->Get_CurAnimationIndex() &&
		m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		
		_uint iRandomValue = static_cast<_uint>(m_pGameInstance->Get_Random(0, 2));

		switch (iRandomValue)
		{
		case 0:
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::ATTACK_R2, false);
			break;
		case 1:
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::ATTACK_L2, false);
			break;
		default:
			break;
		}

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::ATTACK_R2);

		return true;
	}

	return false;
}

_bool CPlayer_Idle::Check_Attack3()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::ATTACK_R3 != pModel->Get_CurAnimationIndex() &&
		CPlayer::PLAYER_ANIMATIONID::ATTACK_L3 != pModel->Get_CurAnimationIndex() &&
		m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		
		_uint iRandomValue = static_cast<_uint>(m_pGameInstance->Get_Random(0, 2));

		switch (iRandomValue)
		{
		case 0:
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::ATTACK_R3, false);
			break;
		case 1:
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::ATTACK_L3, false);
			break;
		default:
			break;
		}

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::ATTACK_R3);

		return true;
	}

	return false;
}

_bool CPlayer_Idle::Check_Sh_Attack1()
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


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_ATTACK, true, 0.f);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::SH_ATTACK);

		return true;
	}

	return false;
}

_bool CPlayer_Idle::Check_HomingShuriken()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::RIFT_PICKUP != pModel->Get_CurAnimationIndex() &&
		m_pGameInstance->Get_KeyState(KEY::Z) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
		TrackPos = 0.0;

	
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::RIFT_PICKUP);
		return true;
	}

	return false;
}

_bool CPlayer_Idle::Check_SwapWeapon()
{
	CBody_Player* pBody = static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY));
	CModel* pModel = pBody->Get_Model();

	CWeapon_Player* pWeapon = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON));

	if (m_pGameInstance->Get_KeyState(KEY::G) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();

		if (CWeapon_Player::WEAPON_TYPE::KATANA == pWeapon->Get_CurType())
		{
			m_pGameInstance->Play_Sound(TEXT("KatanaToSh.ogg"), SOUND_PLAYER, 2.f);
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_KAT_TO_SHUR, true);
		}

		else if (CWeapon_Player::WEAPON_TYPE::SHURIKEN == pWeapon->Get_CurType())
		{
			m_pGameInstance->Play_Sound(TEXT("ShToKatana.ogg"), SOUND_PLAYER, 2.f);
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_SHUR_TO_KAT, true);
		}

		return true;
	}

	// 애니메이션이 중간쯤 왔을때 무기교체라 치면

	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();
	_double Duration = pModel->Get_CurAnimation()->Get_Duration();

	if (0.7f <= TrackPos / (_float)Duration)
	{
		_uint iAniIndex = pModel->Get_CurAnimationIndex();

		if (CPlayer::PLAYER_ANIMATIONID::SH_KAT_TO_SHUR == iAniIndex)
		{
			pWeapon->Set_CurType(CWeapon_Player::WEAPON_TYPE::SHURIKEN);
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_IDLE, true);

			static_cast<CPlayer*>(m_pOwner)->Set_CanSwapWeapon(false);			// 스킬 UI들 다 안착하면 true로
		}
		else if (CPlayer::PLAYER_ANIMATIONID::SH_SHUR_TO_KAT == iAniIndex)
		{
			pWeapon->Set_CurType(CWeapon_Player::WEAPON_TYPE::KATANA);
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);

			static_cast<CPlayer*>(m_pOwner)->Set_CanSwapWeapon(false);
		}

	}



	return true;
}


_bool CPlayer_Idle::Check_CutAll()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (m_pGameInstance->Get_KeyState(KEY::Q) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::FURR_AIM_LOOP);
		return true;
	}

	return false;
}


_bool CPlayer_Idle::Check_Nami()
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

_bool CPlayer_Idle::Check_MindControl()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (m_pGameInstance->Get_KeyState(KEY::TWO) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::MIND_CONTROL_START_START);
		return true;
	}

	return false;
}

_bool CPlayer_Idle::Check_TimeStop()
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



CPlayer_Idle* CPlayer_Idle::Create(class CGameObject* pOwner)
{
	CPlayer_Idle* pInstance = new CPlayer_Idle(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Idle::Free()
{
	__super::Free();
}
