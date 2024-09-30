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


	return S_OK;
}


void CElite_Stun::Update(_float fTimeDelta)
{
	m_pOwner->Get_Transform()->Go_Backward(fTimeDelta * 1.5f);

	static_cast<CElite*>(m_pOwner)->Set_Groggy(true);



	CModel* pModel = m_pOwner->Get_Model();
	CFsm* pFsm = m_pOwner->Get_Fsm();

	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();


	if (CElite::ELITE_ANIMATION::HIT_STUN == pModel->Get_CurAnimationIndex() &&
		0.9 <= TrackPos / (_float)Duration)
	{
		_bool	bDashBlock = true;

		pModel->SetUp_Animation(CElite::ELITE_ANIMATION::DASH_TO_ALERDLB, true);
		pFsm->Change_State(CElite::ELITE_ANIMATION::BLOCK_F, -1, &bDashBlock);
	}
}


void CElite_Stun::End_State()
{
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
