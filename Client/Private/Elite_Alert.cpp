#include "stdafx.h"
#include "Elite_Alert.h"


#include "Elite.h"
#include "GameInstance.h"
#include "Animation.h"


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

	if(m_fCanTurboActiveTime > 0.f)
		m_fCanTurboActiveTime -= fTimeDelta;
	else
	{
		if (Check_Turbo(fTimeDelta))
		{
			m_fCanTurboActiveTime = 10.f;
			return;
		}			
	}

	
	CModel* pModel = static_cast<CElite*>(m_pOwner)->Get_Model();

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
			pEliteTransform->LookAt_XZSmooth(vPlayerPos, fTimeDelta, nullptr);	
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
