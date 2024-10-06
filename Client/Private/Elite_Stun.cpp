#include "stdafx.h"
#include "Elite_Stun.h"


#include "Elite.h"
#include "GameInstance.h"
#include "Animation.h"


CElite_Stun::CElite_Stun(class CGameObject* pOwner)
	: CState{ CElite::ELITE_ANIMATION::HIT_STUN , pOwner }
{

}


HRESULT CElite_Stun::Initialize()
{
	return S_OK;
}


HRESULT CElite_Stun::Start_State(void* pArg)
{
	CModel* pModel = m_pOwner->Get_Model();
	_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();

	TrackPos = 0.0;

	m_fStunTime = 2.f;

	return S_OK;
}


void CElite_Stun::Update(_float fTimeDelta)
{
	m_pOwner->Get_Transform()->Go_Backward(fTimeDelta * 0.3f);

	static_cast<CElite*>(m_pOwner)->Set_Groggy(true);

	CModel* pModel = m_pOwner->Get_Model();
	CFsm* pFsm = m_pOwner->Get_Fsm();


	m_fStunTime -= fTimeDelta;

	if (m_fStunTime <= 0.f)
	{
		_bool	bDashBlock = true;

		pModel->SetUp_Animation(CElite::ELITE_ANIMATION::DASH_TO_ALERDLB, true);
		pFsm->Change_State(CElite::ELITE_ANIMATION::BLOCK_F, -1, &bDashBlock);
	}
}


void CElite_Stun::End_State()
{
	CElite* pElite = static_cast<CElite*>(m_pOwner);
	pElite->Set_Energy(100.f);

	static_cast<CElite*>(m_pOwner)->Set_Groggy(false);
}




CElite_Stun* CElite_Stun::Create(class CGameObject* pOwner)
{
	CElite_Stun* pInstance = new CElite_Stun(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CElite_Stun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElite_Stun::Free()
{
	__super::Free();
}
