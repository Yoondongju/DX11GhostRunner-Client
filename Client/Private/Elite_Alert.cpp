#include "stdafx.h"
#include "Elite_Alert.h"


#include "Elite.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Player.h"

CElite_Alert::CElite_Alert(class CGameObject* pOwner)
	: CState{ CElite::ELITE_ANIMATION::IDLE_TO_ALERT , pOwner }
{

}

HRESULT CElite_Alert::Initialize()
{
	return S_OK;
}

HRESULT CElite_Alert::Start_State(void* pArg)
{
	


	return S_OK;
}

void CElite_Alert::Update(_float fTimeDelta)
{
	if (Check_Death())
		return;

	CElite* pElite = static_cast<CElite*>(m_pOwner);
	CModel* pModel = pElite->Get_Model();
	CFsm* pFsm = pElite->Get_Fsm();

	if (false == pElite->IsPage2())				// Page 1
	{
		if (m_fCanTurboActiveTime > 0.f)
			m_fCanTurboActiveTime -= fTimeDelta;
		else
		{
			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
			_vector vPlayerPos = pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);

			CTransform* pEliteTransform = m_pOwner->Get_Transform();
			
			pEliteTransform->LookAt_XZ(vPlayerPos);
			
			if (m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD)
			{
				pModel->SetUp_Animation(CElite::ELITE_ANIMATION::WALK_R, true);
				pFsm->Change_State(CElite::ELITE_ANIMATION::WALK_F, STATE_DIR::RIGHT);
			}
	
			if (m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
			{
				pModel->SetUp_Animation(CElite::ELITE_ANIMATION::WALK_L, true);
				pFsm->Change_State(CElite::ELITE_ANIMATION::WALK_F, STATE_DIR::LEFT);
			}

		}
	}
	else                                        // Page 2
	{
		if (m_fCanTurboActiveTime > 0.f)
			m_fCanTurboActiveTime -= fTimeDelta;
		else
		{
			if (Check_Turbo(fTimeDelta))
			{
				m_fCanTurboActiveTime = 0.4f;
				return;
			}
		}
	}

	

	

	
	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();

	if (0.9 <= TrackPos / (_float)Duration)
	{
		pModel->SetUp_Animation(CElite::ELITE_ANIMATION::ALERT_LOOP, true);
	}

}

void CElite_Alert::End_State()
{
	m_isAttackActive = false;
}

_bool CElite_Alert::Check_Death()
{
	return _bool();
}

_bool CElite_Alert::Check_Turbo(_float fTimeDelta)
{
	// 터보가 발동되는조건 
	// 거리안에 들어오고 +    

	_vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	vPlayerPos = XMVectorSetY(vPlayerPos, 0.0f);  // Y축 성분 제거
	
	CTransform* pEliteTransform = m_pOwner->Get_Transform();
	_vector vElitePos = pEliteTransform->Get_State(CTransform::STATE_POSITION);
	vElitePos = XMVectorSetY(vElitePos, 0.0f);  // Y축 성분 제거


	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(vPlayerPos, vElitePos)));

	
	if (true == m_isAttackActive || m_fCanTurboDistance >= fDistance)
	{
		m_isAttackActive = true;

		_vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		
		CTransform* pEliteTransform = m_pOwner->Get_Transform();
		_vector vElitePos = pEliteTransform->Get_State(CTransform::STATE_POSITION);
		
		_vector vEliteLookNor = XMVector3Normalize(pEliteTransform->Get_State(CTransform::STATE_LOOK));
		
		_vector vEliteToPlayerDir = XMVectorSubtract(vPlayerPos, vElitePos);
		vEliteToPlayerDir = XMVectorSetY(vEliteToPlayerDir, 0.0f);  // Y축 성분 제거
		_vector vEliteToPlayerDirNor = XMVector3Normalize(vEliteToPlayerDir);
		
		_float fDot = XMVectorGetX(XMVector3Dot(vEliteLookNor, vEliteToPlayerDirNor));
		
		_float fAngle = XMConvertToDegrees(acos(fDot));
		
		if (1.f < fAngle)
		{
			pEliteTransform->LookAt_XZSmooth(vPlayerPos, fTimeDelta * 2.f, nullptr);	
		}	
		else
		{	
			CFsm* pFsm = m_pOwner->Get_Fsm();
			pFsm->Change_State(CElite::ELITE_ANIMATION::ALERT_TO_TURBO);

			return true;
		}	
	}
	
	return false;
}




CElite_Alert* CElite_Alert::Create(class CGameObject* pOwner)
{
	CElite_Alert* pInstance = new CElite_Alert(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CElite_Alert"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElite_Alert::Free()
{
	__super::Free();
}
