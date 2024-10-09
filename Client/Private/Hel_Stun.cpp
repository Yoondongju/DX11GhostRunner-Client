#include "stdafx.h"
#include "Hel_Stun.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

CHel_Stun::CHel_Stun(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::STUN_HIT , pOwner }
{

}

HRESULT CHel_Stun::Initialize()
{
	return S_OK;
}

HRESULT CHel_Stun::Start_State(void* pArg)
{
	CModel* pModel = m_pOwner->Get_Model();
	_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
	TrackPos = 0.0;

	return S_OK;
}


void CHel_Stun::Update(_float fTimeDelta)
{
	m_pOwner->Get_Transform()->Go_Backward(fTimeDelta * 0.1f);

	static_cast<CHel*>(m_pOwner)->Set_Groggy(true);

	CModel* pModel = m_pOwner->Get_Model();
	CFsm* pFsm = m_pOwner->Get_Fsm();
	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();

	if (0.9f < (_float)TrackPos / Duration)
	{
		CModel* pModel = m_pOwner->Get_Model();
		CFsm* pFsm = m_pOwner->Get_Fsm();


		pModel->SetUp_Animation(CHel::HEL_ANIMATION::IDLE_TO_JUMP, true);
		pFsm->Change_State(CHel::HEL_ANIMATION::IDLE_TO_JUMP);
	}
}


void CHel_Stun::End_State()
{
	CHel* pHel = static_cast<CHel*>(m_pOwner);
	pHel->Set_Energy(100.f);

	static_cast<CHel*>(m_pOwner)->Set_Groggy(false);
}


CHel_Stun* CHel_Stun::Create(class CGameObject* pOwner)
{
	CHel_Stun* pInstance = new CHel_Stun(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Stun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Stun::Free()
{
	__super::Free();
}
