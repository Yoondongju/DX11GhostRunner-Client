#include "stdafx.h"
#include "Player_Sh_Run.h"

#include "Player.h"
#include "Body_Player.h"

#include "GameInstance.h"
#include "GrapplingPointUI.h"

CPlayer_Sh_Run::CPlayer_Sh_Run(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::SH_RUN ,pOwner }
{

}

HRESULT CPlayer_Sh_Run::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Sh_Run::Start_State(void* pArg)
{
	_float fSpeedPerSec = m_pOwner->Get_Transform()->Get_SpeedPerSec();

	m_pOwner->Get_Transform()->Set_SpeedPerSec(fSpeedPerSec * 2.2f);


	return S_OK;
}


void CPlayer_Sh_Run::Update(_float fTimeDelta)
{
	m_fSoundTimer += fTimeDelta;
	if (m_fSoundTimer > 0.2f)
	{
		m_pGameInstance->Play_Sound(TEXT("walk.ogg"), SOUND_PLAYER, g_fEffectVolume);
		m_fSoundTimer = 0.f;
	}


	if (m_pGameInstance->Get_KeyState(KEY::LSHIFT) == KEY_STATE::NONE)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::SH_WALK);
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_WALK, true, 0.f);
	}



	if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::NONE &&
		m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::NONE)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_IDLE, true, 0.f);
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);	
	}

	//if (Check_Sh_Attack1())
	//	return;

	if (Check_HookUp())
		return;

	if (Check_Jump())
		return;

}


void CPlayer_Sh_Run::End_State()
{
	_float fOriginSpeedPerSec = m_pOwner->Get_Transform()->Get_OriginSpeedPerSec();

	m_pOwner->Get_Transform()->Set_SpeedPerSec(fOriginSpeedPerSec);
}


_bool CPlayer_Sh_Run::Check_HookUp()
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

_bool CPlayer_Sh_Run::Check_Jump()
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

_bool CPlayer_Sh_Run::Check_Sh_Attack1()
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



CPlayer_Sh_Run* CPlayer_Sh_Run::Create(class CGameObject* pOwner)
{
	CPlayer_Sh_Run* pInstance = new CPlayer_Sh_Run(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Sh_Run"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Sh_Run::Free()
{
	__super::Free();
}
