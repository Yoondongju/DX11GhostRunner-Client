#include "stdafx.h"
#include "Player_JumpFall.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
#include "Animation.h"

#include "GrapplingPointUI.h"

#include "EventNotify.h"

CPlayer_JumpFall::CPlayer_JumpFall(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::JUMP_LOOP , pOwner }
{

}

HRESULT CPlayer_JumpFall::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_JumpFall::Start_State(void* pArg)
{


	return S_OK;
}

void CPlayer_JumpFall::Update(_float fTimeDelta)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	if (Check_HookUp())
		return;

	if (Check_Jump())
		return;


	if (pPlayer->IsNamiActive() && Check_Nami())
		return;
	else
	{
		if (m_pGameInstance->Get_KeyState(KEY::ONE) == KEY_STATE::TAP)
		{
			static_cast<CEventNotify*>(m_pGameInstance->Find_Notify(LEVEL_GAMEPLAY))->Set_Active(true, CEventNotify::TEXT_EVENT::UNABLE_SKILL_COOLTIME);
		}
	}


	

	CTransform* pTransform = pPlayer->Get_Transform();
	pPlayer->Set_JumpProgressTime(m_fAccTime);	// 점프 진행시간이 누적된걸 던져


	_float fLandY = pPlayer->Get_LandPosY();
	_float fOffSetY = pPlayer->Get_OffsetY();


	CModel* pModel = pPlayer->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
	CFsm* pFsm = m_pOwner->Get_Fsm();

	if (true == m_isFirstCallLoop && (fLandY + fOffSetY) * 4 > XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION)))
	{
		m_isFirstCallLoop = false;

		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::JUMP_LOOP, true);	
	}
	else if (true == m_isFirstCallEnd && 0.1f >= fabs((fLandY + fOffSetY) - XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION))))
	{
		m_isFirstCallEnd = false;


		if(CWeapon_Player::WEAPON_TYPE::KATANA ==  pPlayer->Get_CurWeaponType())
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);	
		else
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_IDLE, true);

		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);		
	}

}

void CPlayer_JumpFall::End_State()
{
	m_isFirstCallLoop = true;
	m_isFirstCallEnd = true;
}



_bool CPlayer_JumpFall::Check_HookUp()
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

_bool CPlayer_JumpFall::Check_Nami()
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

_bool CPlayer_JumpFall::Check_Jump()
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



CPlayer_JumpFall* CPlayer_JumpFall::Create(class CGameObject* pOwner)
{
	CPlayer_JumpFall* pInstance = new CPlayer_JumpFall(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_JumpFall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_JumpFall::Free()
{
	__super::Free();
}
