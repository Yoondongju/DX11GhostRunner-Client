#include "stdafx.h"
#include "Player_Jump.h"

#include "Player.h"
#include "Body_Player.h"

#include "GameInstance.h"

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

	// 점프 막 시작 이니 ?   or  점프 중이니  or  점프 끝났니 
	
	
	if(m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP &&
		CPlayer::PLAYER_ANIMATIONID::JUMP_START == pModel->Get_NextAnimationIndex())
		pRigidBody->Add_Force_Direction(pTransform->Get_State(CTransform::STATE::STATE_UP), 2000, Engine::CRigidBody::VELOCITYCHANGE);


	return S_OK;
}

void CPlayer_Jump::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;

	CTransform* pTransform = m_pOwner->Get_Transform();
	_float fOffSetY = static_cast<CPlayer*>(m_pOwner)->Get_OffsetY();

	if ((fOffSetY * 3) >= XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION)))
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

		if (CPlayer::PLAYER_ANIMATIONID::JUMP_START == pModel->Get_CurAnimationIndex() ||
			CPlayer::PLAYER_ANIMATIONID::JUMP_LOOP == pModel->Get_CurAnimationIndex())
		{
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::JUMP_END, false, CPlayer::PLAYER_ANIMATIONID::IDLE);
			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
		}
			
	}


	Check_Dash();
	Check_HookUp();
}

void CPlayer_Jump::End_State()
{
	m_fAccTime = 0.f;
}


void CPlayer_Jump::Check_HookUp()
{
	if (m_pGameInstance->Get_KeyState(KEY::F) == KEY_STATE::TAP)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::HOOK_UP, false, CPlayer::PLAYER_ANIMATIONID::IDLE);	// Change_State보다 먼저 세팅해줘야함 Hook이나 Attack같은 같은 State를 공유하는녀석일 경우
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::HOOK_UP);
	}
}

void CPlayer_Jump::Check_Dash()
{
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
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::DASH_F, true);

				m_fAccTime = 0.f;
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
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::DASH_B, true);

				m_fAccTime = 0.f;
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
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::DASH_L, true);

				m_fAccTime = 0.f;
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
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::DASH_R, true);

				m_fAccTime = 0.f;
			}
			m_fLastKeyPressTimeD = m_fAccTime;
		}
	}
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
