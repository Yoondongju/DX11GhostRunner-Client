#include "stdafx.h"
#include "Player_Run.h"

#include "Player.h"
#include "Body_Player.h"

#include "GameInstance.h"
#include "GrapplingPointUI.h"

#include "Animation.h"
#include "SwordTrail.h"

#include "EventNotify.h"

CPlayer_Run::CPlayer_Run(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::RUN ,pOwner }
{

}

HRESULT CPlayer_Run::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Run::Start_State(void* pArg)
{
	_float fSpeedPerSec = m_pOwner->Get_Transform()->Get_SpeedPerSec();

	m_pOwner->Get_Transform()->Set_SpeedPerSec(fSpeedPerSec * 1.8);


	if (LEVEL_STAGE2_BOSS == g_CurLevel)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
		m_pGameInstance->ActiveDistortion(pPlayer->Get_WaterPuddleDistortionTex(), CRenderer::DISTORTION_TYPE::WATERPUDDLE, pPlayer->Get_WaterPuddle_WorldMatrix());
	}
	


	return S_OK;
}


void CPlayer_Run::Update(_float fTimeDelta)
{
	m_fSoundTimer += fTimeDelta;
	if (m_fSoundTimer > 0.2f)
	{
		m_pGameInstance->Play_Sound(TEXT("walk.ogg"), SOUND_PLAYER, g_fEffectVolume);
		m_fSoundTimer = 0.f;
	}
	

	m_fSwordSpinTime -= fTimeDelta;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (m_fSwordSpinTime < 0.f)
	{
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SPIN_A, true);	
		m_fSwordSpinTime = 5.f;
		m_fFristSpinCall = true;

	}
	else
	{
		if (CPlayer::PLAYER_ANIMATIONID::SPIN_A == pModel->Get_CurAnimationIndex())
		{
			_double Duration = pModel->Get_CurAnimation()->Get_Duration();
			const _double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();

			if (true == m_fFristSpinCall && 0.8f <= (TrackPos / Duration))
			{
				CFsm* pFsm = m_pOwner->Get_Fsm();

				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::RUN, true);
				m_fFristSpinCall = false;

			}
		}
	}


	if (m_pGameInstance->Get_KeyState(KEY::LSHIFT) == KEY_STATE::NONE)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
	
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::WALK);
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::WALK, true);
	}


	if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::NONE)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
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


	if (Check_HookUp())
		return;
	
	if (Check_Jump())
		return;
}


void CPlayer_Run::End_State()
{
	_float fOriginSpeedPerSec = m_pOwner->Get_Transform()->Get_OriginSpeedPerSec();
	m_pOwner->Get_Transform()->Set_SpeedPerSec(fOriginSpeedPerSec);

	
	m_pGameInstance->UnActiveDistortion();
}


_bool CPlayer_Run::Check_HookUp()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	if (m_pGameInstance->Get_KeyState(KEY::F) == KEY_STATE::TAP)
	{
		_vector vPlayerPos = pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		vPlayerPos = XMVectorSetY(vPlayerPos, 0.f);

		list<CGameObject*>& GrapUIs = m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_GrapplingPointUI");
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

_bool CPlayer_Run::Check_Jump()
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

		if (0.1f >= fabs((fLandY + fOffSetY) - XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION))))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();
			CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::JUMP_START);
			
		}

		return true;
	}

	return false;
}

_bool CPlayer_Run::Check_Block1()
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

_bool CPlayer_Run::Check_Block2()
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

_bool CPlayer_Run::Check_Block3()
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

_bool CPlayer_Run::Check_Attack1()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::ATTACK_R1 != pModel->Get_CurAnimationIndex() &&
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

_bool CPlayer_Run::Check_Attack2()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::ATTACK_R2 != pModel->Get_CurAnimationIndex() &&
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

_bool CPlayer_Run::Check_Attack3()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	if (CPlayer::PLAYER_ANIMATIONID::ATTACK_R3 != pModel->Get_CurAnimationIndex() &&
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

_bool CPlayer_Run::Check_CutAll()
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

_bool CPlayer_Run::Check_Nami()
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

_bool CPlayer_Run::Check_TimeStop()
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



CPlayer_Run* CPlayer_Run::Create(class CGameObject* pOwner)
{
	CPlayer_Run* pInstance = new CPlayer_Run(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Run"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Run::Free()
{
	__super::Free();
}
