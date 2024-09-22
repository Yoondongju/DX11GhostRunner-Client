#include "stdafx.h"
#include "Elite_Turbo.h"


#include "Elite.h"
#include "GameInstance.h"
#include "Animation.h"


CElite_Turbo::CElite_Turbo(class CGameObject* pOwner)
	: CState{ CElite::ELITE_ANIMATION::ALERT_TO_TURBO , pOwner }
{

}

HRESULT CElite_Turbo::Initialize()
{
	return S_OK;
}

HRESULT CElite_Turbo::Start_State(void* pArg)
{


	return S_OK;
}

void CElite_Turbo::Update(_float fTimeDelta)
{
	if (Check_Death())
		return;

	if (true == m_bStartTurboDash && Check_TurboDash(fTimeDelta))
		return;

	CModel* pModel = static_cast<CElite*>(m_pOwner)->Get_Model();

	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();

	

	if (CElite::ELITE_ANIMATION::ALERT_TO_TURBO == pModel->Get_CurAnimationIndex() &&
		0.9 <= TrackPos / (_float)Duration)
	{		
		pModel->SetUp_Animation(CElite::ELITE_ANIMATION::TURBO_LOOP, true);
	}
	else if (CElite::ELITE_ANIMATION::TURBO_LOOP == pModel->Get_CurAnimationIndex())
	{
		_vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		CTransform* pEliteTransform = m_pOwner->Get_Transform();
		_vector vElitePos = pEliteTransform->Get_State(CTransform::STATE_POSITION);

		_vector vEliteLookNor = XMVector3Normalize(pEliteTransform->Get_State(CTransform::STATE_LOOK));

		_vector vEliteToPlayerDir = XMVectorSubtract(vPlayerPos, vElitePos);
		vEliteToPlayerDir = XMVectorSetY(vEliteToPlayerDir, 0.0f);  // Y축 성분 제거
		_vector vEliteToPlayerDirNor = XMVector3Normalize(vEliteToPlayerDir);

		_float fDot = XMVectorGetX(XMVector3Dot(vEliteLookNor, vEliteToPlayerDirNor));

		_float fAngle = XMConvertToDegrees(acos(fDot));

		if( 1.f < fAngle)
			pEliteTransform->LookAt_XZSmooth(vPlayerPos, fTimeDelta * 2, nullptr);
		else
			m_bStartTurboDash = true;
		
		
	}





	else if (CElite::ELITE_ANIMATION::TURBO_TO_DASH == pModel->Get_CurAnimationIndex() &&
		0.9 <= TrackPos / (_float)Duration)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pModel->SetUp_Animation(CElite::ELITE_ANIMATION::WALK_B, true);
		pFsm->Change_State(CElite::ELITE_ANIMATION::WALK_F,STATE_DIR::FRONT);
	}




}

void CElite_Turbo::End_State()
{
	m_bStartTurboDash = false;
	m_fAccSpeed = 0.f;
}


_bool CElite_Turbo::Check_Death()
{
	return _bool();
}

_bool CElite_Turbo::Check_TurboDash(_float fTimeDelta)
{
	_vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	
	CTransform* pEliteTransform = m_pOwner->Get_Transform();
	_vector vElitePos = pEliteTransform->Get_State(CTransform::STATE_POSITION);

	_vector vInterval =  XMVectorSubtract(vPlayerPos, vElitePos);

	_float fDistance = XMVectorGetX(XMVector3Length(vInterval));
	

	if (40.f >= fDistance)
	{
		CModel* pModel = static_cast<CElite*>(m_pOwner)->Get_Model();
		pModel->SetUp_Animation(CElite::ELITE_ANIMATION::TURBO_TO_DASH, true);
	}
	else
	{	
		m_pOwner->Get_Transform()->LookAt_XZSmooth(vPlayerPos, fTimeDelta, nullptr);

		m_fAccSpeed += fTimeDelta;
		pEliteTransform->Go_Dir_XZ(XMVector3Normalize(vInterval), m_fAccSpeed * 0.4f);
	}





	return false;
}




CElite_Turbo* CElite_Turbo::Create(class CGameObject* pOwner)
{
	CElite_Turbo* pInstance = new CElite_Turbo(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CElite_Turbo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElite_Turbo::Free()
{
	__super::Free();
}
