#include "stdafx.h"
#include "Mira_WalkIn.h"


#include "Mira.h"
#include "GameInstance.h"
#include "Animation.h"


CMira_WalkIn::CMira_WalkIn(class CGameObject* pOwner)
	: CState{ CMira::MIRA_ANIMATION::WALKIN_AROUND , pOwner }
{

}

HRESULT CMira_WalkIn::Initialize()
{
	return S_OK;
}

HRESULT CMira_WalkIn::Start_State()
{


	return S_OK;
}

void CMira_WalkIn::Update(_float fTimeDelta)
{
	CModel* pModel = m_pOwner->Get_Model();

    _double         Duration = pModel->Get_CurAnimation()->Get_Duration();
    const _double&  TrackPos = pModel->Get_Referene_CurrentTrackPosition();
    
    if (0.9f <= TrackPos / Duration)
    {
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pModel->SetUp_Animation(CMira::MIRA_ANIMATION::IDLE, true);
		pFsm->Change_State(CMira::MIRA_ANIMATION::IDLE);
    }
}

void CMira_WalkIn::End_State()
{

}


CMira_WalkIn* CMira_WalkIn::Create(class CGameObject* pOwner)
{
	CMira_WalkIn* pInstance = new CMira_WalkIn(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMira_WalkIn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMira_WalkIn::Free()
{
	__super::Free();
}
