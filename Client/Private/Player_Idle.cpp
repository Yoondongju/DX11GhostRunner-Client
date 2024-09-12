#include "stdafx.h"
#include "Player_Idle.h"

#include "Player.h"

#include "Body_Player.h"
#include "Weapon_Player.h"

#include "GameInstance.h"
#include "GrapplingPointUI.h"

#include "Animation.h"

CPlayer_Idle::CPlayer_Idle(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::IDLE , pOwner }
{

}

HRESULT CPlayer_Idle::Initialize()
{
    return S_OK;
}

HRESULT CPlayer_Idle::Start_State()
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
		}
		break;
		case 1:
		{
			if (Check_Attack2())
				return;
			if (pPlayer->IsBlockActive() && Check_Block2())
				return;
		}
		break;
		case 2:
		{
			if (Check_Attack3())
				return;
			if (pPlayer->IsBlockActive() && Check_Block3())
				return;
		}
		break;
		default:
			break;
		}


		if (pPlayer->IsDashActive() && Check_Dash())
		{
			return;
		}

		// SKILL
		if (pPlayer->IsCutAllActive() && Check_CutAll())
			return;
		if (Check_Nami())
			return;
		if (Check_MindControl())
			return;
		if (Check_TimeStop())
			return;
	}

	else if (CWeapon_Player::WEAPON_TYPE::SHURIKEN == static_cast<CPlayer*>(m_pOwner)->Get_CurWeaponType())
	{
		if (Check_Sh_Attack1())
			return;

		if (pPlayer->IsDashActive() && Check_Sh_Dash())
		{
			return;
		}
			
	}



	if (Check_HookUp())
		return;

	if (Check_Jump())
		return;

	if (Check_RunAndWalk())
		return;

	if (Check_SwapWeapon())
		return;

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
	if (false == pPlayer->Get_GrapplingPoint()->Get_FindPlayer())
		return false;

	if (m_pGameInstance->Get_KeyState(KEY::F) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

		if (CWeapon_Player::WEAPON_TYPE::KATANA == pPlayer->Get_CurWeaponType())
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::HOOK_UP, false);
		else if(CWeapon_Player::WEAPON_TYPE::SHURIKEN == pPlayer->Get_CurWeaponType())
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::HOOK_UP, false);

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::HOOK_UP);	

		return true;
	}

	return false;
}

_bool CPlayer_Idle::Check_Jump()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP)
	{
		CTransform* pTransform = m_pOwner->Get_Transform();
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

		_float fLandY = pPlayer->Get_LandPosY();
		_float fOffSetY = pPlayer->Get_OffsetY();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


		if (fLandY + fOffSetY == XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION)))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();
			CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::JUMP_START);
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
		m_pGameInstance->Get_KeyState(KEY::E) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::BLOCK_R1, false);
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
		m_pGameInstance->Get_KeyState(KEY::E) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::BLOCK_R2, false);
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
		m_pGameInstance->Get_KeyState(KEY::E) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::BLOCK_R3, false);
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
		m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		

		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::ATTACK_R1, false);	// Change_State보다 먼저 세팅해줘야함 Hook이나 Attack같은 같은 State를 공유하는녀석일 경우
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
		m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::ATTACK_R2, false);	// Change_State보다 먼저 세팅해줘야함 Hook이나 Attack같은 같은 State를 공유하는녀석일 경우
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
		m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		

		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::ATTACK_R3, false);	// Change_State보다 먼저 세팅해줘야함 Hook이나 Attack같은 같은 State를 공유하는녀석일 경우
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


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_ATTACK, false, 0.f);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::SH_ATTACK);

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
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_KAT_TO_SHUR, false);			
		}

		else if (CWeapon_Player::WEAPON_TYPE::SHURIKEN == pWeapon->Get_CurType())
		{
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_SHUR_TO_KAT,false);
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
		}
		else if (CPlayer::PLAYER_ANIMATIONID::SH_SHUR_TO_KAT == iAniIndex)
		{
			pWeapon->Set_CurType(CWeapon_Player::WEAPON_TYPE::KATANA);
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
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

		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::FURR_AIM_TRICUT, false);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::FURR_AIM_TRICUT);

		return true;
	}

	return false;
}


_bool CPlayer_Idle::Check_Nami()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (m_pGameInstance->Get_KeyState(KEY::TWO) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::NAMI_IDLE_TO_AIM, false);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::NAMI_IDLE_TO_AIM);

		return true;
	}

	return false;
}

_bool CPlayer_Idle::Check_MindControl()
{
	return _bool();
}

_bool CPlayer_Idle::Check_TimeStop()
{
	return _bool();
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
